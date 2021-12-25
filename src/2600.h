#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL.h>

#ifndef EXTERN
#define EXTERN	extern
#endif

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

enum {
	CF = 1<<0,
	ZF = 1<<1,
	IF = 1<<2,
	DF = 1<<3,
	BF = 1<<4,
	VF = 1<<6,
	NF = 1<<7,
};

/*
 *	util.c
 */
char	*hex(uint16);
void	debug(void);
void	errorf(int, char *, ...);

/*
 *	emu.c	
 */
uint16	indf(uint8);
void	nz(uint8);
void	step(void);
uint8	read(uint16);
void	write(uint8, uint16);
void	tiawrite(uint8, uint8);
uint8	fetch8(void);
uint16	fetch16(void);

EXTERN uint16	pc, cc;
EXTERN uint8	rP, rX, rY, rA, rS;
EXTERN uint8	rom[4096];
EXTERN uint8	tiareg[64];
EXTERN char	hexs[5];

EXTERN SDL_Surface	*scr;
