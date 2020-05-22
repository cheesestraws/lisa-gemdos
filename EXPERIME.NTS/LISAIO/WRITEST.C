#include <mini.h>
#include <gemerror.h>


#define GetBPB(a) bios(7,a)
#define RWAbs(a,b,c,d,e) bios(4,a,b,c,d,e)

#define BUFSIZ 10000
char buf[BUFSIZ];
char ret[BUFSIZ];

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

/***************************************************************************/

main(p)
char* p;
{
	char  *d;
	int i, j, drv, rec, fs, f1, f2, nd;
	BPB *b;
	long err, sp;
	
	/* Get drive letter from command tail using base page pointer.*/
	
 	if (p[0x80])
	{
		p += 0x81;
		while (*p == ' ')	/* scan past initial blanks */
			p++;
		drv = (*p >= 'a') && (*p <= 'z') ? *p -'a' : *p - 'A';
		if (drv & 0xFFF0)	/* out of range? */
			P_Term(1);
	}
	else
		P_Term(2);
		

	/* Init the disk's FATS and root directory. */

	for (i=0; i < BUFSIZ; i++)
		buf[i] = 0x20;
	
	// I'M BORED OF BEING PROTECTED
	// SUPERVISOR MODE AHOY BABEH
	sp = S_State(0L);


// 	for (i=1; i < 8000; i++) {
// 		// write
// 	
// 		C_ConWS("(dev ");
// 		C_ConWI(drv);
// 		C_ConWS(") ");
// 		C_ConWS("Writing block ");
// 		C_ConWI(i);
// 		C_ConWS("\r\n");
// 		err = RWAbs(1,buf,1,i,drv);
// 		if (err == 0) {
// 			C_ConWS("              (ok)\r\n");
// 		} else {
// 			C_ConWS("              (");
// 			C_ConWI((int)err);
// 			C_ConWS(")\r\n");
// 		}
// 		
// 		// verify
// 		for (j=0; j < BUFSIZ; j++) {
// 			ret[j] = '\0';
// 		}
// 		
// 		err = RWAbs(0, ret, 1, i, drv);
// 		if (err == 0) {
// 			C_ConWS("              (ok)\r\n");
// 		} else {
// 			C_ConWS("              (");
// 			C_ConWI((int)err);
// 			C_ConWS(")\r\n");
// 		}
// 		
// 		C_ConWS("              (<- ");
// 		C_ConWI((int)(ret[0]));
// 		C_ConWS(")\r\n");		
// 	}

	// Write
	C_ConWS("(dev ");
	C_ConWI(drv);
	C_ConWS(") ");
	C_ConWS("Writing block ");
	C_ConWI(4);
	C_ConWS("\r\n");
	err = RWAbs(1,buf,1,4,drv);
	if (err == 0) {
		C_ConWS("              (ok)\r\n");
	} else {
		C_ConWS("              (");
		C_ConWI((int)err);
		C_ConWS(")\r\n");
	}
	
	// verify
	for (j=0; j < BUFSIZ; j++) {
		ret[j] = '\0';
	}
	
	err = RWAbs(0, ret, 1, 4, drv);
	if (err == 0) {
		C_ConWS("              (ok)\r\n");
	} else {
		C_ConWS("              (");
		C_ConWI((int)err);
		C_ConWS(")\r\n");
	}
	
	C_ConWS("              (<- ");
	C_ConWI((int)(ret[0]));
	C_ConWS(")\r\n");		

	S_State(sp);

	P_Term0();
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

