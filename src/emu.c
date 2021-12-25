#define EXTERN
#include "2600.h"

#define imm()	fetch8()
#define abs()	read(fetch16())
#define absx()	read((fetch16()+rX) & 0xffff)
#define absy()	read((fetch16()+rY) & 0xffff)
#define zp()	read(fetch8())
#define zpx()	read((fetch8()+rX) & 0xff)
#define zpy()	read((fetch8()+rY) & 0xff)
#define ind()	read(indf(0))
#define indx()	read(indf(rX))
#define indy()	read((indf(0)+rY) & 0xffff)

uint16 indf(uint8 o)
{
	uint8 a;

	a = fetch8()+o;
	return read(a) | read((a+1) & 0xff)<<8;
}

void nz(uint8 v)
{
	rP &= ~(NF | ZF);
	if(v == 0)
		rP |= ZF;
	else if(v & 0x80)
		rP |= NF;
}

void step(void)
{
	uint8 op;
	
	op = fetch8();
	switch(op) {
		/* lda */
	case 0xA9:	nz(rA = imm());		return;
	case 0xA5:	nz(rA = zp());		return;
	case 0xB5:	nz(rA = zpx());		return;
	case 0xAD:	nz(rA = abs());		return;
	case 0xBD:	nz(rA = absx());	return;
	case 0xB9:	nz(rA = absy());	return;
	case 0xA1:	nz(rA = indx());	return;
	case 0xB1:	nz(rA = indy());	return;
		/* sta */
	case 0x85:	write(rA, zp());	return;
	case 0x95:	write(rA, zpx());	return;
	case 0x8D:	write(rA, abs());	return;
	case 0x9D:	write(rA, absx());	return;
	case 0x99:	write(rA, absy());	return;
	case 0x81:	write(rA, indx());	return;
	case 0x91:	write(rA, indy());	return;
	default:	errorf(0, "unimplemented op: %s", hex(op));
	}
}

uint8 read(uint16 a)
{
	return rom[a & 0xfff];
}

void tiawrite(uint8 v, uint8 a)
{
/*	switch(a) {
	case WSYNC:;
	}*/
}

void write(uint8 v, uint16 a)
{
	if(a < 0x7f)
		tiawrite(v, a);
}

uint8 fetch8(void)
{
	return read(pc++);
}

uint16 fetch16(void)
{
	uint16 r;

	r = read(pc++);
	r |= read(pc++) << 8;
	return r;
}

int main(int argc, char **argv)
{
	int i, j, v0, v1;
	FILE *f;

	if(argc != 2)
		errorf(0, "need one rom file as argument");
	f = fopen(argv[1], "r");
	if(!f)
		errorf(0, "cannot open file: %s", argv[1]);
	for(i = 0; i < 4096; i++) {
		v0 = fgetc(f);
		v1 = fgetc(f);
		if(v0 == EOF || v1 == EOF) {
			for(j = i; j < 4096; j++)
				rom[j] = 0;
			break;
		}
		if(((v0 < '0' || v0 > '9') && (v0 < 'A' || v0 > 'F')) || ((v1 < '0' || v1 > '9') && (v1 < 'A' || v1 > 'F')))
			errorf(0, "bad rom");
		if(v0 >= 'A' && v0 <= 'F')
			rom[i] = (v0-'A'+10) << 4;
		else
			rom[i] = (v0-'0') << 4;
		if(v1 >= 'A' && v1 <= 'F')
			rom[i] |= v1-'A'+10;
		else
			rom[i] |= v1-'0';
	}
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		errorf(0, "SDL_Init: %s", SDL_GetError());
	scr = SDL_SetVideoMode(640, 480, 32, 0);
	if(!scr)
		errorf(0, "SDL_SetVideoMode: %s", SDL_GetError());
	SDL_WM_SetCaption("2600", "");

	rP |= (BF | 1<<5);
	pc = 0x1000;
	debug();
	for(;;) {
		step();
		debug();
	}
}
