#include "2600.h"

void draw(int x, int y, uint32 col)
{
	int oy, ox, i;

	x *= SF;
	y *= SF;
	for(oy = 0; oy < SF; oy++)
		for(ox = 0; ox < SF; ox++)
			for(i = 0; i < 4; i++)
				rast[(y+oy)*W*4 + (x+ox)*4 + i] = col>>i*8 & 0xff;
}

void tiawrite(uint16 a, uint8 v)
{
	switch(a) {
	case WSYNC:
		tia(228-px);
		break;
	}
	tiareg[a] = v;
}

void tia(uint8 n)
{
	int i;

	for(i = 0; i < n; i++) {
		if(py >= 40 && py < 232 && px >= 68 && px < 228)
			draw(px-68, py-40, 0xffffff);
		px++;
		if(px >= 228) {
			px = 0;
			py++;
			if(py >= 262)
				py = 0;
		}
	}
	if(SDL_LockSurface(scr) < 0)
		errorf(0, "SDL_LockSurface: %s", SDL_GetError());
	memcpy((uint8*)scr->pixels, rast, W*H*4);
	SDL_UnlockSurface(scr);
	SDL_UpdateRect(scr, 0, 0, 0, 0);
}
