#include <mini.h>
#include <gemerror.h>

main(p)
char* p;
{	
	char* sp;
	
	long trap;
	long vector;
	long *value;
	
	C_ConWS("Trap       Vector     Address    Value\r\n");
	
	sp = S_State(0L);
	
	for (trap = 1; trap < 17; trap++) {
		vector = trap + 32;
		
		dbglx(trap);
		C_ConWS("  ");
		dbglx(vector);
		C_ConWS("  ");
		
		vector *= 4;
		dbglx(vector);
		C_ConWS("  ");
		
		value = vector;
		dbglx(*value);
		
		C_ConWS("\r\n");
	}

	S_State(sp);	
	P_Term(0);
}

dbglx(n)
long n;
{
	int i;
	long hit;
	char buf[10];
	
	for (i = 0; i < 8; i++) {
		hit = n & 0x0000000F;
		n = n >> 4;
		if (hit < 10) {
			buf[7-i] = '0' + hit;
		} else {
			buf[7 - i] = 'A' + (hit - 10);
		}
	}
	buf[8] = ' ';
	buf[9] = '\0';
	C_ConWS(buf);
}
