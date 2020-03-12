 	;######### System Clock - 12Mhz ########
org 500h
	db 23h, 30h, 02ah, 39h, 38h, 37h, 36h, 35h, 34h, 33h, 32h, 31h

ORG 0

	JMP main

org 13h	; int1
	setb p0.0
	setb p0.1
	setb p0.2
	setb p0.3	
	call scanKeyBoard
	CLR p0.0
	CLR p0.1
	CLR p0.2
	CLR p0.3
	reti

org 030h
main:
	CLR SM0 
	SETB SM1 ; 2 режим

	MOV A, PCON 
	SETB ACC.7 
	MOV PCON, A ; set SMOD in PCON to double baud rate

	MOV TMOD, #20H 
	MOV TH1, #243 
	MOV TL1, #243 
	SETB TR1 ; start timer 1
	SETB IT1 ; по срезу
	SETB EX1 ; прерывание от INT1
	SETB EA	; разрешить все прерывания
	mov dptr, #500h
	CLR p0.0
	CLR p0.1
	CLR p0.2
	CLR p0.3
	sjmp $

scanKeyBoard:
	mov r0, #0
	mov p0, #11111110b
 	CALL scanColumn
 	JB F0, gotKey

 	inc r0
 	mov p0, #11111101b
 	CALL scanColumn
 	JB F0, gotKey

	inc r0
	mov p0, #11111011b
 	CALL scanColumn
 	JB F0, gotKey;

	inc r0
	mov p0, #11110111b
 	CALL scanColumn
 	JB F0, gotKey

	ret
    gotKey:
	call sendKey
	ret

scanColumn:
	mov r1, #0
	JNB P0.4, gotCol
 	INC R1 
 	JNB P0.5, gotCol
 	INC R1 
 	JNB P0.6, gotCol
 	INC R1 
	clr f0 	
	RET ; ключ не найден
     gotCol:
 	SETB F0 ; ключ найден, устанавливаем пользовательский флаг
	ret

sendKey:
	mov a, r0
	mov b, #3
	mul ab
	add a, r1
	movc a, @a + dptr
	MOV C, P
	MOV ACC.7, C ; бит чётности
	MOV SBUF, A 
	JNB TI, $ ; ожидание пока установится бит TI
	CLR TI
	ret
