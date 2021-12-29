#include "2600.h"

uint32 coltab[8][16] = {
	{0x000000, 0x444400, 0x702800, 0x841800, 0x880000, 0x78005c, 0x480078, 0xdddddd, 0x000088, 0x00187c, 0x002c5c, 0x003c2c, 0x003c00, 0x143800, 0x2c3000, 0x442800},
	{0x404040, 0x646410, 0x844414, 0x983418, 0x9c2020, 0x8c2074, 0x602090, 0x302098, 0x1c209c, 0x1c3890, 0x1c4c78, 0x1c5c48, 0x205c20, 0x345c1c, 0x4c501c, 0x644818},
	{0x6c6c6c, 0x848424, 0x985c28, 0xac5030, 0xb03c3c, 0xa03c88, 0x783ca4, 0x4c3cac, 0x3840b0, 0x3854a8, 0x386890, 0x387c64, 0x407c40, 0x507c38, 0x687034, 0x846830},
	{0x909090, 0xa0a034, 0xac783c, 0xc06848, 0xc05858, 0xb0589c, 0x8c58b8, 0x6858c0, 0x505cc0, 0x5070bc, 0x5084ac, 0x509c80, 0x5c9c5c, 0x6c9850, 0x848c4c, 0xa08444},
	{},
	{},
	{},
	{},
};

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
		return;
	case COLUP0:
	case COLUP1:
	case COLUPF:
	case COLUBK:
		colubk = coltab[v&0x7][(v&0xf8)>>3];
		return;
	}
	tiareg[a] = v;
}

void tia(uint8 n)
{
	int i;

	for(i = 0; i < n; i++) {
		if(py >= 40 && py < 232 && px >= 68 && px < 228)
			draw(px-68, py-40, colubk);
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
