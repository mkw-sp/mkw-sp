# BMG Cleaner

This tool will run through all `.bmg.json5` files and strip `"font": "regular"` fields from them,
as our tooling will default the `font` value to be `regular`, removing ~20k lines from the project.
