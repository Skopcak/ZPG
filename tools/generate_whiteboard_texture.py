"""Render a lecture slide for the board and create the shared login text.

Requires Pillow and Poppler (pdftoppm). These are asset tools, not app dependencies.
python tools/generate_whiteboard_texture.py path/to/ZPG_Prednaska_7.pdf --page 36
"""

import argparse
from pathlib import Path
import subprocess
from tempfile import TemporaryDirectory
from PIL import Image, ImageDraw, ImageFont, ImageOps

ROOT = Path(__file__).resolve().parents[1]


def generate(pdf, page, pdftoppm):
    with TemporaryDirectory(prefix='zpg-board-') as temporary:
        prefix = Path(temporary)/'slide'
        subprocess.run([pdftoppm, '-f', str(page), '-l', str(page), '-scale-to', '2800',
                        '-singlefile', '-png', str(pdf), str(prefix)], check=True)
        # Keep the complete slide and its aspect ratio; add white side margins.
        slide = Image.open(prefix.with_suffix('.png')).convert('RGB')
        slide = ImageOps.contain(slide, (2400, 1200), Image.Resampling.LANCZOS)
        board = Image.new('RGB', (2400, 1200), 'white')
        board.paste(slide, ((2400-slide.width)//2, (1200-slide.height)//2))
        destination = ROOT/'project/Models/whiteboard'/f'Lecture7_slide{page}.png'
        board.save(destination)
        print(destination)

    login = Image.new('RGBA', (512, 128), (0, 0, 0, 0))
    draw = ImageDraw.Draw(login)
    font = ImageFont.truetype('C:/Windows/Fonts/segoeuib.ttf', 82)
    draw.text((258, 59), 'jar0193', font=font, fill=(0, 0, 0, 220), anchor='mm', stroke_width=2)
    draw.text((256, 57), 'jar0193', font=font, fill='white', anchor='mm')
    login_path = ROOT/'project/Textures/login.png'
    login.save(login_path)
    print(login_path)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('pdf', type=Path)
    parser.add_argument('--page', type=int, default=36)
    parser.add_argument('--pdftoppm', default='pdftoppm')
    arguments = parser.parse_args()
    generate(arguments.pdf.resolve(), arguments.page, arguments.pdftoppm)
