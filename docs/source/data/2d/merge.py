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

    x = Image.open("x1pencil.png")
    y = Image.open("y1pencil.png")

    merged = Image.new("RGBA", (2*w, 1*h))
    merged.paste(x, (0*w, 0*h))
    merged.paste(y, (1*w, 0*h))

    merged.save(outfname)

    image = Image.open(outfname)
    draw = ImageDraw.Draw(image)

    font = ImageFont.truetype('DejaVuSans', 50)
    offset = 0.05*min(w, h)

    draw.text((0*w+offset, 0*h+offset), "(0): x1", font=font, fill=(255, 255, 255))
    draw.text((1*w+offset, 0*h+offset), "(1): y1", font=font, fill=(255, 255, 255))

    image.save(outfname)

if __name__ == "__main__":
    default()
    transpose()

