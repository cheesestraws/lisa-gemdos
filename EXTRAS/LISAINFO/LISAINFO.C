#include <mini.h>
#include <gemerror.h>

char BP();
long LP();

main(p)
char* p;
{	
	char* sp;
	char b;
	long l;
		
	sp = S_State(0L);

	b = BP(0x02AFL);
	if (b > 3) {
		C_ConWS("/!\\ Got gibberish model number.  Are you sure this is a Lisa?\r\n\r\n");
	}
	
	C_ConWS("Model ID: ");
	C_ConWI((int)b);
	C_ConWS(" -- ");
	switch(b) {
		case 0: C_ConWS("Lisa 1"); break;
		case 1: C_ConWS("Lisa 2: Sony, old I/O board"); break;
		case 2: C_ConWS("Lisa 2: Sony, new I/O board"); break;
		case 3: C_ConWS("Lisa 2/10: Sony, new I/O, internal disc"); break;
		default: C_ConWS("?!?!");
	}
	C_ConWS("\r\n");
	
	b = BP(0x01B3L);
	C_ConWS("Boot device: ");
	C_ConWI((int)b);
	C_ConWS("\r\n");
	
	l = LP(0x0294L);
	C_ConWS("Top of memory: 0x");
	dbglx(l);
	C_ConWS("\r\n");
	
	S_State(sp);	
	P_Term(0);
}

char BP(addr)
long addr;
{
	char *loc;
	
	loc = (char*)addr;
	return *loc;
}

long LP(addr)
long addr;
{
	long *loc;
	
	loc = (long*)addr;
	return *loc;
}


C_ConWI(i) 
int i;
{
	char itoabuf[15];
	itoa(i, itoabuf);
	C_ConWS(itoabuf);
}

itoa(n, str)
int n;
char* str;
{
	char buf[15];
	int i; // index into buffer
	int j; // index into output string 
	int negative;
	int rem;
	
	i = 0;
	negative = 0;
	
	
	// Terminating nul here.  We're constructing the string backwards so
	// put the nul first.
	buf[i++] = '\0';
	
	// Zero?
	if (n == 0) 
	{
		buf[i++] = '0';
	}
	
	// Negative?
	if (n < 0) 
	{
		negative = 1;
		n = -n;
	}
	
	// Iterate
	while (n > 0) {
		rem = n % 10;
		n -= rem;
		buf[i++] = rem + '0';
		n = n / 10;
	}
	
	if (negative) {
		buf[i++] = '-';
	}
	
	// Now read the string out backwards.
	j = 0;
	while (i >= 0) {
		str[j++] = buf[--i];
	}
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
