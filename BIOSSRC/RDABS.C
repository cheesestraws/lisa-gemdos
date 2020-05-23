/*  rdabs.c -						*/
/*
MODIFICATION HISTORY

	20 AUG 85	LTG	Added drive check in rwas() to not return MEDIA_CHANGE
				error if on fixed disk and eject floppy.
	22 MAY 20   REM Shim ProFile code onto Tom Stepleton's LisaIO
NAMES

	LTG	Lou T. Garavaglia
*/

#define MEDIA_CHANGE -14L

/*
**  OLDRWABS - set to true if you want to use the old rwabs routine.  this 
** 	should be removed (and code inside the oldrwabs conditional) after
**	the 'aug 15' release.
*/

#define	OLDRWABS	1

extern char nsect[];
int newdsk; /* sony disk has changed */

char p_buf[532];

/* in profiglu.s */
extern p_setup(/* long drive */);
extern p_init();
extern p_io(/*long bcmd, long rtrspr, char* buf*/);


dskinit()
{

	p_setup((long)0x02);
	p_init();
}

/*
**  rwabs -
**	check for media change, then do something rediculous like
**	call another routine to read each sector, one at a time,
**	into the user's buffer. (ahem).
*/

long	rwabs( wrtflg , bufr , num , recn , drive )
	int	wrtflg , num , recn , drive ;
	char	*bufr;
{

#if	OLDRWABS		
	int i,rtn;
	if (newdsk && (drive == 0))
	{
		newdsk = 0;
		return(MEDIA_CHANGE);
	}
        for (i = 0; i < num; i++,recn++)
        {
	    if (rtn = dskrw(((long) recn),bufr,wrtflg,drive)) break;
	    bufr += 512;
        }
        return(rtn);
#else


	int i,rtn;

	if ((newdsk) && (drive < 2))
	{
		newdsk = 0 ;
		rtn = (int) MEDIA_CHANGE ;
	}
        else for (i = 0; i < num; i++,recn++)
        {
	    if(   rtn = dskrw(((long) recn), bufr, wrtflg, drive )   ) 
		break;
	    bufr += 512;
        }

        return(rtn);
#endif
}

/***** twiggy disk handler ******/
flprw(blk,buffer,rw,drive)
long blk;
int rw,drive;
char *buffer;
{ /* map this sector # onto the disk (# is 0 - 1701) */
	int trk,sec,head,iblk,ns,ns2;
	iblk = blk;
	drive = 0x0080;
	head = 0;
	sec = 0;
	for (trk = 0; trk < 80; sec += nsect[trk++]) if (iblk < sec) break;
	sec -= nsect[--trk];
	iblk -= sec;
	/* almost ready, but now the kicker... heavy duty skewing here */
	ns = (nsect[trk] + 1) >> 1;
	ns2 = ns << 1;
	/* skew by 2 */
	sec = ((iblk << 1) + (iblk / ns)) % ns2;
	/* and now for something completely different */
	fdisk(drive,trk,head,buffer,rw,sec);
	return(0);
}	


/****** profile disk handler *****/

dskrw(blk,buffer,rw,drive)
long blk;
int rw,drive;
char *buffer;
{
	int i,j;
	char x;
	
	long blkcmd;
	int err;
		
	if (drive < 2) return(flprw(blk,buffer,rw,drive));
		
	// if we are writing, then fill the buffer
	if (rw) {	
		// fill tag bytes
		x = 0;
		if (blk == 0) { x = 0xAA; }
	
		for (i = 0; i < 20; i++) {
			p_buf[i] = x;
		}
	
		for (i = 0; i < 512; i++) {
			p_buf[i+20] = buffer[i];
		}
		
	}
	
	blkcmd = (blk << 8) | (long)rw;
	err = p_io(blkcmd, (long)0x0A03, p_buf);
	if (err != 0) {
		return err;
	}
	
	// if we are reading, copy the internal buffer out
	if (!rw) {
		for (i = 0; i < 512; i++) {
			buffer[i] = p_buf[i+20];
		}
	}
	
	return(0); /* ok */
}
