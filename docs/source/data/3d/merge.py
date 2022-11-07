from PIL import Image
from PIL import ImageDraw
from PIL import ImageFont

def default():
    outfname = "default.png"

    w = 800
    h = 800

    no = Image.open("nopencil.png")
    x1 = Image.open("x1pencil.png")

    merged = Image.new("RGBA", (2*w, 1*h))
    merged.paste(no, (0*w, 0*h))
    merged.paste(x1, (1*w, 0*h))

    merged.save(outfname)

def transpose():
    outfname = "transpose.png"

    w = 800
    h = 800

    x1 = Image.open("x1pencil.png")
    y1 = Image.open("y1pencil.png")
    z1 = Image.open("z1pencil.png")
    x2 = Image.open("x2pencil.png")
    y2 = Image.open("y2pencil.png")
    z2 = Image.open("z2pencil.png")

    merged = Image.new("RGBA", (3*w, 2*h))
    merged.paste(x1, (0*w, 0*h))
    merged.paste(y1, (1*w, 0*h))
    merged.paste(z1, (2*w, 0*h))
    merged.paste(x2, (2*w, 1*h))
    merged.paste(y2, (1*w, 1*h))
    merged.paste(z2, (0*w, 1*h))

    merged.save(outfname)

    image = Image.open(outfname)
    draw = ImageDraw.Draw(image)

    font = ImageFont.truetype('DejaVuSans', 50)
    offset = 0.05*min(w, h)

    draw.text((0*w+offset, 0*h+offset), "(0): x1", font=font, fill=(255, 255, 255))
    draw.text((1*w+offset, 0*h+offset), "(1): y1", font=font, fill=(255, 255, 255))
    draw.text((2*w+offset, 0*h+offset), "(2): z1", font=font, fill=(255, 255, 255))
    draw.text((0*w+offset, 1*h+offset), "(5): z2", font=font, fill=(255, 255, 255))
    draw.text((1*w+offset, 1*h+offset), "(4): y2", font=font, fill=(255, 255, 255))
    draw.text((2*w+offset, 1*h+offset), "(3): x2", font=font, fill=(255, 255, 255))

    image.save(outfname)

if __name__ == "__main__":
    default()
    transpose()

