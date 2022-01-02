#define EXTERN
#include "2600.h"

int main(int argc, char **argv)
{
	int i, v;
	FILE *f;

	if(argc != 2)
		errorf(0, "need one rom file as argument");
	f = fopen(argv[1], "r");
	if(!f)
		errorf(0, "cannot open file: %s", argv[1]);
	/*for(i = 0; i < 4096; i++) {
		v0 = fgetc(f);
		v1 = fgetc(f);
		if(v0 == EOF || v1 == EOF)
			break;
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
	}*/
	for(i = 0; i < 4096; i++) {
		v = fgetc(f);
		if(v == EOF)
			break;
		rom[i] = v;
	}
	pc = read(0xfffc) | read(0xfffd) << 8;
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		errorf(0, "SDL_Init: %s", SDL_GetError());
	scr = SDL_SetVideoMode(W, H, 32, 0);
	if(!scr)
		errorf(0, "SDL_SetVideoMode: %s", SDL_GetError());
	SDL_WM_SetCaption("2600", "");

	rP |= (BF | 1<<5);
	for(i = 0; i < 1000000; i++) {
		step();
	}
	SDL_Delay(2000);
}
