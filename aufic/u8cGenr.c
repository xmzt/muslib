#include <stdio.h>

static void
u8c_byte0_neglen_table()
{
	unsigned int i;
	const char *pre;
	unsigned int len;
	
	for(i = 0; ; i++) {
		if(i % 0x10) pre = ", ";
		else if(0 == i) pre = "int8_t const u8cByte0NeglenTable[0x100] = {\n    ";
		else if(0x100 > i) pre = ",\n    ";
		else {
			printf("\n};\n");
			return;
		} 

		if(0x80 > i) len = -1;
		else if(0xC0 > i) len = 0;
		else if(0xE0 > i) len = -2;
		else if(0xF0 > i) len = -3;
		else if(0xF8 > i) len = -4;
		else if(0xFC > i) len = -5;
		else if(0xFE > i) len = -6;
		else if(0xFF > i) len = -7;
		else len = 0;

		printf("%s%d", pre, len);
	}
}

int
main(int argc, char **argv)
{
	u8c_byte0_neglen_table();
	return 0;
}
