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

uint8 nz(uint8 v)
{
	rP &= ~(NF | ZF);
	if(v == 0)
		rP |= ZF;
	else if(v & 0x80)
		rP |= NF;
	return v;
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

void dec(uint16 a)
{
	uint8 v;
	
	v = read(a);
	write(a, nz(v-1));
}

void inc(uint16 a)
{
	uint8 v;

	v = read(a);
	write(a, nz(v+1));
}

void step(void)
{
	uint8 op;

	#define c(n) tia((n)*3)
	
	op = fetch8();
	switch(op) {
		/* dec */
	case 0xc6:	c(5); dec(zp());			return;
	case 0xd6:	c(6); dec(zpx());			return;
	case 0xce:	c(6); dec(abs());			return;
	case 0xde:	c(7); dec(absx());			return;
		/* dex */
	case 0xca:	c(2); nz(--rX);				return;
		/* dey */
	case 0x88:	c(2); nz(--rY);				return;
		/* eor */
	case 0x49:	c(2); nz(rA ^= imm());			return;
	case 0x45:	c(3); nz(rA ^= read(zp()));		return;
	case 0x55:	c(4); nz(rA ^= read(zpx()));		return;
	case 0x4d:	c(4); nz(rA ^= read(abs()));		return;
	case 0x5d:	c(4); nz(rA ^= read(absx()));		return;
	case 0x59:	c(4); nz(rA ^= read(absy()));		return;
	case 0x41:	c(6); nz(rA ^= read(indx()));		return;
	case 0x51:	c(5); nz(rA ^= read(indy()));		return;
		/* inc */
	case 0xe6:	c(5); inc(zp());			return;
	case 0xf6:	c(6); inc(zpx());			return;
	case 0xee:	c(6); inc(abs());			return;
	case 0xfe:	c(7); inc(absx());			return;
		/* inx */
	case 0xe8:	c(2); nz(++rX);				return;
		/* iny */
	case 0xc8:	c(2); nz(++rY);				return;
		/* lda */
	case 0xa9:	c(2); nz(rA = imm());			return;
	case 0xa5:	c(3); nz(rA = read(zp()));		return;
	case 0xb5:	c(4); nz(rA = read(zpx()));		return;
	case 0xad:	c(4); nz(rA = read(abs()));		return;
	case 0xbd:	c(4); nz(rA = read(absx()));		return;
	case 0xb9:	c(4); nz(rA = read(absy()));		return;
	case 0xa1:	c(6); nz(rA = read(indx()));		return;
	case 0xb1:	c(5); nz(rA = read(indy()));		return;
		/* ldx */
	case 0xa2:	c(2); nz(rX = imm());			return;
	case 0xa6:	c(3); nz(rX = read(zp()));		return;
	case 0xb6:	c(4); nz(rX = read(zpy()));		return;
	case 0xae:	c(4); nz(rX = read(abs()));		return;
	case 0xbe:	c(4); nz(rX = read(absy()));		return;
		/* ldy */
		/* sta */
	case 0x85:	c(3); write(zp(), rA);			return;
	case 0x95:	c(4); write(zpx(), rA);			return;
	case 0x8d:	c(4); write(abs(), rA);			return;
	case 0x9d:	c(5); write(absx(), rA);		return;
	case 0x99:	c(5); write(absy(), rA);		return;
	case 0x81:	c(6); write(indx(), rA);		return;
	case 0x91:	c(6); write(indy(), rA);		return;
		/* stx */
	case 0x86:	c(3); write(zp(), rX);			return;
	case 0x96:	c(4); write(zpy(), rX);			return;
	case 0x8e:	c(4); write(abs(), rX);			return;
		/* sty */
	default:	errorf(0, "bad op: %s", hex(op));
	}
}
