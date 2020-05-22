* lisa_io ProFile C glue

* these defined in profilio.s
xref _pfhdstup
xref _pfhdinit
xref _pfhdio

xdef _p_setup
xdef _p_init
xdef _p_io

* _p_setup(long id)
_p_setup:
	MOVE.L 4(SP), D0
	JSR _pfhdstup
	RTS
	
* p_init()
_p_init:
	JSR _pfhdinit
	RTS
	
*_p_io(long bcmd, long rtrspr, char* buf)
_p_io:
	MOVE.L 4(SP), D1 * bcmd
	MOVE.L 8(SP), D2 * trtspr
	MOVE.L 12(SP), A0
	JSR _pfhdio
	RTS