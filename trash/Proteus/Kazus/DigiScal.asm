;=============================================
; DigiScal.asm
; Цифровая шкала с возможностью
; записи промежуточной частоты
; Денисов А.Ю. Тамбов 1999
; Исправлена ошибка в расчете частоты 16.11.2000
;=============================================
;
        LIST    p=16F84
       __CONFIG 03FF1H
;=============================================
                                   ; значения задержки
                                   ; могут быть от 1 до 255
T1          equ       .67          ; грубо
T2          equ       .221         ; точно
                                   ; (подобрано для 4000 kHz)
;=============================================
;
IndF        equ        00h         ; Доступ к памяти через FSR
Timer0      equ        01h         ; TMR0
OptionR     equ        01h         ; Option (RP0=1)
PC          equ        02h         ; Счетчик команд
Status      equ        03h         ; Status
FSR         equ        04h         ; Регистр косвенной адресации
PortA       equ        05h         ; Port A
TrisA       equ        05h         ; Tris A - RP0=1
PortB       equ        06h         ; Port B
TrisB       equ        06h         ; Tris B - RP0=1
EEData      equ        08h         ; EEPROM Data
EECon1      equ        08h         ; EECON1 - RP0=1
EEAdr       equ        09h         ; EEPROM Address
EECon2      equ        09h         ; EECON2 - RP0=1

IntCon      equ        0Bh         ;
;
KeyBuf      equ        0Ch         ; буфер клавиатуры
KeyWait     equ        0Dh         ; ожидание клавиатуры
Count       equ        0Eh         ; временный счетчик
Count1      equ        0Fh         ; еще один
;
LED0        equ       010h         ;
LED1        equ       011h         ;
LED2        equ       012h         ;
LED3        equ       013h         ; ячейки
LED4        equ       014h         ; индикатора
LED5        equ       015h         ;
LED6        equ       016h         ;
LED7        equ       017h         ;
;
Temp        equ       018h         ; временный регистр
LEDIndex    equ       019h         ; указатель LED

TimerL      equ       01ah         ; младший байт счетчика частоты
TimerM      equ       01bh         ; средний байт счетчика частоты
TimerH      equ       01ch         ; старший байт счетчика частоты

IF_L        equ       01dh         ; младший байт ПЧ
IF_M        equ       01eh         ; средний байт ПЧ
IF_H        equ       01fh         ; старший байт ПЧ
;
;=============================================
;                                    Результат в
W           equ        0           ; аккумуляторе
F           equ        1           ; регистре
;
;=============================================
; Flag bits:
CF          equ        0           ; Carry
DC          equ        1           ; DC
ZF          equ        2           ; Zero
;
RP0         equ        5
;=============================================
            org        2100h

            ; частота ПЧ по умолчанию
            DE         008h,064h,070h   ; 5.5 мгц
;
            DE         0h,0h,0h
            DE      "Copyright (C) 1999 Alexander Y Denisov"

;
;=============================================
;
            org        0
            goto    Start
;
;=============================================
; Проверка клавиатуры
;=============================================
;
Inkey
            clrf       PortA       ; RA0..RA3 = 0

            bsf        Status,RP0
            movlw      b'00010011'
            movwf      TrisA       ; RA0,RA1,RA4 input
            bcf        Status,RP0  ;

            movf       PortA,w
            andlw      b'00000011'
            return
;
;=============================================
KeyQuery    ; опрос клавиатуры
            call       Inkey
            addwf      PC,f
            goto       Fun         ; режим установки ПЧ
            goto       plusIF      ; плюс ПЧ
            goto       minusIF     ; минус ПЧ
            goto       Go1         ; и все по новой.
;
Fun
            incf       KeyWait,f
            btfss      KeyWait,3
            goto       Go

            movlw      0
            movwf      KeyWait
Function
            call       Inkey
            addwf      PC,f
            goto       Function    ; ждем отжатия клавиш
            nop
            nop
            nop
            movf       TimerL,w
            movwf      IF_L
            movf       TimerM,w
            movwf      IF_M
            movf       TimerH,w
            movwf      IF_H
            call       Bin2LCD
            goto       Edt
;
;=============================================
;
FunOff
            call       Inkey
            addwf      PC,f
            goto       Fun1
            goto       NextFun     ; ничего не нажато
            goto       NextFun     ; ничего не нажато
            goto       NextFun     ; ничего не нажато
Fun1
            incf       KeyWait,f
            btfss      KeyWait,7
            goto       Edt         ; next 8xLED
Fun11
            call       Inkey
            addwf      PC,f
            goto       Fun11       ; ждем отжатия клавиш
            goto       WrtMem      ; запись в память
            goto       WrtMem      ; запись в память
            goto       WrtMem      ; запись в память
;
;=============================================
; Перекодирование двоичного в код LCD
;=============================================
;
Bin2LCD
            bcf        Status,0    ; clear the carry bit
            movlw      .24
            movwf      Count
            clrf       LED3
            clrf       LED2
            clrf       LED1
            clrf       LED0
loop16
            rlf        TimerL,f
            rlf        TimerM,f
            rlf        TimerH,f
            rlf        LED0,f
            rlf        LED1,f
            rlf        LED2,f
            rlf        LED3,f
;
            decfsz     Count,f
            goto       adjDEC

            swapf      LED3,w
            andlw      0Fh
            movwf      LED7

            movfw      LED3
            andlw      0Fh
            movwf      LED6

            swapf      LED2,w
            andlw      0Fh
            movwf      LED5

            movfw      LED2
            andlw      0Fh
            movwf      LED4

            swapf      LED1,w
            andlw      0Fh
            movwf      LED3

            movfw      LED1
            andlw      0Fh
            movwf      LED2

            swapf      LED0,w
            andlw      0Fh
            movwf      LED1

            movfw      LED0
            andlw      0Fh
            movwf      LED0

            return
;
adjDEC
            movlw      LED0
            movwf      FSR
            call       adjBCD
;
            movlw      LED1
            movwf      FSR
            call       adjBCD
;
            movlw      LED2
            movwf      FSR
            call       adjBCD
;
            movlw      LED3
            movwf      FSR
            call       adjBCD
;
            goto       loop16
;
adjBCD
            movlw      3
            addwf      0,W
            movwf      Count1
            btfsc      Count1,3
            movwf      0
            movlw      30
            addwf      0,W
            movwf      Count1
            btfsc      Count1,7
            movwf      0
;
            retlw      0
;
;=============================================
; Преобразование BCD -> 7 сегментный код
;=============================================

LCDTable
            addwf      PC,F        ; W + PC -> PC
            retlw      b'00111111' ; ..FEDCBA = '0'
            retlw      b'00000110' ; .....CB. = '1'
            retlw      b'01011011' ; .G.ED.BA = '2'
            retlw      b'01001111' ; .G..DCBA = '3'
            retlw      b'01100110' ; .GF..CB. = '4'
            retlw      b'01101101' ; .GF.DC.A = '5'
            retlw      b'01111101' ; .GFEDC.A = '6'
            retlw      b'00000111' ; .....CBA = '7'
            retlw      b'01111111' ; .GFEDCBA = '8'
            retlw      b'01101111' ; .GF.DCBA = '9'
            retlw      b'01110001' ; .GFE...A = 'F'
;
;=============================================
;Main program
;
Start
            bsf        Status,RP0

            movlw      b'00010000' ; RA0..RA3 outputs
            movwf      TrisA       ; RA4 input

            movlw      b'00000000' ; RB0..RB7 outputs
            movwf      TrisB

            clrwdt                 ;
            movlw      b'00100111' ; Prescaler -> Timer0,
            movwf      OptionR     ; 1:256, rising edge
            bcf        Status,RP0  ;

            clrf       Count       ; указатели
            clrf       LEDIndex

            clrf       LED0        ; индикаторы
            clrf       LED1
            clrf       LED2
            clrf       LED3
            clrf       LED4
            clrf       LED5
            clrf       LED6
            clrf       LED7

            clrf       TimerL      ; ячейки счета
            clrf       TimerM
            clrf       TimerH

;=============================================
;Преобразование bin => BCD => код для индикаторов
;
Go
            bcf        EECon1,2    ; запрещение записи
            movlw      0
            movwf      EEAdr
            bsf        Status,RP0
            bsf        EECon1,0
            bcf        Status,RP0  ;
            movf       EEData,w
            movwf      IF_H

            movlw      1
            movwf      EEAdr
            bsf        Status,RP0
            bsf        EECon1,0
            bcf        Status,RP0  ;
            movf       EEData,w
            movwf      IF_M

            movlw      2
            movwf      EEAdr
            bsf        Status,RP0
            bsf        EECon1,0
            bcf        Status,RP0  ;
            movf       EEData,w
            movwf      IF_L

            call       Bin2LCD
;
;=============================================
; Регистры LED0..LED7 заполнены значениями, готовимся
; измерять и отображать
;=============================================
;
            clrf       IntCon      ; обнуление бита переполнения
            clrf       TimerH      ; старший байт измерения
            clrf       Timer0      ; собственно таймер
            clrf       LEDIndex    ; указатель цифры
;
            movlw      .60         ; начальное значение счетчика
            movwf      Count       ; 60 -> Count
;
;=============================================
; Начало измерения и индикации:  RA3 set input
;=============================================
;
            movlw      b'00000000' ; 0 во все порты
            movwf      PortA
;
            bsf        Status,RP0
            movlw      b'00011000' ; RA0..RA2 output,RA3..RA4 input
            movwf      TrisA       ; RA4 input
            bcf        Status,RP0  ;
;
;=============================================
; 7-step cycle of digits
;=============================================
;
LEDCycle    movlw      LED0
            addwf      LEDIndex,W  ; LED1 + LEDIndex -> W

            movwf      FSR         ; W -> FSR
            movf       IndF,W      ; LED(0..6) -> W
            call       LCDTable    ; W -> семисегментный код

            movwf      Temp        ; точка есть?
            movlw      5
            bsf        Status,ZF
            subwf      LEDIndex,W
            btfss      Status,ZF
            goto       NoDot
            bsf        Temp,7
NoDot       movf       Temp,W
            movwf      PortB       ; вывод цифры в PortB

            movf       LEDIndex,W  ; LEDIndex -> W
            movwf      PortA       ; вывод позиции в PortA
;
;=============================================
; Проверка TMR0 на переполнение
;=============================================
;
            btfss      IntCon,2
            goto       DoNothing
            incf       TimerH,F
            bcf        IntCon,2
            goto       O_K
DoNothing   nop
            nop
            nop
;
;=============================================
; The first timing loop
;=============================================
O_K
            movlw      T1
            movwf      Temp
Pause
            decfsz     Temp,F
            goto       Pause

;=============================================
;
            incf       LEDIndex,F
            movlw      7           ; is 7th?
            bcf        Status,ZF
            subwf      LEDIndex,W
            btfss      Status,ZF
            goto       LEDCycle    ; след. цифра
            nop
;
            clrf       LEDIndex
            decfsz     Count,F
            goto       LEDCycle    ; next 7xLED
            nop

;=============================================
; The second timing loop
;=============================================

            movlw      T2
            movwf      Temp

EndPause    decfsz     Temp,F
            goto       EndPause
            nop

;=============================================
; Завершение измерения
;=============================================
Nx
            clrw
            movwf      PortB       ; RB0..RB7 = 0
            movwf      PortA       ; RA0..RA3 = 0

            bsf        Status,RP0
            movlw      b'00010000'
            movwf      TrisA       ; RA4 input
            bcf        Status,RP0  ;
            nop
            nop
;=============================================
; Последняя проверка TMR0 на переполнение
;=============================================
            btfss      IntCon,2
            goto       Analyse
            bcf        IntCon,2
            incf       TimerH,F
;=============================================
; Анализ содержимого предварительного делителя
;=============================================
Analyse
            nop
            movf       Timer0,W    ; средний байт счетчика
            movwf      TimerM      ; TMR0 -> TimerM

            clrf       TimerL
CountIt
            incf       TimerL,F
            bsf        PortA,3     ; _| false impulz
            nop
            bcf        PortA,3     ;    |_
            nop
            movf       Timer0,W    ; actual Timer0 -> W
            bcf        Status,ZF
            subwf      TimerM,W
            btfsc      Status,ZF
            goto       CountIt
            incf       TimerL,F
            comf       TimerL,F
            incf       TimerL,F
            incf       TimerL,F    ; младший байт счетчика
            goto       KeyQuery
minusIF
            comf    IF_L,f
            incf    IF_L,f
            btfsc   Status,ZF
            decf    IF_M,f
            comf    IF_M,f
            btfsc   Status,ZF
            decf    IF_H,f
            comf    IF_H,f
;
            movf    IF_L,w
            addwf   TimerL,f
            btfss   Status,CF
            goto    min11

            incf    TimerM,f
            btfss   Status,ZF
            goto    min11

            incf    TimerH,f
min11
            movf    IF_M,w
            addwf   TimerM,f
            btfsc   Status,CF
            incf    TimerH,f
            movf    IF_H,w
            addwf   TimerH,f
            btfsc   Status,CF   ; результат отрицательный?
            goto    Go1         ; нет
            btfsc   Status,CF   ; а не ноль?
            goto    Go1         ; нет
            comf    TimerL,f    ; преобразование
            incf    TimerL,f
            btfsc   Status,ZF
            decf    TimerM,f
            comf    TimerM,f    ; отрицательного
            btfsc   Status,ZF
            decf    TimerH,f
            comf    TimerH,f    ; результата
Go1
            movlw      0
            movwf      KeyWait
            goto       Go
plusIF
            movf    IF_L,w
            addwf   TimerL,f
            btfss   Status,CF
            goto    pl11

            incf    TimerM,f
            btfss   Status,ZF
            goto    pl11

            incf    TimerH,f
pl11
            movf    IF_M,w
            addwf   TimerM,f
            btfsc   Status,CF
            incf    TimerH,f
            movf    IF_H,w
            addwf   TimerH,f
            goto    Go1
Edt
            clrf       IntCon      ; clear overflow bite
            movlw      0ah
            movwf      LED7        ; признак режима
;=============================================
; 8-step cycle of digits
;=============================================
;
            movlw      b'00000000' ;
            movwf      PortA
;
            bsf        Status,RP0
            movlw      b'00010000' ; RA0..RA3 output,RA4 input
            movwf      TrisA       ;
            bcf        Status,RP0  ;
            clrf       LEDIndex    ; указатель цифры
EdtCycle
            movlw      LED0
            addwf      LEDIndex,W  ; LED1 + LEDIndex -> W

            movwf      FSR         ; W -> FSR
            movf       IndF,W      ; LED(0..6) -> W
            call       LCDTable    ; W -> семисегментный код

            movwf      Temp        ; точка есть?
            movlw      5
            bsf        Status,ZF
            subwf      LEDIndex,W
            btfss      Status,ZF
            goto       NoDot1
            bsf        Temp,7
NoDot1      movf       Temp,W
            movwf      PortB       ; вывод цифры в PortB

            movf       LEDIndex,W  ; LEDIndex -> W
            movwf      PortA       ; вывод позиции в PortA
;
;=============================================
; Timing loop
;=============================================
            movlw      .255
            movwf      Temp
Pause1
            decfsz     Temp,F
            goto       Pause1

;=============================================
;
            incf       LEDIndex,F
            btfss      LEDIndex,3
            goto       EdtCycle    ; след. цифра
;
            clrf       LEDIndex
            goto       FunOff
;
NextFun
            movlw      0           ; выход из цикла редактирования
            movwf      KeyWait
            goto       Edt         ; next 8xLED
;
WrtMem      ;      запись в EEPROM

            movlw      0
            movwf      IntCon      ; запрещение прерываний
            movwf      EEAdr
            movf       IF_H,w
            movwf      EEData
            bsf        Status,RP0
            bsf        EECon1,2    ; разрешение записи
            movlw      055h
            movwf      EECon2
            movlw      0AAh
            movwf      EECon2
            bsf        EECon1,1
wr1
            btfss      EECon1,4
            goto       wr1
            bcf        EECon1,4
            bcf        Status,RP0

            movlw      1
            movwf      EEAdr
            movf       IF_M,w
            movwf      EEData
            bsf        Status,RP0
            bsf        EECon1,2    ; разрешение записи
            movlw      055h
            movwf      EECon2
            movlw      0AAh
            movwf      EECon2
            bsf        EECon1,1
wr2
            btfss      EECon1,4
            goto       wr2
            bcf        EECon1,4
            bcf        Status,RP0

            movlw      2
            movwf      EEAdr
            movf       IF_L,w
            movwf      EEData
            bsf        Status,RP0
            bsf        EECon1,2    ; разрешение записи
            movlw      055h
            movwf      EECon2
            movlw      0AAh
            movwf      EECon2
            bsf        EECon1,1
wr3
            btfss      EECon1,4
            goto       wr3
            bcf        EECon1,4
            bcf        Status,RP0
;
            goto       Go1
;
;=============================================
            end
