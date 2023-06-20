import json5
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
        # Handle already cleaned `message_id: message`
        if isinstance(message, str) or message is None:
            new_bmg[message_id] = message
            continue

        # Check for `font` key that is not "regular"
        if message.get("font", "regular") == "regular":
            # If font: regular or no font specified, store message inline.
            new_bmg[message_id] = message["string"]
        else:
            new_bmg[message_id] = message

    path.write_text(json5.dumps(new_bmg, indent=4, ensure_ascii=False).replace("\n", "\r\n"))

with multiprocessing.Pool() as pool:
    pool.map(cleanup_bmg, bmg_paths)
