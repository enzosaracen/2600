#include "2600.h"

char *hex(uint16 v)
{
	int i, t, n;

	n = 2;
	if((v & 0xff00) != 0)
		n = 4;
	for(i = 0; i < n; i++) {
		t = v>>(n-1-i)*4 & 0xf;
		if(t >= 10)
			hexs[i] = 'A'+(t-10);
		else
			hexs[i] = '0'+t;
	}
	hexs[i] = 0;
	return hexs;
}

void debug(void)
{
	int i;
	char *s;

	printf("PC=$%s\n", hex(pc));
	printf("A=$%s\n", hex(rA));
	printf(" X=$%s", hex(rX));
	printf(" Y=$%s", hex(rY));
	printf(" S=$%s\n", hex(rS));
		
	s = "NV1BDIZC";
	for(i = 0; i < 8; i++) {
		if(rP>>(7-i) & 1)
			printf("%c", s[i]);
		else
			printf("-");
	}
	printf("\n\n");
}

void errorf(int dbg, char *fmt, ...)
{
	va_list arg;

	fprintf(stderr, "error: ");
	va_start(arg, fmt);
	vfprintf(stderr, fmt, arg);
	va_end(arg);
	fprintf(stderr, "\n");
	if(dbg)
		debug();
	SDL_Quit();
	exit(1);
}
