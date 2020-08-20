from PIL import Image

TARGET = "sampleimage.jpg"
PALETTE = ((255, 255, 255), (0, 0, 0))
KERNEL = (((1, 0), 7 / 16), ((-1, 1), 3 / 16), ((0, 1), 5 / 16), ((1, 1), 1 / 16))
RESULT = "dithered.png"

img = Image.open(TARGET).convert("RGB")
data = list(img.getdata())
data = [data[y * img.width:(y + 1) * img.width] for y in range(img.height)]
new = Image.new("RGB", img.size)
for y in range(img.height - 1):
    for x in range(img.width - 1):
        pixel = data[y][x]
        deltas = [sum([(pixel[i] - color[i]) ** 2 for i in range(3)]) for color in PALETTE]
        match = PALETTE[deltas.index(min(deltas))]
        error = [pixel[i] - match[i] for i in range(3)]
        new.putpixel((x, y), match)
        for rel, ratio in KERNEL:
            current = data[y + rel[1]][x + rel[0]]
            data[y + rel[1]][x + rel[0]] = [current[i] + error[i] * ratio for i in range(3)]
new.save(RESULT)
