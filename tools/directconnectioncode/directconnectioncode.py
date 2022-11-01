from argparse import ArgumentParser
import re
import socket
import struct


def parse_ip(ip: str) -> str:
    assert not re.search(
        "[^0123456789.]", ip), "Unsupported character found in IP address"

    octets = ip.split('.')
    assert len(octets) == 4, "IP address has unexpected octets: accepts x.x.x.x"

    for i, j in enumerate(octets):
        j = int(j)
        assert j >= 0 and j < 256, f"IP address octet {i + 1} exceeds expected range: 0 - 255"

    ip = socket.inet_aton(ip)
    return ip


def generate(ip: str, port: int, passcode: int):
    assert port >= 1024 and port < 65536, "Port exceeds accepted range: 1024 - 65535"
    assert passcode >= 0 and passcode < 2048, "Passcode exceeds accepted range: 0 - 2047"

    ip = parse_ip(ip)
    port = struct.pack(">H", port)
    passcode = struct.pack(">H", passcode)

    hex_code = passcode + port + ip
    direct_code = str(struct.unpack(">Q", hex_code)[0])

    direct_code = "0" * (18 - len(direct_code)) + direct_code
    direct_code = direct_code[:6] + '-' + \
        direct_code[6:12] + '-' + direct_code[-6:]
    
    print(direct_code)


if __name__ == '__main__':
    parser = ArgumentParser()
    parser.add_argument('ip', type=str)
    parser.add_argument('port', type=int)
    parser.add_argument('passcode', type=int)
    args = parser.parse_args()

    generate(args.ip, args.port, args.passcode)
