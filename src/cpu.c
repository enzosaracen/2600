#include "2600.h"

#define imm()	(fetch8())
#define abs()	(fetch16())
#define absx()	((fetch16()+rX) & 0xffff)
#define absy()	((fetch16()+rY) & 0xffff)
#define zp()	(fetch8())
#define zpx()	((fetch8()+rX) & 0xff)
#define zpy()	((fetch8()+rY) & 0xff)
#define ind()	(indf(0))
#define indx()	(indf(rX))
#define indy()	((indf(0)+rY) & 0xffff)

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

	if(a & 0x1000)
		return rom[a & 0xfff];
	else if(a <= 0x7f);
		//return tiaread(a);
	else if(a <= 0xff)
		return ram[a-0x80];
	else if(a < 0x2ff); //riot
	errorf(1, "read defaulted, a: %d", a);
}

void write(uint16 a, uint8 v)
{
	if(a <= 0x7f)
		tiawrite(a, v);
	else if (a <= 0xff)
		ram[a-0x80] = v;
	else if(a < 0x2ff)
		errorf(1, "riot not ready");
	else if(a & 0x1000)
		errorf(0, "write to rom, a: %d", a);
	else
		errorf(1, "write defaulted, v: %d\ta: %d", v, a);
}

void step(void)
{
	uint8 op;

	#define c(n) tia((n)*3)
	
	op = fetch8();
	switch(op) {
		/* lda */
	case 0xA9:	c(2); nz(rA = imm());			return;
	case 0xA5:	c(3); nz(rA = read(zp()));		return;
	case 0xB5:	c(4); nz(rA = read(zpx()));		return;
	case 0xAD:	c(4); nz(rA = read(abs()));		return;
	case 0xBD:	c(4); nz(rA = read(absx()));		return;
	case 0xB9:	c(4); nz(rA = read(absy()));		return;
	case 0xA1:	c(6); nz(rA = read(indx()));		return;
	case 0xB1:	c(5); nz(rA = read(indy()));		return;
		/* sta */
	case 0x85:	c(3); write(zp(), rA);			return;
	case 0x95:	c(4); write(zpx(), rA);			return;
	case 0x8D:	c(4); write(abs(), rA);			return;
	case 0x9D:	c(5); write(absx(), rA);		return;
	case 0x99:	c(5); write(absy(), rA);		return;
	case 0x81:	c(6); write(indx(), rA);		return;
	case 0x91:	c(6); write(indy(), rA);		return;
	default:	errorf(0, "bad op: %s", hex(op));
	}
}
