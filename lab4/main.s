;startup.s Source code for my first program on the ARM Cortex M3
;Author - Dave Duguid
;Modified August 2012 Trevor Douglas
; Directives
	PRESERVE8
	THUMB

; Vector Table Mapped to Address 0 at Reset, Linker requires __Vectors to be exported
	AREA RESET, DATA, READONLY
	EXPORT 	__Vectors


__Vectors DCD 0x20002000 ; stack pointer value when stack is empty
	DCD Reset_Handler ; reset vector

	ALIGN
		
;; How to Access (R/W) an external register

INITIAL_MSP	EQU	0x20001000	    ; Initial Main Stack Pointer Value
RCC_APB2ENR	EQU	0x40021018	    ;  APB2 Peripheral Clock Enable Register
GPIOA_CRL	EQU	0x40010800      ; Port Configuration Register for PA7 -> PA0
GPIOA_ODR   EQU 0x4001080C
GPIOC_CRH	EQU	0x40011004		; Port Register for PC 13
GPIOC_IDR	EQU 0x40011008		; IDR for PC 13

;My program, Linker requires Reset_Handler and it must be exported
	AREA MYCODE, CODE, READONLY
	ENTRY

	EXPORT Reset_Handler


Reset_Handler ;We only have one line of actual application code

	BL instantiate	;Branch to Phase 1
	
	BL blinkLoop	;Branch to Phase 2
	
	BL press 		;Branch to Phase 3
	
forever
	B forever		;forever loop to stop
	
instantiate PROC
	LDR	R6, = RCC_APB2ENR	; R6 will contain the address of the register
	MOV	R0, #0x14	; Turns on GPIOA AND GPIOC Clock
	STR	R0, [R6]    ; Store R0 at the address specified by R6

	LDR	R6, = GPIOA_CRL	    ; CRL determines gpio pins 0-7
	LDR	R0,[R6]             ; Get the current value at the address specified by R6
	ORR R0, #0x00300000     ; Set MODE5 to OUTPUT 50MHz
	AND R0, #0xFF3FFFFF     ; SET CNF5 to GENERAL PURPOSE OUTPUT (PUSH PULL)
	STR R0, [R6]            ; Store R0 at the address specified by R6
	
	LDR R6, = GPIOA_ODR
	LDR R0, [R6]
	LDR R0, =0x20			; turning LED on? (DOUBT)
	STR R0, [R6]
	
	LDR R6, = GPIOC_CRH		; Rinse and Repeat for values specific to port C
	LDR R0, [R6]
	ORR R0, #0x00400000		; MODE13 to Input (Reset State)
	AND R0, #0xFF4FFFFF		; CNF13 to Floating Input (the bits are 0100 = 4)
	STR R0, [R6]

	LDR R6, =0x0 ; EXTRA INSTRUCTION TO PREVENT FREEZE (IGNORE THIS)

	ALIGN
	
	ENDP

blinkLoop PROC
	PUSH {R0, R1, R2, R3, R6, LR}	; save current state
	LDR R6, = GPIOA_ODR
	LDR R0, [R6]
	
	MOV R2, #10				; will decrement this counter every blink
	
loop
	LDR R0, =0x20			; turning LED on
	STR R0, [R6]
	
	MOV R0, #500
	BL delay_ms				;delay loop b/w blinks
	
	SUBS R2, R2, #1			;decrement
	
	LDR R0, =0x00			; turning LED off
	STR R0, [R6]
	
	MOV R0, #500
	BL delay_ms				;delay loop b/w blinks
	
	SUBS R2, R2, #1			;decrement
	BNE loop
	
	POP{R0, R1, R2, R3, R6, LR}
	BX LR
	
	ENDP

delay_ms
    PUSH {R1,R2,LR}
    MOV  R1, R0              ; R1 = ms counter
ms_loop
    MOV R2, #1500         	 ; K: ~1 ms @ ~72 MHz
inner
    SUBS R2, R2, #1
    BNE  inner
    SUBS R1, R1, #1
    BNE  ms_loop
    POP  {R1,R2,PC}			; popping PC returns us directly back to last line
	
	ALIGN
		
;Phase 3
press PROC
    LDR  R6, =GPIOC_IDR     
    LDR  R0, [R6]           
    AND  R0, #(1 << 13)		;LSL 13 (for LED at 13th bit)
	CMP R0, #0x2000			;bit 13 = 1
    BEQ  not_pressed         
    B    turn_on        
	ENDP

turn_on PROC
    LDR  R6, =GPIOA_ODR       ; LED at PA5
    LDR  R1, =0x20            ; turn LED ON (bit5=1)
    STR  R1, [R6]
    B    press                ; go back and check again
	ENDP


not_pressed PROC
    LDR  R6, =GPIOA_ODR       ; LED at PA5
    LDR  R1, =0x00            ; turn LED OFF (bit5=0)
    STR  R1, [R6]
    B    press                ; go back and check again
	ENDP
	
	END