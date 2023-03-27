import csv
import sys
import configparser

language_lookup = ["nl", "fr_ntsc", "fr_pal", "de", "it", "jp", "kr", "pt_ntsc", "pt_pal", "ru", "es_ntsc", "es_pal", "gr", "pl", "fi", "sw", "cz", "dk"]

db_csv = sys.argv[1]
language_csv = sys.argv[2]


class Track:
    cannonical_name: str
    authors: str
    prefix: str
    sha1: str
    clan: int
    id: str

    translations: list[str]

    @staticmethod
    def from_csv(line: list[str]) -> "Track":
        self = Track()
        sha1, id, _, clan, _, _, _, _, _, prefix, name, _, _, authors, _, _ = line

        self.cannonical_name = name
        self.translations = []
        self.authors = authors
        self.clan = int(clan)
        self.prefix = prefix
        self.sha1 = sha1
        self.id = id

        return self

    def to_ini(self, config: configparser.ConfigParser):
        config.add_section(self.id)
        config.set(self.id, "prefix", self.prefix)
        config.set(self.id, "trackname", self.cannonical_name)
        config.set(self.id, "author", self.authors)
        config.set(self.id, "sha1", self.sha1)

        for i, trans in enumerate(self.translations):
            config.set(self.id, language_lookup[i], trans)


tracks: list[Track] = []
with open(db_csv) as db_csv:
    db_csv.readline()

    reader = csv.reader(db_csv, delimiter="|")
    tracks.extend(Track.from_csv(line) for line in reader)

with open(language_csv) as language_csv:
    reader = csv.reader(language_csv, delimiter="|")
    for line in reader:
        if line == [] or line[0] == "":
            continue

        clan_id = int(line[0])
        for track in tracks:
            if track.clan == clan_id:
                track.translations = line[2:]


parser = configparser.ConfigParser(interpolation=None)
for track in tracks:
    track.to_ini(parser)

with open("tracks.ini", "w") as tracks_ini:
    parser.write(tracks_ini)
