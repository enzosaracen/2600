#include "2600.h"

uint32 coltab[8][16] = {
	{0x000000, 0x444400, 0x702800, 0x841800, 0x880000, 0x78005c, 0x480078, 0xdddddd, 0x000088, 0x00187c, 0x002c5c, 0x003c2c, 0x003c00, 0x143800, 0x2c3000, 0x442800},
	{0x404040, 0x646410, 0x844414, 0x983418, 0x9c2020, 0x8c2074, 0x602090, 0x302098, 0x1c209c, 0x1c3890, 0x1c4c78, 0x1c5c48, 0x205c20, 0x345c1c, 0x4c501c, 0x644818},
	{0x6c6c6c, 0x848424, 0x985c28, 0xac5030, 0xb03c3c, 0xa03c88, 0x783ca4, 0x4c3cac, 0x3840b0, 0x3854a8, 0x386890, 0x387c64, 0x407c40, 0x507c38, 0x687034, 0x846830},
	{0x909090, 0xa0a034, 0xac783c, 0xc06848, 0xc05858, 0xb0589c, 0x8c58b8, 0x6858c0, 0x505cc0, 0x5070bc, 0x5084ac, 0x509c80, 0x5c9c5c, 0x6c9850, 0x848c4c, 0xa08444},
	{0xb0b0b0, 0xb8b840, 0xbc8c4c, 0xd0805c, 0xd07070, 0xc070b0, 0xa070cc, 0x7c70d0, 0x6874d0, 0x6888cc, 0x689cc0, 0x68b494, 0x74b474, 0x84b468, 0x9ca864, 0xb89c58},
	{0xc8c8c8, 0xd0d050, 0xcca05c, 0xe09470, 0xe08888, 0xd084c0, 0xb484dc, 0x9488e0, 0x7c8ce0, 0x7c9cdc, 0x7cb4d4, 0x7cd0ac, 0x8cd08c, 0x9ccc7c, 0xb4c078, 0xd0b46c},
	{0xdcdcdc, 0xe8e85c, 0xdcb468, 0xeca880, 0xeca0a0, 0xdc9cd0, 0xc49cec, 0xa8a0ec, 0x90a4ec, 0x90b4ec, 0x90cce8, 0x90e4c0, 0xa4e4a4, 0xb4e490, 0xccd488, 0xe8cc7c},
	{0xececec, 0xfcfc68, 0xecc878, 0xfcbc94, 0xfcb4b4, 0xecb0e0, 0xd4b0fc, 0xbcb4fc, 0xa4b8fc, 0xa4c8fc, 0xa4e0fc, 0xa4fcd4, 0xb8fcb8, 0xc8fca4, 0xe0ec9c, 0xfce08c},
};

void pixel(int x, int y, uint32 col)
{
	int oy, ox, i;

	x *= SFW;
	y *= SFH;
	for(oy = 0; oy < SFH; oy++)
		for(ox = 0; ox < SFW; ox++)
			for(i = 0; i < 4; i++)
				rast[(y+oy)*W*4 + (x+ox)*4 + i] = col>>i*8 & 0xff;
}

void draw(void)
{
	SDL_Event ev;

	if(SDL_LockSurface(scr) < 0)
		errorf(0, "SDL_LockSurface: %s", SDL_GetError());
	memcpy((uint8*)scr->pixels, rast, W*H*4);
	SDL_UnlockSurface(scr);
	SDL_UpdateRect(scr, 0, 0, 0, 0);
	while(SDL_PollEvent(&ev))
		switch(ev.type) {
		case SDL_QUIT:
			SDL_Quit();
			exit(0);
			break;
		case SDL_KEYDOWN:
			switch(ev.key.keysym.sym) {
			case SDLK_UP:		portA &= ~(1<<4); inpt |= 1<<0; break;
			case SDLK_DOWN:		portA &= ~(1<<5); inpt |= 1<<1; break;
			case SDLK_LEFT:		portA &= ~(1<<6); inpt |= 1<<2; break;
			case SDLK_RIGHT:	portA &= ~(1<<7); inpt |= 1<<3; break;
			case ' ':		inpt |= 1<<4; break;
			case SDLK_RETURN:	inpt |= 1<<5; break;
			case SDLK_TAB:		inpt |= 1<<6; break;
			case SDLK_F2:		portB &= ~(1); break;
			case SDLK_F3:		portB &= ~(1<<3); break;
			}
			break;
		case SDL_KEYUP:
			switch(ev.key.keysym.sym) {
			case SDLK_UP:		portA |= 1<<4; inpt &= ~(1<<0); latch &= ~(1<<0); break;
			case SDLK_DOWN:		portA |= 1<<5; inpt &= ~(1<<1); latch &= ~(1<<1); break;
			case SDLK_LEFT:		portA |= 1<<6; inpt &= ~(1<<2); latch &= ~(1<<2); break;
			case SDLK_RIGHT:	portA |= 1<<7; inpt &= ~(1<<3); latch &= ~(1<<3); break;
			case ' ':		inpt &= ~(1<<4); latch &= ~(1<<4); break;
			case SDLK_RETURN:	inpt &= ~(1<<5); latch &= ~(1<<5); break;
			case SDLK_TAB:		inpt &= ~(1<<6); latch &= ~(1<<6); break;
			case SDLK_F3:		portB |= 1<<3; break;
			}
			break;
		}
}

void playfield(void)
{
	int x;
	uint32 col;

	col = colupf;
	x = px / 4;
	if(x >= 20) {
		if(tiareg[CTRLPF] & 2)
			col = colup1;
		if(tiareg[CTRLPF] & 1)
			x = 39-x;
		else
			x = x-20;
	} else
		if(tiareg[CTRLPF] & 2)
			col = colup0;
	if(x < 4) {
		if(!(tiareg[PF0] & 0x10<<x))
			return;
	} else if(x < 12) {
		if(!(tiareg[PF1] & 0x80>>(x-4)))
			return;
	} else if(x < 20) {
		if(!(tiareg[PF2] & 1<<(x-12)))
			return;
	}
	pixel(px, py, col);
	ppf = 1;
}

void missile(uint8 t)
{
	if(tiareg[RESMP0+t] & 2) {
		tiareg[RESM0+t] = t ? tiareg[PF1] : tiareg[PF0];
		return;
	}
	if(tiareg[ENAM0+t] && px >= tiareg[RESM0+t])
		if((px-tiareg[RESM0+t]) <= ((tiareg[NUSIZ0+t] & 0x30) >> 3)) {
			pixel(px, py, t ? colup1 : colup0);
			t ? (pm1 = 1) : (pm0 = 1);
		}
}

void ball(void)
{
	if(tiareg[ENABL] && px >= tiareg[RESBL])
		if((px-tiareg[RESBL]) <= ((tiareg[CTRLPF] & 0x30) >> 3)) {
			pixel(px, py, colupf);
			pbl = 1;
		}
}

void player(uint8 t)
{
	int x;
	uint8 v;

	x = px - tiareg[RESP0+t];
	if(x < 0)
		return;
	switch(tiareg[NUSIZ0+t] & 7) {
	case 0:	if(x >= 8) return; break;
	case 1:	if(x >= 8 && (x < 16 || x >= 24)) return; break;
	case 2: if(x >= 8 && (x < 32 || x >= 40)) return; break;
	case 3: if(x >= 40 || (x & 0xf) >= 8) return; break;
	case 4: if(x >= 8 && (x < 64 || x >= 72)) return; break;
	case 5: if(x >= 16) return; break;
	case 6: if(x >= 72 || ((x & 0x1f) >= 8)) return; break;
	case 7: if(x >= 32) return; break;
	}
	v = 0x80 >> (x % 8);
	if(tiareg[REFP0+t] & 8)
		v = 1 << (x % 8);
	if(tiareg[GRP0+t] & v) {
		pixel(px, py, t ? colup1 : colup0);
		t ? (pp1 = 1) : (pp0 = 1);
	}
}

void tiawrite(uint16 a, uint8 v)
{
	int i, t;

	switch(a) {
	case VSYNC:
		if(v & 2)
			draw();
		return;
	case VBLANK:
		if(v & 1<<6)
			latch = 0xff;
		/* reliance on vblank finnicky, seems to work well but in some
		 * games (river raid) py just keeps going up (although vblank seems to be
		 * called) and nothing ends up getting drawn,
		 * will have to investigate */
		if(!(v & 2))
			py = 0;
		return;
	case WSYNC:
		if(px >= 160) {
			t = (228-px)+160;
		} else
			t = 160-px;
		tcarry += t % 3;
		timerstep(t / 3);
		tia(t);
		return;
	case COLUP0:
		colup0 = coltab[(v & 0xe)>>1][(v & 0xf0)>>4];
		return;
	case COLUP1:
		colup1 = coltab[(v & 0xe)>>1][(v & 0xf0)>>4];
		return;
	case COLUPF:
		colupf = coltab[(v & 0xe)>>1][(v & 0xf0)>>4];
		return;
	case COLUBK:
		colubk = coltab[(v & 0xe)>>1][(v & 0xf0)>>4];
		return;
	case RESP0:
	case RESP1:
	case RESM0:
	case RESM1:
	case RESBL:
		tiareg[a] = px;
		return;
	case GRP0:
		if(tiareg[VDELP0] & 1) {
			grp0d = v;
			return;
		}
		break;
	case GRP1:
		printf("(%d,%d)\t%s\n", py, px, hex(tiareg[GRP1]));
		if(tiareg[VDELP1] & 1) {
			grp1d = v;
			return;
		}
		break;
	case ENABL:
		if(tiareg[VDELBL] & 1) {
			enabld = v;
			return;
		}
		break;
	case HMOVE:
		for(i = 0; i < 5; i++) {
			t = tiareg[RESP0+i] - ((char)tiareg[HMP0+i] >> 4);
			if(t < 0)
				t += 160;
			else if(t > 160)
				t -= 160;
			tiareg[RESP0+i] = t;
		}
		break;
	case HMCLR:
		tiareg[HMP0] = tiareg[HMP1] = tiareg[HMM0] = tiareg[HMM1] = tiareg[HMBL] = 0;
		break;
	case CXCLR:
		cxm0p = cxm1p = cxp0fb = cxp1fb = cxm0fb = cxm1fb = cxblpf = cxppmm = 0;
		break;
	}
	tiareg[a] = v;
}

uint8 tiaport(uint8 p)
{
	uint8 l;

	if((tiareg[VBLANK] & 1<<7) && p <= 3)
		return 0;
	l = inpt & (1<<p);
	if((tiareg[VBLANK] & 1<<6) && p >= 4)
		l = latch & (1<<p);
	return l ? 0x80 : 0;
}

uint8 tiaread(uint16 a)
{
	switch(a) {
	case CXM0P:	return cxm0p;
	case CXM1P:	return cxm1p;
	case CXP0FB:	return cxp0fb;
	case CXP1FB:	return cxp1fb;
	case CXM0FB:	return cxm0fb;
	case CXM1FB:	return cxm1fb;
	case CXBLPF:	return cxblpf;
	case CXPPMM:	return cxppmm;
	case INPT0:
	case INPT1:
	case INPT2:
	case INPT3:
	case INPT4:
	case INPT5:	return tiaport(a-INPT0);
	}
	return 0;
	errorf(1, "tiaread defaulted");
}

void tia(uint8 n)
{
	int i;

	for(i = 0; i < n; i++) {
		if(py < 222 && px < 160) {
			pixel(px, py, colubk);
			playfield();
			missile(0);
			missile(1);
			ball();
			player(0);
			player(1);
			cxm0p |= (pm0 & pp0) << 6;
			cxm0p |= (pm0 & pp1) << 7;
			cxm1p |= (pm1 & pp1) << 6;
			cxm1p |= (pm1 & pp0) << 7;
			cxp0fb |= (pp0 & pbl) << 6;
			cxp0fb |= (pp0 & ppf) << 7;
			cxp1fb |= (pp1 & pbl) << 6;
			cxp1fb |= (pp1 & ppf) << 7;
			cxm0fb |= (pm0 & pbl) << 6;
			cxm0fb |= (pm0 & ppf) << 7;
			cxm1fb |= (pm1 & pbl) << 6;
			cxm1fb |= (pm1 & ppf) << 7;
			cxblpf |= (pbl & ppf) << 7;
			cxppmm |= (pm0 & pm1) << 6;
			cxppmm |= (pp0 & pp1) << 7;
		}
		if(++px >= 228) {
			px = 0;
			py++;
			if(tiareg[VDELBL] & 1)
				tiareg[ENABL] = enabld;
			if(tiareg[VDELP0] & 1)
				tiareg[GRP0] = grp0d;
			if(tiareg[VDELP1] & 1)
				tiareg[GRP1] = grp1d;
		}
	}
}
