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
		//debug();
		/*for(i = 0; i < 8; i++) {
			for(int j = 0; j < 16; j++)
				printf("%s ", hex(ram[i*16 + j]));
			printf("\n");
		}
		printf("\n\n");*/
	}
	SDL_Delay(2000);
}
