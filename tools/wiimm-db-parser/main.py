import csv
import configparser
import contextlib

language_lookup = ["nl", "fr_ntsc", "fr_pal", "de", "it", "jp", "kr", "pt_ntsc", "pt_pal", "ru", "es_ntsc", "es_pal", "gr", "pl", "fi", "sw", "cz", "dk"]

class Track:
    cannonical_name: str
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
        sha1, id, _, clan, ctype, slot, _, _, _, prefix, name, _, _, authors, _, _ = line

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
        config.set(self.sha1, "trackname", trackname)
        config.set(self.sha1, "author", self.authors)
        config.set(self.sha1, "type", self.ctype)
        config.set(self.sha1, "slot", self.slot)
        config.set(self.sha1, "id", self.id)

        for i, trans in enumerate(self.translations):
            config.set(self.sha1, language_lookup[i], trans)


def read_wiimm_csv(path: str) -> list[Track]:
    tracks: list[Track] = []
    with open(path) as db_csv:
        db_csv.readline()

        reader = csv.reader(db_csv, delimiter="|")
        tracks.extend(Track.from_csv(line) for line in reader)

    return tracks

def read_languages_csv(tracks: list[Track]) -> list[Track]:
    with open("language.csv") as language_csv:
        reader = csv.reader(language_csv, delimiter="|")
        for line in reader:
            if line == [] or line[0] == "":
                continue

            clan_id = int(line[0])
            for track in tracks:
                if track.clan == clan_id:
                    track.translations = line[2:]

    return tracks


def main():
    tracks = read_wiimm_csv("extended-tracks.csv")

    try:
        tracks.extend(read_wiimm_csv("public-ref.list"))
    except FileNotFoundError:
        print("Warning: Could not find public-ref.list, output will only include vanilla and out-of-db tracks!")
        print("Warning: You can download it from http://archive.tock.eu/wbz/public-ref.list\n")

    # Ignore missing languages.csv
    with contextlib.suppress(FileNotFoundError):
        tracks = read_languages_csv(tracks)


    parser = configparser.ConfigParser(interpolation=None)
    for track in tracks:
        track.to_ini(parser)

    with open("tracks.ini", "w") as tracks_ini:
        print(f"Finished writing {len(tracks)} tracks to tracks.ini!")
        parser.write(tracks_ini)

if __name__ == "__main__":
    main()
