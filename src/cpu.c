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
	if(a & 0x1000)
		return rom[a & 0xfff];
	else if(a <= 0x7f);
		//return tiaread(a);
	else if(a <= 0x1ff)
		return ram[a & 0x7f];
	errorf(1, "read defaulted, a: %d", a);
}

void write(uint16 a, uint8 v)
{
	if(a <= 0x7f)
		tiawrite(a, v);
	else if (a <= 0x1ff)
		ram[a & 0x7f] = v;
	else if(a & 0x1000)
		errorf(0, "write to rom, a: %d", a);
	else
		errorf(1, "write defaulted, v: %d\ta: %d", v, a);
}

void push8(uint8 v)
{
	write(0x100 | rS--, v);
}

void push16(uint16 v)
{
	write(0x100 | rS--, v >> 8);
	write(0x100 | rS--, v);
}

uint8 pop8(void)
{
	return read(0x100 | ++rS);
}

uint16 pop16(void)
{
	uint16 v;

	v = read(0x100 | ++rS);
	v |= read(0x100 | ++rS) << 8;
	return v;
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

uint8 lsr(uint8 v)
{
	rP &= ~CF;
	if(v & 0x80)
		rP |= CF;
	return nz(v >> 1);
}

uint8 asl(uint8 v)
{
	rP &= ~CF;
	if(v & 0x80)
		rP |= CF;
	return nz(v << 1);
}

uint8 bcd(uint8 v)
{
	uint8 r;

	r = (v & 0xf) + (v & 0xf0)*10;
	if(r > 99)
		errorf(1, "bad bcd");
	return r;
}

void adc(uint8 v)
{
	int t;

	if(rP & DF) {
		t = bcd(v) + bcd(rA) + (rP & CF);
		rP &= ~CF;
		if(t > 99)
			rP |= CF;
	} else {
		t = rA + v + (rP & CF);
		rP &= ~CF;
		if(t > 0xff)
			rP |= CF;
	}

	rP &= ~(NF | VF | ZF);
	if((t & 0x80) != (rA & 0x80))
		rP |= VF;
	if(t & 0x80)
		rP |= NF;
	if(t == 0)
		rP |= ZF;
	rA = t;
}

void bit(uint8 v)
{
	v &= rA;
	rP &= ~(NF | VF | ZF);
	if(v & 0x80)
		rP |= NF;
	if(v & 0x40)
		rP |= VF;
	if(v == 0)
		rP |= ZF;
}

void cmp(uint8 a, uint8 b)
{
	uint8 t;

	rP &= ~(NF | ZF | CF);
	t = a-b;
	if(t & 0x80)
		rP |= NF;
	if(a >= b)
		rP |= CF;
	if(t == 0)
		rP |= ZF;
}

uint8 rol(uint8 v)
{
	uint8 c;

	c = v & 0x80;
	v = (v << 1) | (rP & CF);
	rP &= ~(NF | ZF | CF);
	if(c)
		rP |= CF;
	if(v == 0)
		rP |= ZF;
	if(v & 0x80)
		rP |= NF;
	return v;
}

uint8 ror(uint8 v)
{
	uint8 c;

	c = v & 1;
	v = (v >> 1) | ((rP & CF) << 7);
	rP &= ~(NF | ZF | CF);
	rP |= c;
	if(v == 0)
		rP |= ZF;
	if(v & 0x80)
		rP |= NF;
	return v;
}

void interrupt(void)
{
	push16(++pc);
	push8(rP | 0x20);
	pc = read(0xfffa);
	pc |= read(0xfffb) << 8;
	rP |= IF;
}

void step(void)
{
	char tc;
	uint16 ta;
	uint8 op;

	#define c(n) tia((n)*3)
	
	op = fetch8();
	switch(op) {
/* adc */
	case 0x69:	c(2); adc(imm());			return;
	case 0x65:	c(3); adc(read(zp()));			return;
	case 0x75:	c(4); adc(read(zpx()));			return;
	case 0x6d:	c(4); adc(read(abs()));			return;
	case 0x7d:	c(4); adc(read(absx()));		return;
	case 0x79:	c(4); adc(read(absy()));		return;
	case 0x61:	c(6); adc(read(indx()));		return;
	case 0x71:	c(5); adc(read(indy()));		return;
/* and */
	case 0x29:	c(2); nz(rA &= imm());			return;
	case 0x25:	c(2); nz(rA &= read(zp()));		return;
	case 0x35:	c(3); nz(rA &= read(zpx()));		return;
	case 0x2d:	c(4); nz(rA &= read(abs()));		return;
	case 0x3d:	c(4); nz(rA &= read(absx()));		return;
	case 0x39:	c(4); nz(rA &= read(absy()));		return;
	case 0x21:	c(6); nz(rA &= read(indx()));		return;
	case 0x31:	c(5); nz(rA &= read(indy()));		return;
/* asl */
	case 0x0a:	c(2); rA = asl(rA);			return;
	case 0x06:	c(5); ta = zp();
			write(ta, asl(read(ta)));		return;
	case 0x16:	c(6); ta = zpx();
			write(ta, asl(read(ta)));		return;
	case 0x0e:	c(6); ta = abs();
			write(ta, asl(read(ta)));		return;
	case 0x1e:	c(7); ta = absx();
			write(ta, asl(read(ta)));		return;
/* bcc */
	case 0x90:	c(2); tc = fetch8();
			if((rP & CF) == 0)
				pc += tc;
								return;
/* bcs */
	case 0xb0:	c(2); tc = fetch8();
			if(rP & CF)
				pc += tc;
								return;
/* beq */
	case 0xf0:	c(2); tc = fetch8();
			if(rP & ZF)
				pc += tc;
								return;
/* bit */
	case 0x24:	c(3); bit(read(zp()));			return;
	case 0x2c:	c(4); bit(read(abs()));			return;
/* bmi */
	case 0x30:	c(2); tc = fetch8();
			if(rP & NF)
				pc += tc;
								return;
/* bne */
	case 0xd0:	c(2); tc = fetch8();
			if((rP & ZF) == 0)
				pc += tc;
								return;
/* bpl */
	case 0x10:	c(2); tc = fetch8();
			if((rP & NF) == 0)
				pc += tc;
								return;
/* brk */
	case 0x00:	c(7); interrupt();			return;
/* bvc */
	case 0x50:	c(2); tc = fetch8();
			if((rP & VF) == 0)
				pc += tc;
								return;
/* bvs */
	case 0x70:	c(2); tc = fetch8();
			if(rP & VF)
				pc += tc;
								return;
/* clc */
	case 0x18:	c(2); rP &= ~CF;			return;
/* cld */
	case 0xd8:	c(2); rP &= ~DF;			return;
/* cli */
	case 0x58:	c(2); rP &= ~IF;			return;
/* clv */
	case 0xb8:	c(2); rP &= ~VF;			return;
/* cmp */
	case 0xc9:	c(2); cmp(rA, imm());			return;
	case 0xc5:	c(3); cmp(rA, read(zp()));		return;
	case 0xd5:	c(4); cmp(rA, read(zpx()));		return;
	case 0xcd:	c(4); cmp(rA, read(abs()));		return;
	case 0xdd:	c(4); cmp(rA, read(absx()));		return;
	case 0xd9:	c(4); cmp(rA, read(absy()));		return;
	case 0xc1:	c(6); cmp(rA, read(indx()));		return;
	case 0xd1:	c(5); cmp(rA, read(indy()));		return;
/* cpx */
	case 0xe0:	c(2); cmp(rX, imm());			return;
	case 0xe4:	c(3); cmp(rX, read(zp()));		return;
	case 0xec:	c(4); cmp(rX, read(abs()));		return;
/* cpy */
	case 0xc0:	c(2); cmp(rY, imm());			return;
	case 0xc4:	c(3); cmp(rY, read(zp()));		return;
	case 0xcc:	c(4); cmp(rY, read(abs()));		return;
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
/* jmp */
	case 0x4c:	c(3); pc = abs();			return;
	case 0x6c:	c(5); pc = ind();			return;
/* jsr */
	case 0x20:	c(6); push16(pc-1); pc = abs();		return;
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
	case 0xa0:	c(2); nz(rY = imm());			return;
	case 0xa4:	c(3); nz(rY = read(zp()));		return;
	case 0xb4:	c(4); nz(rY = read(zpx()));		return;
	case 0xac:	c(4); nz(rY = read(abs()));		return;
	case 0xbc:	c(4); nz(rY = read(absx()));		return;
/* lsr */
	case 0x4a:	c(2); rA = lsr(rA);			return;
	case 0x46:	c(5); ta = zp();
			write(ta, lsr(read(ta)));		return;
	case 0x56:	c(6); ta = zpx();
			write(ta, lsr(read(ta)));		return;
	case 0x4e:	c(6); ta = abs();
			write(ta, lsr(read(ta)));		return;
	case 0x5e:	c(7); ta = absx();
			write(ta, lsr(read(ta)));		return;
/* nop */
	case 0xea:	c(2);					return;
/* ora */
	case 0x09:	c(2); nz(rA |= imm());			return;
	case 0x05:	c(2); nz(rA |= read(zp()));		return;
	case 0x15:	c(3); nz(rA |= read(zpx()));		return;
	case 0x0d:	c(4); nz(rA |= read(abs()));		return;
	case 0x1d:	c(4); nz(rA |= read(absx()));		return;
	case 0x19:	c(4); nz(rA |= read(absy()));		return;
	case 0x01:	c(6); nz(rA |= read(indx()));		return;
	case 0x11:	c(5); nz(rA |= read(indy()));		return;
/* pha */
	case 0x48:	c(3); push8(rA);			return;
/* php */
	case 0x08:	c(3); push8(rP);			return;
/* pla */
	case 0x68:	c(4); nz(rA = pop8());			return;
/* plp */
	case 0x28:	c(4); rP = pop8();			return;
/* rol */
	case 0x2a:	c(2); rA = rol(rA);			return;
	case 0x26:	c(5); ta = zp();
			write(ta, rol(read(ta)));		return;
	case 0x36:	c(6); ta = zpx();
			write(ta, rol(read(ta)));		return;
	case 0x2e:	c(6); ta = abs();
			write(ta, rol(read(ta)));		return;
	case 0x3e:	c(7); ta = absx();
			write(ta, rol(read(ta)));		return;
/* ror */
	case 0x6a:	c(2); rA = ror(rA);			return;
	case 0x66:	c(5); ta = zp();
			write(ta, ror(read(ta)));		return;
	case 0x76:	c(6); ta = zpx();
			write(ta, ror(read(ta)));		return;
	case 0x6e:	c(6); ta = abs();
			write(ta, ror(read(ta)));		return;
	case 0x7e:	c(7); ta = absx();
			write(ta, ror(read(ta)));		return;
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
