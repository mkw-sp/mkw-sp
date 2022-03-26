from dataclasses import dataclass
import socket
import struct
from pathlib import Path, PurePosixPath
import os
import sys

import json
from colorama import Fore, Style


def encode_string_fixed(string, encoding, length, null_terminate=False):
    return string[:length-null_terminate].encode(encoding).ljust(length, b'\0')

def IdentifyV1(conn=None, cc="?"):
    print(f"{Fore.GREEN}Connected to {cc}{Style.RESET_ALL}")
    if conn:
        response = f"Server: {sys.version}"
        conn.sendall(encode_string_fixed(response, 'utf-8', 256))

HANDLERS = {
    'IdentifyV1': lambda args: IdentifyV1(**args),
}

#
# Config
#
PORT = 1234
ROOTS = [
    os.path.abspath(".\\out"),
]

print(f"PORT = {PORT}")
print(f"ROOTS = {','.join(ROOTS)}")

#
# Constants
#
PACKET_TYPE_NETFILE_OPEN = 0
PACKET_TYPE_NETFILE_READ = 1
PACKET_TYPE_NETDIR_READ = 4

PACKET_TYPE_JSON = 11

NODE_TYPE_NONE = 0
NODE_TYPE_FILE = 1
NODE_TYPE_DIR = 2

MAX_PATH_LEN = 128


@dataclass
class Packet:
    packet_len: int
    packet_type: int
    file_id: int


@dataclass
class OpenPacket:
    path: str


@dataclass
class OpenResponsePacket:
    file_id: int
    file_size: int

    def sendall(self, conn):
        conn.sendall(struct.pack("!II", self.file_id, self.file_size))


@dataclass
class ReadPacket:
    read_ofs: int
    read_len: int


@dataclass
class ReadDirResponsePacket:
    node_type: int
    path: str

    def sendall(self, conn):
        bytes_node_type = struct.pack("!I", self.node_type)
        bytes_path = encode_string_fixed(
            self.path, 'utf-16be', MAX_PATH_LEN * 2, null_terminate=True)
        conn.sendall(bytes_node_type + bytes_path)


def read_packet(conn):
    packet_raw = conn.recv(12)
    if not packet_raw:
        return None
    return Packet(*struct.unpack("!III", packet_raw))


def read_open_packet(conn):
    raw_path = conn.recv(MAX_PATH_LEN * 2)
    try:
        path = raw_path.split(b'\x00\x00')[0].decode('utf-16be')
    except:
        print("Invalid path")
        return None
    return OpenPacket(path)


def read_read_packet(conn):
    return ReadPacket(*struct.unpack("!II", conn.recv(8)))

# https://security.openstack.org/guidelines/dg_using-file-paths.html


def is_safe_path(basedir, path, follow_symlinks=True):
    if follow_symlinks:
        matchpath = os.path.realpath(path)
    else:
        matchpath = os.path.abspath(path)
    # print(basedir)
    # print(os.path.commonpath((basedir, matchpath)))
    return basedir == os.path.commonpath((basedir, matchpath))


def read_from_host(path_string):
    path = PurePosixPath(path_string)

    if path.parts[0:2] != ('/', 'mkw-sp'):
        print(
            f"[read_from_host] Rejecting path {path}: does not start with /mkw-sp")
        return None

    sub_path = PurePosixPath(*path.parts[2:])

    for root in ROOTS:
        new_path = Path(root) / sub_path

        if not is_safe_path(root, new_path):
            print("Rejecting unsafe path: {}".format(path_string))
            return None

        try:
            with open(new_path, 'rb') as f:
                return f.read()
        except FileNotFoundError:
            continue
        except PermissionError:
            l = [(f, os.path.isdir(os.path.join(new_path, f)))
                 for f in os.listdir(new_path)]
            print(l)
            return l

    return None


class NetFile:
    def __init__(self, path):
        self.file = read_from_host(path)
        self.path = path

    def close(self):
        pass

    def file_size(self):
        if not self.file:
            return 0

        return len(self.file)

    def read(self, ofs, lenf):
        # print("READ: OFS = %s, LEN = %s, REALLEN = %i" % (ofs, lenf, len(self.file)))
        return self.file[ofs:ofs+lenf]

    def read_dir(self):
        if not self.file:
            return None

        path, is_dir = self.file[0]
        self.file = self.file[1:]
        return path, is_dir


class NetFileCache:
    def __init__(self, max_open_files=8):
        self.fileid_to_path = {}
        self.max_open_files = max_open_files
        self.open_netfiles = {}

    def close(self):
        for path in self.open_netfiles:
            print(f"Closing {path}")
            self.open_netfiles[path].close()

    def open_file(self, path, file_id):
        if file_id in self.fileid_to_path:
            pass  # raise RuntimeError("Duplicate netfile id")

        if path in self.open_netfiles:
            print(f"Reopening {path}")
            self.open_netfiles[path].close()

        self.fileid_to_path[file_id] = path
        self.open_netfiles[path] = NetFile(path)

        return self.open_netfiles[path].file_size()

    def read_file(self, file_id, ofs, len):
        file = self.get_raw(file_id)
        res = file.read(ofs, len)
        return res

    def get_raw(self, file_id):
        if file_id not in self.fileid_to_path:
            raise RuntimeError("File isnt open")

        path = self.fileid_to_path[file_id]
        return self.open_netfiles[path]


class Server:
    def __init__(self):
        self.file_cache = NetFileCache()

    def close(self):
        self.file_cache.close()

    def on_open_packet(self, conn, packet: Packet):
        open_packet = read_open_packet(conn)
        if open_packet is None:
            print("Didnt recieve open packet")
            OpenResponsePacket(packet.file_id, 0).sendall(conn)
            return

        print(f"[{packet.file_id}] Opening {open_packet.path}")
        file_size = self.file_cache.open_file(open_packet.path, packet.file_id)
        print(f"[{packet.file_id}] Filesize: {file_size}")
        OpenResponsePacket(packet.file_id, file_size).sendall(conn)

    def on_read_packet(self, conn, packet: Packet):
        read_packet = read_read_packet(conn)
        data = self.file_cache.read_file(
            packet.file_id, read_packet.read_ofs, read_packet.read_len)
        if not data:
            print("NONE %s" % self.file_cache.fileid_to_path[packet.file_id])
            conn.sendall(b'\0' * read_packet.len)
            return
        assert len(data) == read_packet.read_len
        conn.sendall(data)

    def on_packet(self, conn, packet: Packet):
        if packet.packet_type == PACKET_TYPE_NETFILE_OPEN:
            self.on_open_packet(conn, packet)
        elif packet.packet_type == PACKET_TYPE_NETFILE_READ:
            self.on_read_packet(conn, packet)
        elif packet.packet_type == PACKET_TYPE_NETDIR_READ:
            entry = self.file_cache.get_raw(packet.file_id)
            q = entry.read_dir()

            if q:
                read_res, is_dir = q
                response = ReadDirResponsePacket(
                    NODE_TYPE_DIR if is_dir else NODE_TYPE_FILE, read_res)
            else:
                response = ReadDirResponsePacket(NODE_TYPE_NONE, "<NONE>")
            response.sendall(conn)
        elif packet.packet_type == PACKET_TYPE_JSON:
            len = struct.unpack("!I", conn.recv(4))[0]
            is_utf16 = len & 0x8000_0000
            len &= ~0x8000_0000
            characters = conn.recv(len).decode(
                'utf-16be' if is_utf16 else 'utf-8')
            fmt = '{' + characters + '}'
            # print(fmt)
            j = json.loads(fmt)
            for handler in j:
                if handler in HANDLERS:
                    j[handler]['conn'] = conn
                    HANDLERS[handler](j[handler])
        else:
            print("Unknown packet")


def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('', PORT))
    s.listen(1)

    dns = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    dns.connect(("8.8.8.8", 80))
    print("Listening on {}:{} configured to {}".format(
        dns.getsockname()[0], s.getsockname()[1], s.getsockname()))
    dns.close()

    conn, addr = s.accept()
    print(f"Connected to {addr}")

    the_server = Server()

    while True:
        packet = read_packet(conn)
        if not packet:
            print("Shutdown")
            break
        the_server.on_packet(conn, packet)
    print("...Shutting down")
    the_server.close()
    conn.close()


if __name__ == '__main__':
    main()
