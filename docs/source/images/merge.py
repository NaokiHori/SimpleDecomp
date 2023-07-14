import sys
from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont


def default_2d(w, h):
    no = Image.open("nopencil_2d.png")
    x1 = Image.open("x1pencil_2d.png")
    merged = Image.new("RGBA", (2 * w, 1 * h))
    merged.paste(no, (0 * w, 0 * h))
    merged.paste(x1, (1 * w, 0 * h))
    merged.save("default_2d.png")

def transpose_2d(w, h):
    x = Image.open("x1pencil_2d.png")
    y = Image.open("y1pencil_2d.png")
    image = Image.new("RGBA", (2 * w, 1 * h))
    image.paste(x, (0 * w, 0 * h))
    image.paste(y, (1 * w, 0 * h))
    draw = ImageDraw.Draw(image)
    try:
        font = ImageFont.truetype("Arial", 50)
    except OSError:
        font = ImageFont.truetype("DejaVuSans", 50)
    offset = 0.05 * min(w, h)
    draw.text((0 * w + offset, 0 * h + offset), "(0): x1", font=font, fill=(255, 255, 255))
    draw.text((1 * w + offset, 0 * h + offset), "(1): y1", font=font, fill=(255, 255, 255))
    image.save("transpose_2d.png")

def default_3d(w, h):
    no = Image.open("nopencil_3d.png")
    x1 = Image.open("x1pencil_3d.png")
    merged = Image.new("RGBA", (2 * w, 1 * h))
    merged.paste(no, (0 * w, 0 * h))
    merged.paste(x1, (1 * w, 0 * h))
    merged.save("default_3d.png")

def transpose_3d(w, h):
    x1 = Image.open("x1pencil_3d.png")
    y1 = Image.open("y1pencil_3d.png")
    z1 = Image.open("z1pencil_3d.png")
    x2 = Image.open("x2pencil_3d.png")
    y2 = Image.open("y2pencil_3d.png")
    z2 = Image.open("z2pencil_3d.png")
    image = Image.new("RGBA", (3 * w, 2 * h))
    image.paste(x1, (0 * w, 0 * h))
    image.paste(y1, (1 * w, 0 * h))
    image.paste(z1, (2 * w, 0 * h))
    image.paste(x2, (2 * w, 1 * h))
    image.paste(y2, (1 * w, 1 * h))
    image.paste(z2, (0 * w, 1 * h))
    draw = ImageDraw.Draw(image)
    try:
        font = ImageFont.truetype("Arial", 50)
    except OSError:
        font = ImageFont.truetype("DejaVuSans", 50)
    offset = 0.05 * min(w, h)
    draw.text((0 * w + offset, 0 * h + offset), "(0): x1", font=font, fill=(255, 255, 255))
    draw.text((1 * w + offset, 0 * h + offset), "(1): y1", font=font, fill=(255, 255, 255))
    draw.text((2 * w + offset, 0 * h + offset), "(2): z1", font=font, fill=(255, 255, 255))
    draw.text((0 * w + offset, 1 * h + offset), "(5): z2", font=font, fill=(255, 255, 255))
    draw.text((1 * w + offset, 1 * h + offset), "(4): y2", font=font, fill=(255, 255, 255))
    draw.text((2 * w + offset, 1 * h + offset), "(3): x2", font=font, fill=(255, 255, 255))
    image.save("transpose_3d.png")

if __name__ == "__main__":
    argv = sys.argv
    assert(2 == len(argv))
    ndims = int(argv[1])
    assert(2 == ndims or 3 == ndims)
    w = 800
    h = 800
    if 2 == ndims:
        default_2d(w, h)
        transpose_2d(w, h)
    else:
        default_3d(w, h)
        transpose_3d(w, h)

