#include <mini.h>
#include <gemerror.h>

#define GetBPB(a) bios(7,a)
#define RWAbs(a,b,c,d,e) bios(4,a,b,c,d,e)

#define BPB struct _bpb
BPB /* bios parameter block */
{
	int	recsiz;
	int	clsiz;
	int 	clsizb;
	int	rdlen;	/* root directory length in records */
	int	fsiz;	/* fat size in records */
	int	fatrec;	/* first fat record (of last fat) */
	int	datrec;	/* first data record */
	int	numcl;	/* number of data clusters available */
        int     b_flags;
} ;

#define BUFSIZ 512
char buf[BUFSIZ];

char BP();
long LP();

main(p)
char* p;
{	
	char* sp;
	char b;
	long l;
	BPB *bpb;
	unsigned long capacity;
	int err;
		
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
	
	C_ConWS("\r\n");
	bpb = GetBPB(2);
	C_ConWS("C: BPB claims ");
	C_ConWI(bpb->numcl);
	C_ConWS(" clusters of ");
	C_ConWI(bpb->clsizb);
	C_ConWS(" bytes\r\n");
	
	C_ConWS("\r\n");
	
	capacity = 0;
	errRwAbs(0, 
	
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
