#include <SDL.h>
#undef main
#include <stdio.h>
#include <vector>

using namespace std;

extern void kernelmain();

const int WIDTH = 800;
const int HEIGHT = 800;
const long double SPOSX = -0.5;
const long double SPOSY = 0;
const long double SSCALEX = 3;
const int SMAXITER = 10;
const long double DPOS = 0.02;
const long double KSCALE = 1.1;
const int DITER = 5;
const Uint32 INCOL = 0x00000000;
Uint32 COLS [6] = {0x00000764, 0x00206bcb, 0x00edffff, 0x00ffaa00, 0x00000200, 0x00000764};
long double COLPTS [6] = {0, 0.16, 0.42, 0.6425, 0.8575, 1};

vector <Uint32> computeColors(int maxiter, long double* pts, Uint32* cols) {
	long double colstep = 1.0 / maxiter;
	int idx = -1;
	bool start = true;
	long double d;
	int cold [3];
	int col1 [3];
	vector <Uint32> comp;
	for (int i = 0; i < maxiter; i++) {
		long double pos = i * colstep;
		while (start or pos > pts[idx + 1]) {
			idx++;
			start = false;
			d = pts[idx + 1] - pts[idx];
			col1[0] = cols[idx] >> 16; col1[1] = (cols[idx] >> 8) % 256; col1[2] = cols[idx] % 256;
			int col2[3] = { cols[idx + 1] >> 16, (cols[idx + 1] >> 8) % 256, cols[idx + 1] % 256 };
			cold[0] = col2[0] - col1[0]; cold[1] = col2[1] - col1[1]; cold[2] = col2[2] - col1[2];
		}
		long double colrt = (pos - pts[idx]) / d;
		Uint32 col = ((Uint8)(col1[0] + cold[0] * colrt) << 16) + ((Uint8)(col1[1] + cold[1] * colrt) << 8) + (Uint8)(col1[2] + cold[2] * colrt);
		comp.push_back(col);
	}
	return comp;
}

bool init(SDL_Window* &window, SDL_Surface* &surf) {
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO) >= 0) {
		SDL_DisplayMode mode;
		SDL_GetCurrentDisplayMode(0, &mode);
		window = SDL_CreateWindow("Mandelbrot", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, mode.w, mode.h, SDL_WINDOW_SHOWN);
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		if (window != NULL) {
			surf = SDL_GetWindowSurface(window);
		}
		else {
			success = false;
		}
	}
	else {
		success = false;
	}
	return success;
}

void close(SDL_Window* &window, SDL_Surface* &surf) {
	SDL_FreeSurface(surf);
	surf = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
}

int mandelbrot(int maxiter, long double x, long double y) {
	long double real = x;
	long double imag = y;
	for (int i = 0; i < maxiter; i++) {
		long double real2 = real * real;
		long double imag2 = imag * imag;
		if (real2 + imag2 > 4) {
			return i;
		}
		imag = 2 * real * imag + y;
		real = real2 - imag2 + x;
	}
	return -1;
}

void render(SDL_Surface* surf, int maxiter, long double posx, long double posy, long double scalex, vector <Uint32> colramp, Uint32 incol) {
	long double scaley = scalex * surf->h / surf->w;
	long double ascaley = scaley / surf->h;
	long double ascalex = scalex / surf->w;
	long double ctrx = surf->w / 2.0;
	long double ctry = surf->h / 2.0;
	Uint8* target = (Uint8*)surf->pixels;
	SDL_LockSurface(surf);
	for (int y = 0; y < surf->h; y++) {
		long double ry = (y - ctry) * ascaley + posy;
		for (int x = 0; x < surf->w; x++) {
			long double rx = (x - ctrx) * ascalex + posx;
			int iter = mandelbrot(maxiter, rx, ry);
			Uint32 col;
			if (iter == -1) {
				col = incol;
			}
			else {
				col = colramp[iter];
			}
			*(Uint32*)target = col;
			target += surf->format->BytesPerPixel;
		}
	}
	SDL_UnlockSurface(surf);
}

int main() {
	kernelmain();
	SDL_Window* window = NULL;
	SDL_Surface* screen = NULL;
	if (init(window, screen)) {
		int maxiter = SMAXITER;
		long double posx = SPOSX;
		long double posy = SPOSY;
		long double scalex = SSCALEX;
		bool recomp = true;
		vector <Uint32> colramp;
		SDL_Event ev;
		bool quit = false;
		while (!quit) {
			while (SDL_PollEvent(&ev) != 0) {
				if (ev.type == SDL_QUIT) {
					quit = true;
				}
				else if (ev.type == SDL_KEYDOWN) {
					long double sdpos = DPOS * scalex;
					switch (ev.key.keysym.sym) {
					case SDLK_ESCAPE:
						quit = true;
						break;
					case SDLK_UP:
						posy -= sdpos;
						break;
					case SDLK_DOWN:
						posy += sdpos;
						break;
					case SDLK_LEFT:
						posx -= sdpos;
						break;
					case SDLK_RIGHT:
						posx += sdpos;
						break;
					case SDLK_KP_PLUS:
						scalex /= KSCALE;
						break;
					case SDLK_KP_MINUS:
						scalex *= KSCALE;
						break;
					case SDLK_PAGEUP:
						maxiter += DITER;
						recomp = true;
						break;
					case SDLK_PAGEDOWN:
						int niter = maxiter - DITER;
						if (niter > 0) {
							maxiter = niter;
							recomp = true;
						}
						break;
					}
				}
			}
			if (recomp) {
				colramp = computeColors(maxiter, COLPTS, COLS);
				recomp = false;
			}
			render(screen, maxiter, posx, posy, scalex, colramp, INCOL);
			SDL_UpdateWindowSurface(window);
		}
	}
	else {
		printf("Failed to initialize!\n");
	}
	close(window, screen);
	return 0;
}