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

uint16 fetch16(void)
{
	uint16 r;

	r = read(pc++);
	r |= read(pc++) << 8;
	return r;
}

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

uint8 read(uint16 a)
{
	uint8 r;

	if((a & 0xf000) == 0x1000)
		return rom[a & 0xfff];
	else if(a <= 0x7f);
		//tiaread(a) ;
	else if(a <= 0xff);
		//return ram[a];
	else if(a < 0x2ff); // riot
	errorf(1, "read defaulted");
}

void write(uint8 v, uint16 a)
{
	if(a <= 0x7f);
		//tiawrite(a);
	else if (a <= 0xff);
		//ram[a] = v;
	else if(a < 0x2ff);
		//riot
	else if((a & 0xf000) == 0x1000);

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
	default:	errorf(0, "bad op: %s", hex(op));
	}
}
