org 00h

mov dph,#1 
mov dpl,#0 
clr p0.7

loop:
	mov a,#0
	movc a,@a+dptr
	mov p1,a
	inc dpl 
	jmp loop

org 0100h
;Синусоида во всю высоту
;db 07fh, 072h, 066h, 05ah, 04eh, 042h, 037h, 02dh, 024h, 01ch, 014h, 0eh, 09h, 05h, 02h, 00h, 00h, 01h, 03h, 07h, 0bh, 011h, 018h, 020h, 029h, 032h, 03dh, 048h, 054h, 060h, 06ch, 079h, 085h, 092h, 09eh, 0aah, 0b6h, 0c1h, 0cch, 0d5h, 0deh, 0e6h, 0edh, 0f3h, 0f7h, 0fbh, 0fdh, 0feh, 0feh, 0fch, 0f9h, 0f5h, 0f0h, 0eah, 0e2h, 0dah, 0d1h, 0c7h, 0bch, 0b0h, 0a4h, 098h, 08ch, 07fh, 07fh, 072h, 066h, 05ah, 04eh, 042h, 037h, 02dh, 024h, 01ch, 014h, 0eh, 09h, 05h, 02h, 00h, 00h, 01h, 03h, 07h, 0bh, 011h, 018h, 020h, 029h, 032h, 03dh, 048h, 054h, 060h, 06ch, 079h, 085h, 092h, 09eh, 0aah, 0b6h, 0c1h, 0cch, 0d5h, 0deh, 0e6h, 0edh, 0f3h, 0f7h, 0fbh, 0fdh, 0feh, 0feh, 0fch, 0f9h, 0f5h, 0f0h, 0eah, 0e2h, 0dah, 0d1h, 0c7h, 0bch, 0b0h, 0a4h, 098h, 08ch, 07fh, 07fh, 072h, 066h, 05ah, 04eh, 042h, 037h, 02dh, 024h, 01ch, 014h, 0eh, 09h, 05h, 02h, 00h, 00h, 01h, 03h, 07h, 0bh, 011h, 018h, 020h, 029h, 032h, 03dh, 048h, 054h, 060h, 06ch, 079h, 085h, 092h, 09eh, 0aah, 0b6h, 0c1h, 0cch, 0d5h, 0deh, 0e6h, 0edh, 0f3h, 0f7h, 0fbh, 0fdh, 0feh, 0feh, 0fch, 0f9h, 0f5h, 0f0h, 0eah, 0e2h, 0dah, 0d1h, 0c7h, 0bch, 0b0h, 0a4h, 098h, 08ch, 07fh, 07fh, 072h, 066h, 05ah, 04eh, 042h, 037h, 02dh, 024h, 01ch, 014h, 0eh, 09h, 05h, 02h, 00h, 00h, 01h, 03h, 07h, 0bh, 011h, 018h, 020h, 029h, 032h, 03dh, 048h, 054h, 060h, 06ch, 079h, 085h, 092h, 09eh, 0aah, 0b6h, 0c1h, 0cch, 0d5h, 0deh, 0e6h, 0edh, 0f3h, 0f7h, 0fbh, 0fdh, 0feh, 0feh, 0fch, 0f9h, 0f5h, 0f0h, 0eah, 0e2h, 0dah, 0d1h, 0c7h, 0bch, 0b0h, 0a4h, 098h, 08ch, 07fh
;Узкая синусоида
;db 07fh, 079h, 072h, 06ch, 066h, 060h, 05bh, 056h, 051h, 04dh, 049h, 046h, 043h, 041h, 040h, 03fh, 03fh, 03fh, 041h, 042h, 045h, 048h, 04bh, 04fh, 053h, 058h, 05eh, 063h, 069h, 06fh, 075h, 07ch, 082h, 089h, 08fh, 095h, 09bh, 0a0h, 0a6h, 0abh, 0afh, 0b3h, 0b6h, 0b9h, 0bch, 0bdh, 0bfh, 0bfh, 0bfh, 0beh, 0bdh, 0bbh, 0b8h, 0b5h, 0b1h, 0adh, 0a8h, 0a3h, 09eh, 098h, 092h, 08ch, 085h, 07fh, 07fh, 079h, 072h, 06ch, 066h, 060h, 05bh, 056h, 051h, 04dh, 049h, 046h, 043h, 041h, 040h, 03fh, 03fh, 03fh, 041h, 042h, 045h, 048h, 04bh, 04fh, 053h, 058h, 05eh, 063h, 069h, 06fh, 075h, 07ch, 082h, 089h, 08fh, 095h, 09bh, 0a0h, 0a6h, 0abh, 0afh, 0b3h, 0b6h, 0b9h, 0bch, 0bdh, 0bfh, 0bfh, 0bfh, 0beh, 0bdh, 0bbh, 0b8h, 0b5h, 0b1h, 0adh, 0a8h, 0a3h, 09eh, 098h, 092h, 08ch, 085h, 07fh, 07fh, 079h, 072h, 06ch, 066h, 060h, 05bh, 056h, 051h, 04dh, 049h, 046h, 043h, 041h, 040h, 03fh, 03fh, 03fh, 041h, 042h, 045h, 048h, 04bh, 04fh, 053h, 058h, 05eh, 063h, 069h, 06fh, 075h, 07ch, 082h, 089h, 08fh, 095h, 09bh, 0a0h, 0a6h, 0abh, 0afh, 0b3h, 0b6h, 0b9h, 0bch, 0bdh, 0bfh, 0bfh, 0bfh, 0beh, 0bdh, 0bbh, 0b8h, 0b5h, 0b1h, 0adh, 0a8h, 0a3h, 09eh, 098h, 092h, 08ch, 085h, 07fh, 07fh, 079h, 072h, 06ch, 066h, 060h, 05bh, 056h, 051h, 04dh, 049h, 046h, 043h, 041h, 040h, 03fh, 03fh, 03fh, 041h, 042h, 045h, 048h, 04bh, 04fh, 053h, 058h, 05eh, 063h, 069h, 06fh, 075h, 07ch, 082h, 089h, 08fh, 095h, 09bh, 0a0h, 0a6h, 0abh, 0afh, 0b3h, 0b6h, 0b9h, 0bch, 0bdh, 0bfh, 0bfh, 0bfh, 0beh, 0bdh, 0bbh, 0b8h, 0b5h, 0b1h, 0adh, 0a8h, 0a3h, 09eh, 098h, 092h, 08ch, 085h, 07fh
;Пила
db 00h, 06h, 0ch, 012h, 019h, 01fh, 025h, 02bh, 032h, 038h, 03eh, 044h, 04bh, 051h, 057h, 05dh, 064h, 06ah, 070h, 076h, 07dh, 083h, 089h, 08fh, 096h, 09ch, 0a2h, 0a8h, 0afh, 0b5h, 0bbh, 0c1h, 00h, 06h, 0ch, 012h, 019h, 01fh, 025h, 02bh, 032h, 038h, 03eh, 044h, 04bh, 051h, 057h, 05dh, 064h, 06ah, 070h, 076h, 07dh, 083h, 089h, 08fh, 096h, 09ch, 0a2h, 0a8h, 0afh, 0b5h, 0bbh, 0c1h, 00h, 06h, 0ch, 012h, 019h, 01fh, 025h, 02bh, 032h, 038h, 03eh, 044h, 04bh, 051h, 057h, 05dh, 064h, 06ah, 070h, 076h, 07dh, 083h, 089h, 08fh, 096h, 09ch, 0a2h, 0a8h, 0afh, 0b5h, 0bbh, 0c1h, 00h, 06h, 0ch, 012h, 019h, 01fh, 025h, 02bh, 032h, 038h, 03eh, 044h, 04bh, 051h, 057h, 05dh, 064h, 06ah, 070h, 076h, 07dh, 083h, 089h, 08fh, 096h, 09ch, 0a2h, 0a8h, 0afh, 0b5h, 0bbh, 0c1h, 00h, 06h, 0ch, 012h, 019h, 01fh, 025h, 02bh, 032h, 038h, 03eh, 044h, 04bh, 051h, 057h, 05dh, 064h, 06ah, 070h, 076h, 07dh, 083h, 089h, 08fh, 096h, 09ch, 0a2h, 0a8h, 0afh, 0b5h, 0bbh, 0c1h, 00h, 06h, 0ch, 012h, 019h, 01fh, 025h, 02bh, 032h, 038h, 03eh, 044h, 04bh, 051h, 057h, 05dh, 064h, 06ah, 070h, 076h, 07dh, 083h, 089h, 08fh, 096h, 09ch, 0a2h, 0a8h, 0afh, 0b5h, 0bbh, 0c1h, 00h, 06h, 0ch, 012h, 019h, 01fh, 025h, 02bh, 032h, 038h, 03eh, 044h, 04bh, 051h, 057h, 05dh, 064h, 06ah, 070h, 076h, 07dh, 083h, 089h, 08fh, 096h, 09ch, 0a2h, 0a8h, 0afh, 0b5h, 0bbh, 0c1h, 00h, 06h, 0ch, 012h, 019h, 01fh, 025h, 02bh, 032h, 038h, 03eh, 044h, 04bh, 051h, 057h, 05dh, 064h, 06ah, 070h, 076h, 07dh, 083h, 089h, 08fh, 096h, 09ch, 0a2h, 0a8h, 0afh, 0b5h, 0bbh, 0c1h
