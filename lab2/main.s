
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; FILENAME: main.s
;;; ENSE 352 - 091 (LAB 2)
;;; DATE: 23-09-2025
;;;
;;; DESCRIPTION: This source file helps us simulate a Cortex-M3 powered 
;;; Nucleo - STM32F103RB microcontroller board using a debugger tool.
;;; Observations and Answers are mentioned in the comments next to
;;; the corresponding code.
;;;
;;; AUTHOR: Glen Issac(200499313)

;;; Directives
  PRESERVE8
  THUMB  
            
            
;;; Equates

INITIAL_MSP    EQU        0x20001000    ; Initial Main Stack Pointer Value    Allocating 
                                              ; 1000 bytes to the stack as it grows down.
                 
                                    
; Vector Table Mapped to Address 0 at Reset
; Linker requires __Vectors to be exported

  AREA    RESET, DATA, READONLY
  EXPORT  __Vectors

__Vectors    DCD        INITIAL_MSP            ; stack pointer value when stack is empty
            DCD        Reset_Handler        ; reset vector
             
  ALIGN

;The program
; Linker requires Reset_Handler

  AREA    MYCODE, CODE, READONLY



  ENTRY
  EXPORT    Reset_Handler

  ALIGN
            
Reset_Handler    

label                                

	;Phase 1
	LDR R0, = 0x00001111
	LDR R1, = 0x00002222
	LDR R2, = 0x00003333
	  
	PUSH{R0}
	PUSH{R1}
	PUSH{R2}	;the stack pointer increments 4 backwards in hex
	
	POP{R0}
	POP{R1,R2}
	PUSH{R0,R1,R2}
	POP{R0,R1,R2}
	
	ALIGN
	
	;Phase 2
	LDR R3, = 0x00000004  ;duplicate to help with factorial (multiplier)
	LDR R4, = 0x00000004	;storing integer n in R3 
	
;Start of the subroutine
Factorial
	SUB R4, R4, #1
	MUL R3, R3, R4
	CMP R4, #1		;checks to see if multiplier is 1 yet or not
	BNE Factorial	;loop back if not done. (Branch if Not Equal)
	ALIGN

	;We get 0x00000018 as a result, which is 24 in Hexadecimal. (4!)
	
	;Phase 3
	LDR R5, = string1
	LDR R6, = string2
Start
	
	MOV R8, #0 	;Vowel Count
	BL countVowels	;subroutine call for counting vowels
	
	MOV R9, R8	;result of string1 in R9
	MOV R8, #0 	;set to 0
	MOV R5, R6	;now string2 will be counted since string2 is at R5
	BL countVowels
	
	MOV R10, R8	;result of string2 in R10

loop
	B loop
				
countVowels  PROC	;subrtn
	
strLoop		;looping within function for each char of string
	LDRB R7,[R5],#1	;loading 1 char into R7, pointer
	CMP R7, #0
	BEQ done
	
	;check for vowels
	CMP R7,#'a'
	BEQ vowelDetected	;Branch if EQual
	CMP R7,#'e'
	BEQ vowelDetected
	CMP R7,#'i'
	BEQ vowelDetected
	CMP R7,#'o'
	BEQ vowelDetected
	CMP R7,#'u'
	BEQ vowelDetected
	
	;uppercase
	CMP R7,#'A'
	BEQ vowelDetected	;Branch if EQual
	CMP R7,#'E'
	BEQ vowelDetected
	CMP R7,#'I'
	BEQ vowelDetected
	CMP R7,#'O'
	BEQ vowelDetected
	CMP R7,#'U'
	BEQ vowelDetected
	
	B strLoop	;back to top for checks
	
vowelDetected 
	ADD R8, R8, #1	;incrementing vowel count
	B strLoop		;loop back to chars after increment
	
done
	BX LR	;Branch back to where Link Register Points
ENDP
	
;our strings
string1
	DCB "ENSE 352 is fun and I am learning ARM assembly!",0

string2
	DCB "Yes I really love it!",0
	  
	ENDP

	END