/* A very stupid mmemove to get this building. - REM */

extern int *xmgetblk();
extern xmfreblk();

bmove(s, d, n)
char* s;
char* d;
int n;
{
	char* buf;
	int paras;
	int i;
	
	// find length of temporary buffer becaue it's allocated in 16-byte chunks
	paras = (n >> 4) + 1;
	
	buf = (char*)xmgetblk(paras);
	for (i = 0; i < n; i++) {
		buf[i] = s[i];
	}
	for (i=0; i < n; i++) {
		d[i] = buf[i];
	}
	
	xmfreblk(buf);
}

bzero(d, n)
char* d;
int n;
{
	int i;
		
	for (i = 0; i < n; i++) {
		d[i] = 0;
	}
}


lbmove(s, d, n)
char* s;
char* d;
long n;
{
	char* buf;
	int paras;
	long i;
	
	// find length of temporary buffer becaue it's allocated in 16-byte chunks
	paras = (int)((n >> 4) + 1);
	
	buf = (char*)xmgetblk(paras);
	for (i = 0; i < n; i++) {
		buf[i] = s[i];
	}
	for (i=0; i < n; i++) {
		d[i] = buf[i];
	}
	
	xmfreblk(buf);
}

bfill(d, c, n)
char* d;
int c;
int n;
{
	int i;
	for (i = 0; i < n; i++) {
		d[i] = (char)c;
	}
}

kprintf() { return; }