import pygame

SIZE = (200, 200)
POS = [-0.5, 0]
XSCALE = 3
MAXITER = 10
COLPTS = ((0, (0, 7, 100)), (0.16, (32, 107, 203)), (0.42, (237, 255, 255)), (0.6425, (255, 170, 0)), (0.8575, (0, 2, 0)))
INCOL = (0, 0, 0)
DMOV = 0.1
DSCALE = 1.1
DITER = 5

def mandel():
    data = b""
    for y in range(SIZE[1]):
        for x in range(SIZE[0]):
            creal = (x - ctrx) * fragx * XSCALE + POS[0]
            cimag = (y - ctry) * fragy * yscale + POS[1]
            real = creal
            imag = cimag
            for n in range(MAXITER):
                real2 = real * real
                imag2 = imag * imag
                if real2 + imag2 > 4:
                    data += grad[n]
                    break
                imag = 2 * real * imag + cimag
                real = real2 - imag2 + creal
            else:
                data += incol
    return data

fragx = 1 / (SIZE[0])
fragy = 1 / (SIZE[1])
ctrx = SIZE[0] / 2
ctry = SIZE[1] / 2
incol = b"".join([n.to_bytes(1, "little") for n in INCOL[::-1]]) + b"\x00"
lpts = len(COLPTS)

def compute_mov():
    global yscale
    global step
    yscale = XSCALE * (SIZE[1] / SIZE[0])
    step = DMOV * XSCALE

def compute_iter():
    global colstep
    global div
    global grad
    colstep = 255 / MAXITER
    grad = []
    div = 1 / MAXITER
    n = -1
    m = 0
    for p in range(MAXITER):
        pos = p * div
        if (pos >= COLPTS[m][0] and m > 0) or p == 0:
            n += 1
            m += 1
            if m == lpts:
                m = 0
            if n == lpts:
                n = 0
            rg = COLPTS[m][0] - COLPTS[n][0]
            if rg < 0:
                rg += 1
            colrg = [COLPTS[m][1][i] - COLPTS[n][1][i] for i in range(3)]
        rw = b"".join([int(COLPTS[n][1][i] + colrg[i] * ((pos - COLPTS[n][0]) / rg)).to_bytes(1, "little") for i in range(3)][::-1]) + b"\x00"
        grad.append(rw)

compute_mov()
compute_iter()

pygame.init()
screen = pygame.display.set_mode(SIZE)
p = screen.get_buffer()

run = True
while run:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            run = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_LEFT:
                POS[0] -= step
            elif event.key == pygame.K_RIGHT:
                POS[0] += step
            if event.key == pygame.K_UP:
                POS[1] -= step
            elif event.key == pygame.K_DOWN:
                POS[1] += step
            elif event.key == pygame.K_KP_PLUS:
                XSCALE /= DSCALE
                compute_mov()
            elif event.key == pygame.K_KP_MINUS:
                XSCALE *= DSCALE
                compute_mov()
            elif event.key == pygame.K_PAGEUP:
                MAXITER += DITER
                compute_iter()
            elif event.key == pygame.K_PAGEDOWN:
                MAXITER -= DITER
                if MAXITER < 1:
                    MAXITER = 1
                compute_iter()
    p.write(mandel())
    pygame.display.flip()

pygame.quit()
