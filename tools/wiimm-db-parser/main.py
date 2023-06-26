import contextlib
import csv
import os
from typing import Generator

import slot_lookup
from TrackPacks_pb2 import ProtoSha1, AliasDB, ProtoTrack as Track

AliasValue = AliasDB.AliasValue

class ShaTrack:
    sha1: ProtoSha1
    inner: Track

    def __init__(self, sha1, inner):
        self.sha1 = sha1
        self.inner = inner

def track_from_csv(line: list[str]) -> ShaTrack | None:
    sha1, wiimmId, _, _, ctype, slot, music_slot, _, _, prefix, name, _, _, _, _, _ = line

    if len(prefix) != 0:
        name = f"{prefix} {name}"

    if slot == "0":
        return None

    if ctype == "1":
        course_id = slot_lookup.race[int(slot)]
    elif ctype == "2":
        course_id = slot_lookup.battle[int(slot)]
    else:
        raise ValueError(f"Unknown ctype of {ctype} found for track: {name}")

    return ShaTrack(
        sha1=ProtoSha1(data=bytes.fromhex(sha1)),
        inner=Track(
            name=name[:40],
            courseId=course_id,
            musicId=int(music_slot),
            wiimmId=int(wiimmId)
        )
    )

def read_wiimm_csv(path: str) -> Generator[ShaTrack, None, None]:
    with open(path) as db_csv:
        db_csv.readline()

        reader = csv.reader(db_csv, delimiter="|")
        for line in reader:
            track = track_from_csv(line)
            if track is not None:
                yield track

def read_aliases_csv(tracks: list[ShaTrack]) -> list[AliasValue]:
    aliases = []
    tracks_sha1 = {t.inner.wiimmId: t.sha1 for t in tracks}

    with open("in/sha1-reference.txt") as alias_csv:
        for _ in range(10):
            alias_csv.readline()

        reader = csv.reader(alias_csv, delimiter="|")
        for line in reader:
            if line == [] or line[0] == "":
                continue

            alias_sha1, wiimmId, _, _, cflags, _ = line
            wiimmId = int(wiimmId)
            if (
                all(map(lambda f: f not in cflags, "ZPd"))
                and wiimmId in tracks_sha1
                and tracks_sha1[wiimmId].data != bytes.fromhex(alias_sha1)
            ):
                alias_value = AliasValue(
                    aliased=ProtoSha1(data=bytes.fromhex(alias_sha1)),
                    real=tracks_sha1[wiimmId]
                )

                aliases.append(alias_value)

    return aliases

def main():
    tracks = list(read_wiimm_csv("extended-tracks.csv"))

    try:
        tracks.extend([t for t in read_wiimm_csv("in/public-ref.list") if t not in tracks])
    except FileNotFoundError:
        print("Warning: Could not find public-ref.list, output will only include out-of-db tracks!")
        print("Warning: You can download it from http://archive.tock.eu/wbz/public-ref.list\n")

    with contextlib.suppress(FileExistsError):
        os.mkdir("out")
    with contextlib.suppress(FileExistsError):
        os.mkdir("out/tracks")

    for track in tracks:
        track_sha1 = track.sha1.data.hex()
        with open(f"out/tracks/{track_sha1}.pb.bin", "wb") as track_file:
            track_file.write(track.inner.SerializeToString())

    print(f"Finished writing {len(tracks)} tracks to tracks directory")

    alias_db = AliasDB(aliases=read_aliases_csv(tracks))
    with open("out/alias.pb.bin", "wb") as alias_file:
        alias_file.write(alias_db.SerializeToString())

if __name__ == "__main__":
    main()
