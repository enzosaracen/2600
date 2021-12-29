#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL.h>

#ifndef EXTERN
#define EXTERN	extern
#endif

#define SF	3
#define W	160*SF
#define H	192*SF

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

enum {
	VSYNC,
	VBLANK,
	WSYNC,
	RSYNC,
	NUSIZ0,
	NUSIZ1,
	COLUP0,
	COLUP1,
	COLUPF,
	COLUBK,
	CTRLPF,
	REFP0,
	REFP1,
	PF0,
	PF1,
	PF2,
	RESP0,
	RESP1,
	RESM0,
	RESM1,
	RESBL,
	AUDC0,
	AUDC1,
	AUDF0,
	AUDF1,
	AUDV0,
	AUDV1,
	GRP0,
	GRP1,
	ENAM0,
	ENAM1,
	ENABL,
	HMP0,
	HMP1,
	HMM0,
	HMM1,
	HMBL,
	VDELP0,
	VDELP1,
	VDELBL,
	RESMP0,
	RESMP1,
	HMOVE,
	HMCLR,
	CXCLR
};

/*
 *	util.c
 */
char	*hex(uint16);
void	debug(void);
void	errorf(int, char *, ...);

/*
 *	cpu.c	
 */
uint16	indf(uint8);
void	nz(uint8);
void	step(void);
uint8	read(uint16);
void	write(uint16, uint8);
uint16	fetch16(void);
#define fetch8()	read(pc++)

/*
 *	tia.c
 */
void	draw(int, int, uint32);
void	tiawrite(uint16, uint8);
void	tia(uint8);

EXTERN	uint16	pc;
EXTERN	uint8	rP, rX, rY, rA, rS;
EXTERN	uint8	rom[4096], ram[128];
EXTERN	uint8	tiareg[64];
EXTERN	uint16	px, py;
EXTERN	char	hexs[5];

EXTERN	uint8		rast[W*H*4];
EXTERN	SDL_Surface	*scr;
