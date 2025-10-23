;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; FILENAME: main.s
;;; ENSE 352 - 091 (LAB 3)
;;; DATE: 07-10-2025
;;;
;;; DESCRIPTION: This source file helps us simulate a Cortex-M3 powered 
;;; Nucleo - STM32F103RB microcontroller board using a debugger tool.
;;; Observations and Answers are mentioned in the comments next to
;;; the corresponding code.
;;;
;;; AUTHOR: Glen Issac(200499313)

; Directives
	PRESERVE8
	THUMB

; Vector Table Mapped to Address 0 at Reset, Linker requires __Vectors to be exported
	AREA RESET, DATA, READONLY
	EXPORT 	__Vectors


__Vectors DCD 0x20002000 ; stack pointer value when stack is empty
	DCD Reset_Handler ; reset vector

	ALIGN


;My program, Linker requires Reset_Handler and it must be exported
	AREA MYCODE, CODE, READONLY
	ENTRY

	EXPORT Reset_Handler


Reset_Handler ;We only have one line of actual application code
	
	;Phase 1
	BL phase1
	
	;Phase 2
	LDR R0, = 0x12345678	;input register
	LDR R1, = 0x21			; bit 5 = 1, bit 3:0 = 1
	BL rot_left_right1

;Phase 1 code
phase1 PROC
	PUSH{LR} ;Storing last working address space 
	
	LDR R0, = 0x3456789A ;sample input
	BL phase1A
	
	LDR R0, = 0x3456789A ;sample input
	BL phase1B
	
	LDR R0, = 0x3456789A ;sample input
	BL phase1C
	
	;Only one pop is needed since we are coming back to our BL Lines
	POP{LR}	;Popping it back out after last subroutine has been executed
	BX LR
	ENDP
	
phase1A PROC
	LDR R2, = 0x1			;initiating our number for the AND operation
	MOV R2, R2, LSL #11		;left bit shifting for 11 bits
	AND R1, R0, R2			;if AND, returns 0x800, else 0x000
	MOV R1, R1, LSR #11 	;shifting back to have an output of 1 or 0
	BX LR
	ENDP

	ALIGN

phase1B PROC
	;bit 3 = 2^3 = 0x8 (8) ;bit 7 = 2^7 = 0x80 (128)
	ORR R0, R0, #0x8			;sets bit 3 to 1 (already 1 for sample input)
	AND R0, R0, #0xFFFFFF7F		;sets bit 7 to 0 (&0b...0111 1111)
	BX LR
	ENDP

	ALIGN
		
phase1C PROC
	MOV R1, #0				;Clearing answer register
	MOV R3, #32				;bit count for 32 bits
	
counter
	AND R2, R0, #1			;isolate LSB
	ADD R1, R1, R2			;1 is added if bit in R2 was 1, else adds 0
	MOV R0, R0, LSR #1		;right shift by 1
	SUBS R3, R3, #1			;decrement counter, changes Z flag for BNE to check
	BNE counter				;branch back to counter if bits remain
	BX LR

;Phase 2 code
rot_left_right1 PROC
    PUSH {R0, R1, R2, R3, R4, R5, R6, LR} ; Save state
    
    LDR R6, = 0xFFFF                ; R6 = 0x0000FFFF (lower 16-bit mask)
    AND R3, R1, 0xF                 ; R3 = rotation amount (0-15)
    CMP R3, #0
    BEQ no_rotation                 ; If 0, skip all rotation logic
    
    AND R4, R0, R6                  ; R4 = lower 16 bits (0x0000[bits])
    BIC R5, R0, R6                  ; R5 = upper 16 bits (0x[bits]0000)
	;footnote for demo: BIC is AND with second operand as its complement ('R6)
    
    TST R1, #(1<<5)                 ; Test bit 5 for direction (1 = Left, 0=Right)
    BNE rotate_left
    
rotate_right
    ; 1. Duplicate the 16 bits: 0x0000[bits] -> 0x[bits][bits]
    ORR R4, R4, R4, LSL #16
    ; 2. Shift right. The bits from the top wrap into the bottom.
    LSR R4, R4, R3                  ; R4 = 0x0000[rotated_bits]
    B rot_final                     ; Skip left-rotate logic
    
rotate_left
    ; 16bit left rotate by n = 16bit right rotate by (16-n)
    RSB R3, R3, #16                 ; R3 = 16 - n (reverse subtraction)
    
    ; Fall through to the right-rotate logic to execute it
    ORR R4, R4, R4, LSL #16         ; R4 = 0x[bits][bits]
    LSR R4, R4, R3                  ; R4 = 0x0000[rotated_bits]
    
rot_final
    ; R4 is already masked (0x0000[rotated_bits]) from the LSR
    ORR R2, R5, R4                  ; R2 = 0x[upper_bits][rotated_bits]
    B break_rot                     ; Jump to exit

no_rotation
    MOV R2, R0                      ; No rotation, so set R2 = R0

break_rot
    POP {R0, R1, R2, R3, R4, R5, R6, LR} ; Restore state
    BX LR                           ; Return (was missing/implied in original)
    
    ALIGN
    END