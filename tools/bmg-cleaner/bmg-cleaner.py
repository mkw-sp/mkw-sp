import json, json5
import pathlib
import multiprocessing

base_path = pathlib.Path("../../assets/message")
bmg_paths = base_path.glob("*.bmg.json5")

def cleanup_bmg(path: pathlib.Path):
    print("Cleaning up", path.name)

    new_bmg = {}
    bmg = json5.loads(path.read_text())
    assert isinstance(bmg, dict)    

    for message_id, message in bmg.items():
        font = message.get("font")

        new_bmg[message_id] = {"string": message["string"]}
        if font is not None and font != "regular": 
            new_bmg[message_id]["font"] = font

    path.write_text(json5.dumps(new_bmg, indent=4, ensure_ascii=False))

with multiprocessing.Pool() as pool:
    pool.map(cleanup_bmg, bmg_paths)
