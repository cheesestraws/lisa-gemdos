
#include <portab.h>
#include <obdefs.h>

/*------------------------------*/
/*      inside                  */
/*------------------------------*/
UWORD
inside(x, y, pt)                /* determine if x,y is in rectangle     */
UWORD           x, y;
GRECT           *pt;
{
        if ( (x >= pt->g_x) && (y >= pt->g_y) &&
            (x < pt->g_x + pt->g_w) && (y < pt->g_y + pt->g_h) )
                return(TRUE);
        else
                return(FALSE);
} /* inside */

/*------------------------------*/
/*	rc_equal					*/
/*------------------------------*/
WORD
rc_equal(p1, p2)		/* tests for two rectangles equal	*/
GRECT		*p1, *p2;
{
	if ((p1->g_x != p2->g_x) ||
	    (p1->g_y != p2->g_y) ||
	    (p1->g_w != p2->g_w) ||
	    (p1->g_h != p2->g_h))
		return(FALSE);
	return(TRUE);
}

/*------------------------------*/
/*      rc_copy                 */
/*------------------------------*/
VOID
rc_copy(psbox, pdbox)           /* copy source to destination rectangle */
GRECT   *psbox;
GRECT   *pdbox;
{
        pdbox->g_x = psbox->g_x;
        pdbox->g_y = psbox->g_y;
        pdbox->g_w = psbox->g_w;
        pdbox->g_h = psbox->g_h;
}


/*------------------------------*/
/*      min                     */
/*------------------------------*/
WORD
min(a, b)                       /* return min of two values */
WORD            a, b;
{
        return( (a < b) ? a : b );
}

/*------------------------------*/
/*      max                     */
/*------------------------------*/
WORD
max(a, b)                       /* return max of two values */
WORD            a, b;
{
        return( (a > b) ? a : b );
}
