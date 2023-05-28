import csv
import configparser
import contextlib
import sys
from typing import Generator

language_lookup = ["nl", "fr_ntsc", "fr_pal", "de", "it", "jp", "kr", "pt_ntsc", "pt_pal", "ru", "es_ntsc", "es_pal", "gr", "pl", "fi", "sw", "cz", "dk"]
extra_includes = [include.removeprefix("--include-") for include in sys.argv if include.startswith("--include-")]

class Track:
    cannonical_name: str
    music_slot: str
    authors: str
    prefix: str
    ctype: str
    slot: str
    sha1: str
    clan: int
    id: str

    translations: list[str]

    @staticmethod
    def from_csv(line: list[str]) -> "Track":
        self = Track()
        sha1, id, _, clan, ctype, slot, music_slot, _, _, prefix, name, _, _, authors, _, _ = line

        self.music_slot = music_slot
        self.cannonical_name = name
        self.translations = []
        self.authors = authors
        self.clan = int(clan)
        self.prefix = prefix
        self.ctype = ctype
        self.sha1 = sha1
        self.slot = slot
        self.id = id

        return self

    def to_ini(self, config: configparser.ConfigParser):
        if self.prefix != "":
            trackname = f"{self.prefix} {self.cannonical_name}"
        else:
            trackname = self.cannonical_name

        config.add_section(self.sha1)
        config.set(self.sha1, "mslot", self.music_slot)
        config.set(self.sha1, "trackname", trackname)
        config.set(self.sha1, "type", self.ctype)
        config.set(self.sha1, "slot", self.slot)

        if "id" in extra_includes:
            config.set(self.sha1, "id", self.id)
        if "authors" in extra_includes:
            config.set(self.sha1, "author", self.authors)

        for i, trans in enumerate(self.translations):
            config.set(self.sha1, language_lookup[i], trans)


def read_wiimm_csv(path: str) -> Generator[Track, None, None]:
    with open(path) as db_csv:
        db_csv.readline()

        reader = csv.reader(db_csv, delimiter="|")
        yield from (Track.from_csv(line) for line in reader)

def read_languages_csv(tracks: list[Track]) -> list[Track]:
    by_clan = {track.clan: track for track in tracks}
    with open("language.csv") as language_csv:
        reader = csv.reader(language_csv, delimiter="|")
        for line in reader:
            if line == [] or line[0] == "":
                continue

            clan_id = int(line[0])
            if (track := by_clan.get(clan_id)) is not None:
                track.translations = line[2:]

    return tracks

def read_aliases_csv(tracks: list[Track]) -> dict[str, str]:
    aliases = {}
    tracks_sha1 = {t.id: t.sha1 for t in tracks}

    with open("sha1-reference.txt") as alias_csv:
        for _ in range(9):
            alias_csv.readline()

        reader = csv.reader(alias_csv, delimiter="|")
        for line in reader:
            if line == [] or line[0] == "":
                continue

            alias_sha1, wiimmId, _, _, cflags, _ = line
            if all(map(lambda f: f not in cflags, "ZPd")):
                aliases[alias_sha1] = tracks_sha1[wiimmId]

    return aliases

def main():
    tracks = list(read_wiimm_csv("extended-tracks.csv"))

    try:
        tracks.extend(read_wiimm_csv("public-ref.list"))
    except FileNotFoundError:
        print("Warning: Could not find public-ref.list, output will only include out-of-db tracks!")
        print("Warning: You can download it from http://archive.tock.eu/wbz/public-ref.list\n")

    # Ignore missing languages.csv
    with contextlib.suppress(FileNotFoundError):
        tracks = read_languages_csv(tracks)

    aliases = read_aliases_csv(tracks)

    parser = configparser.ConfigParser(interpolation=None)
    for i, track in enumerate(tracks):
        if track.slot == "0":
            del tracks[i]
        else:
            track.to_ini(parser)

    parser.add_section("aliases")
    for alias_sha1, sha1 in aliases.items():
        parser.set("aliases", alias_sha1, sha1)

    with open("tracks.ini", "w") as tracks_ini:
        print(f"Finished writing {len(tracks)} tracks to tracks.ini!")
        parser.write(tracks_ini, space_around_delimiters=False)

if __name__ == "__main__":
    main()
