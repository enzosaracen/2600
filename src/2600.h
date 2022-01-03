#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL.h>

#ifndef EXTERN
#define EXTERN	extern
#endif

#define SFH	3
#define SFW	5
#define W	160*SFW
#define H	222*SFH

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

enum {
	CXM0P,
	CXM1P,
	CXP0FB,
	CXP1FB,
	CXM0FB,
	CXM1FB,
	CXBLPF,
	CXPPMM,
	INPT0,
	INPT1,
	INPT2,
	INPT3,
	INPT4,
	INPT5,
};

enum {
	SWCHA	= 0x0280,
	SWACNT,
	SWCHB,
	SWBCNT,
	INTIM,
	INSTAT,
	TIM1T	= 0x0294,
	TIM8T,
	TIM64T,
	T1024T,
};

/*
 *	util.c
 */
char	*hex(uint16);
char	*op2str(uint8);
void	debug(void);
void	errorf(int, char *, ...);

/*
 *	cpu.c	
 */
uint16	absx(int);
uint16	absy(int);
uint16	indy(int);
uint16	indf(uint8);
uint8	nz(uint8);
void	push8(uint8);
void	push16(uint16);
uint8	pop8(void);
uint16	pop16(void);
void	dec(uint16);
void	inc(uint16);
uint8	lsr(uint8);
uint8	bcd(uint8);
void	adc(uint8);
void	sbc(uint8);
void	bit(uint8);
void	cmp(uint8, uint8);
uint8	rol(uint8);
uint8	ror(uint8);
void	brk(void);
void	branch(char);
void	timerstep(int);
void	step(void);
uint8	read(uint16);
void	write(uint16, uint8);
uint16	fetch16(void);
#define fetch8()	read(pc++)

/*
 *	tia.c
 */
void	pixel(int, int, uint32);
void	draw(void);
void	tiawrite(uint16, uint8);
uint8	tiaread(uint16);
void	tia(uint8);

EXTERN	uint16	pc;
EXTERN	uint8	rP, rX, rY, rA, rS;
EXTERN	uint8	rom[4096], ram[128];
EXTERN	uint8	tiareg[64];
EXTERN	uint16	px, py;
EXTERN	char	hexs[5];
EXTERN	uint32	colubk, colup0, colup1, colupf, colubk;
EXTERN	uint8	swcha, swacnt, swchb, swbcnt, portA, portB;
EXTERN	uint8	timerflags;
EXTERN	int	interval, cyc, time, tcarry;
extern	uint32	coltab[8][16];

EXTERN	uint8		rast[W*H*4];
EXTERN	SDL_Surface	*scr;
