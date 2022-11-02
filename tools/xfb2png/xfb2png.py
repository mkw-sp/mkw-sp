#!/usr/bin/env python3


from argparse import ArgumentParser
from PIL import Image
import os


def clamp(x, minimum, maximum):
    return min(max(x, minimum), maximum)

def convert(in_path, out_path, unstretch):
    if os.path.isdir(in_path):
        os.mkdir(out_path)
        for child_path in sorted(os.listdir(in_path)):
            convert(os.path.join(in_path, child_path), os.path.join(out_path, child_path), unstretch)
    else:
        with open(in_path, 'rb') as in_file:
            in_data = in_file.read()

        out_data = [(0, 0, 0)] * 456 * 608
        for y in range(456):
            for x in range(0, 608, 2):
                i = y * 608 + x

                y1 = in_data[i * 2 + 0] - 16
                y2 = in_data[i * 2 + 2] - 16
                u = in_data[i * 2 + 1] - 128
                v = in_data[i * 2 + 3] - 128

                out_data[i] = (
                    clamp(int(1.164 * y1 + 1.596 * v), 0, 255),
                    clamp(int(1.164 * y1 - 0.392 * u - 0.813 * v), 0, 255),
                    clamp(int(1.164 * y1 + 2.017 * u), 0, 255),
                )

                out_data[i + 1] = (
                    clamp(int(1.164 * y2 + 1.596 * v), 0, 255),
                    clamp(int(1.164 * y2 - 0.392 * u - 0.813 * v), 0, 255),
                    clamp(int(1.164 * y2 + 2.017 * u), 0, 255),
                )

        out_image = Image.new('RGB', (608, 456))
        out_image.putdata(out_data)
        if unstretch:
            out_image = out_image.resize((832, 456))
        out_image.save(f'{out_path}.png')

parser = ArgumentParser()
parser.add_argument('in_path')
parser.add_argument('out_path')
parser.add_argument('--unstretch', action = 'store_true')
args = parser.parse_args()

convert(args.in_path, args.out_path, args.unstretch)
