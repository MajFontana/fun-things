from PIL import Image, ImageDraw
import math

TARGET = "testimage.png"
RESULT = "waveimage.png"
MAXSIDE = 200
SCALE = 50
LINETHICK = 4
MAXFREQ = 10
MINFREQ = 0.5
BGCOL = (0, 0, 0)
PENCOL = (255, 255, 255)
INVERTCOL = False
YSCALE = 0.9
SMOOTH = 1

img = Image.open(TARGET).convert("L")
rat = img.size[0] / img.size[1]
if rat > 1:
    size = (MAXSIDE, int(MAXSIDE / rat))
else:
    size = (int(MAXSIDE * rat), MAXSIDE)
img = img.resize(size)
canv = Image.new("RGB", (size[0] * SCALE, size[1] * SCALE), BGCOL)
draw = ImageDraw.Draw(canv)
rot = 0
circle = 2 * math.pi / SCALE
step = (MAXFREQ - MINFREQ) * circle
minstep = MINFREQ * circle
ct = SCALE // 2
ampsc = SCALE * YSCALE / 2

test = Image.new("L", (size[0] * SCALE, size[1] * SCALE))
testd = ImageDraw.Draw(test)
for y in range(size[1]):
    lpos = None
    for ix in range(canv.size[0]):
        x = ix // SCALE
        rp = ix % SCALE
        pt = math.sin(rot)
        pos = (ix, int(y * SCALE + pt * ampsc + ct))
        if not lpos == None:
            draw.line((lpos, pos), PENCOL, LINETHICK)
        lpos = pos
        col = img.getpixel((x, y))
        if x > 0 and x < size[0] - 1:
            inof = rp - ct
            if inof:
                aof = abs(inof) * SMOOTH
                #out = False
                #if img.getpixel((x + math.copysign(1, inof), y)) != col:
                    #out = True
                    #print(col)
                    #print(img.getpixel((x + math.copysign(1, inof), y)))
                    #print(math.copysign(1, inof))
                    #print(aof, ct)
                col = (img.getpixel((x + math.copysign(1, inof), y)) * aof + col * ct) / (ct + aof)
                #if out:
                    #print(col)
                    #print()
        rot += (INVERTCOL - col / 255) * step + minstep
        testd.line(((ix, y * SCALE), (ix, (y + 1) * SCALE - 1)), int(col), 1)

img.save("origorig.png")
canv.save(RESULT)
test.save("testestest.png")
