/* A very stupid mmemove to get this building. - REM */

memcpy(d, s, n)
char *d;
char *s;
unsigned n;
{
	char *ds;
	char *ss;
	unsigned i;
	
	ds = (char*)d;
	ss = (char*)s;
	
	for (i = 0; i < n; i++) {
		ds[i] = ss[i];
	}
}

strlen(s)
char* s;
{
	int l;
	l = 0;
	
	while (*s != '\0') {
		l++;
		s++;
	}
	
	return l;
}