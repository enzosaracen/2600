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

char *op2str(uint8 op)
{

	switch(op) {
	case 0x69:
	case 0x65:
	case 0x75:
	case 0x6d:
	case 0x7d:
	case 0x79:
	case 0x61:
	case 0x71:	return "adc";
	case 0x29:
	case 0x25:
	case 0x35:
	case 0x2d:
	case 0x3d:
	case 0x39:
	case 0x21:
	case 0x31:	return "and";
	case 0x0a:
	case 0x06:
	case 0x16:	
	case 0x0e:
	case 0x1e:	return "asl";
	case 0x90:	return "bcc";
	case 0xb0:	return "bcs";
	case 0xf0:	return "beq";
	case 0x24:
	case 0x2c:	return "bit";
	case 0x30:	return "bmi";
	case 0xd0:	return "bne";
	case 0x10:	return "bpl";
	case 0x00:	return "brk";
	case 0x50:	return "bvc";
	case 0x70:	return "bvs";
	case 0x18:	return "clc";
	case 0xd8:	return "cld";
	case 0x58:	return "cli";
	case 0xb8:	return "clv";
	case 0xc9:
	case 0xc5:
	case 0xd5:
	case 0xcd:
	case 0xdd:
	case 0xd9:
	case 0xc1:
	case 0xd1:	return "cmp";
	case 0xe0:
	case 0xe4:
	case 0xec:	return "cpx";
	case 0xc0:
	case 0xc4:
	case 0xcc:	return "cpy";
	case 0xc6:
	case 0xd6:
	case 0xce:
	case 0xde:	return "dec";
	case 0xca:	return "dex";
	case 0x88:	return "dey";
	case 0x49:
	case 0x45:
	case 0x55:
	case 0x4d:
	case 0x5d:
	case 0x59:
	case 0x41:
	case 0x51:	return "eor";
	case 0xe6:
	case 0xf6:
	case 0xee:
	case 0xfe:	return "inc";
	case 0xe8:	return "inx";
	case 0xc8:	return "iny";
	case 0x4c:
	case 0x6c:	return "jmp";
	case 0x20:	return "jsr";
	case 0xa9:
	case 0xa5:
	case 0xb5:
	case 0xad:
	case 0xbd:
	case 0xb9: 
	case 0xa1:
	case 0xb1:	return "lda";
	case 0xa2:
	case 0xa6:
	case 0xb6:
	case 0xae:
	case 0xbe:	return "ldx";
	case 0xa0:
	case 0xa4:
	case 0xb4:
	case 0xac:
	case 0xbc:	return "ldy";
	case 0x4a:
	case 0x46:
	case 0x56:	
	case 0x4e:
	case 0x5e:	return "lsr";
	case 0xea:	return "nop";
	case 0x09:
	case 0x05:
	case 0x15:
	case 0x0d:
	case 0x1d:
	case 0x19:
	case 0x01:
	case 0x11:	return "ora";
	case 0x48:	return "pha";
	case 0x08:	return "php";
	case 0x68:	return "pla";
	case 0x28:	return "plp";
	case 0x2a:
	case 0x26:
	case 0x36:
	case 0x2e:
	case 0x3e:	return "rol";
	case 0x6a:
	case 0x66:
	case 0x76:
	case 0x6e:
	case 0x7e:	return "ror";
	case 0x40:	return "rti";
	case 0x60:	return "rts";
	case 0xe9:
	case 0xe5:
	case 0xf5:
	case 0xed:
	case 0xfd:
	case 0xf9:
	case 0xe1:
	case 0xf1:	return "sbc";
	case 0x38:	return "sec";
	case 0xf8:	return "sed";
	case 0x78:	return "sei";
	case 0x85:
	case 0x95:
	case 0x8d:
	case 0x9d:
	case 0x99:
	case 0x81:
	case 0x91:	return "sta";
	case 0x86:
	case 0x96:
	case 0x8e:	return "stx";
	case 0x84:
	case 0x94:
	case 0x8c:	return "sty";
	case 0xaa:	return "tax";
	case 0xa8:	return "tay";
	case 0xba:	return "tsx";
	case 0x8a:	return "txa";
	case 0x9a:	return "txs";
	case 0x98:	return "tya";
	default:	return "illegal (nop)";
	}
}

void debug(void)
{
	int i;
	char *s;

	printf("PC=$%s\n", hex(pc));
	printf("A=$%s", hex(rA));
	printf(" X=$%s", hex(rX));
	printf(" Y=$%s", hex(rY));
	printf(" S=$%s\n", hex(rS));
	printf("COLUBK=$%d\n", colubk);
	printf("px=%d py=%d\n", px, py);
		
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
