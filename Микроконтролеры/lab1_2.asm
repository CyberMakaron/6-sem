 	;######### System Clock - 12Mhz ########
org 500h
	db 23h, 30h, 02ah, 39h, 38h, 37h, 36h, 35h, 34h, 33h, 32h, 31h ; коды символов соответствующих клавиш

ORG 0

	JMP main

org 13h				; прерывание int1	
	call scanKeyBoard
	CLR p0.0		; сброс строк, чтобы можно было обнаружить следующее нажатие
	CLR p0.1
	CLR p0.2
	CLR p0.3
	reti

org 030h
main:
	CLR SM0 
	SETB SM1 	; 2 режим: 8 бит данных + стартовый + стоповый + бит чётности

	MOV A, PCON 
	SETB ACC.7 
	MOV PCON, A 	; set SMOD in PCON to double baud rate (увеличивается точность)

	MOV TMOD, #20H 	; натсройка таймера: (M1 = 1; M0 = 1) из TH1 записывается в TL1 при переполнении
	MOV TH1, #243 	; 256 - 12Мгц/12/16/4800
	MOV TL1, #243 
	SETB TR1 	; start timer 1
	SETB IT1 	; по срезу
	SETB EX1 	; прерывание от INT1
	SETB EA		; разрешить все прерывания
	mov dptr, #500h	; адрес массива символов
	CLR p0.0	; сброс ножек порта P0, соотв. столбцам, чтобы обнаружить нажатие
	CLR p0.1
	CLR p0.2
	CLR p0.3
	sjmp $

scanKeyBoard:		; определяет нажатую клавишу (R0 - строка, R1 - столбец)
	mov r0, #0	; сканируем 0 строку
	mov p0, #11111110b
 	CALL scanColumn	; сканируем столбцы
 	JB F0, gotKey	; если найдена клавиша, то закончить поиск

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

	ret	; нажатия не произошло
    gotKey:
	call sendKey	; если клавиша найдена, то передать её по UART
	ret

scanColumn:	; сканирует столбцы на признак нажатия клавиши (F = 1 и R1 - столбец, если нажата, иначе F = 0)
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
	movc a, @a + dptr	; получаем символ нажатой клавиши
	MOV C, P	; чётность аккумулятора
	MOV ACC.7, C 	; бит чётности
	MOV SBUF, A 	; передаём байт
	JNB TI, $ 	; ожидание пока установится бит TI - отправка завершена
	CLR TI		; сброс TI
	ret
