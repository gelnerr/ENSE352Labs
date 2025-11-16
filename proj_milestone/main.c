
// using PB5/D4 for our black button, and having the same ports (as lab 4) for switches 

#include "main.h"
#include <stdlib.h>
#include <stdint.h>

/*static void delay(void) {
  for (volatile uint32_t i = 0; i < 500000; ++i) { blank }
}*/

static void setup();
static inline uint8_t read_nibble(void);
static inline uint16_t readGuess(void);

uint16_t hc_guess = 0x487A;

int main(void){
	//initializing GPIOs
	setup();
	
	//Guess State and reading guesses
	uint16_t guess_real = readGuess();
	
	if (guess_real == hc_guess)	//win state
	{
		//...win code
		GPIOA->ODR &= ~(1u<<0)&~(1<<1)&~(1u<<4);		//led OFF
		GPIOB->ODR &= ~(1u<<0);		//led OFF
		GPIOA->ODR |= (1u<<0);
	}
	else{										//lose state
		//...lose code
		GPIOA->ODR |= (1u<<0)|(1u<<1)|(1u<<4);
		GPIOB->ODR |= (1u<<0);
	}
	return 0;
}

/*int guess(){
	int attempt;
	
	return attempt;
}

void feedBack(int digit){
	
}

void gameLoop(){
	
}

void gameWin(){
	
}

void gameLoss(){
	
}*/

static inline uint8_t read_nibble(void) {
    // read raw bits
    uint8_t b0 = (GPIOB->IDR >> 4) & 1;  // sw0 on PB4
    uint8_t b1 = (GPIOB->IDR >> 6) & 1;  // sw1 on PB6
    uint8_t b2 = (GPIOB->IDR >> 8) & 1;  // sw2 on PB8
    uint8_t b3 = (GPIOB->IDR >> 9) & 1;  // sw3 on PB9

    // invert if switches are active-low (0 when pressed)
    b0 ^= 1;  b1 ^= 1;  b2 ^= 1;  b3 ^= 1;

    // pack into a hex digit (nibble)
    return (uint8_t)((b3 << 3) | (b2 << 2) | (b1 << 1) | b0);
}

static inline uint16_t readGuess(void) {
	//beginning of game code
	int black = (GPIOB->IDR & (1u<<5))>>5;
	int bit_13 = (GPIOC->IDR & (1u<<13))>>13;
	int sw0 = (GPIOB->IDR & (1u<<4))>>4;
	int sw1 = (GPIOB->IDR & (1u<<6))>>6;
	int sw2 = (GPIOB->IDR & (1u<<8))>>8;
	int sw3 = (GPIOB->IDR & (1u<<9))>>9;
	
	int count = 0;
	uint16_t guess = 0;
	uint8_t nibble;
	
	while (count<4)
	{
		black = (GPIOB->IDR & (1u<<5))>>5;
		while (black == 1u)
		{
			if((sw0==0u)||(sw1==0u)||(sw2==0u)||(sw3==0u)||(bit_13==0u))
			{
				if (sw0 == 0u){		//sw0 at Logic HIGH
					GPIOA->ODR |= (1u<<0);		//led ON
				}
				else{
					GPIOA->ODR &= ~(1u<<0);		//led OFF
				}
				
				if (sw1 == 0u){		//sw1 at Logic HIGH
					GPIOA->ODR |= (1u<<1);		//led ON
				}
				else{
					GPIOA->ODR &= ~(1u<<1);		//led OFF
				}
				
				if (sw2 == 0u){		//sw2 at Logic HIGH
					GPIOA->ODR |= (1u<<4);		//led ON
				}
				else{
					GPIOA->ODR &= ~(1u<<4);		//led OFF
				}
				
				if (sw3 == 0u){		//sw3 at Logic HIGH
					GPIOB->ODR |= (1u<<0);		//led ON
				}
				else{
					GPIOB->ODR &= ~(1u<<0);		//led OFF
				}

				if ((bit_13 == 0u)){
					GPIOA->ODR |= (1u<<5);		//led ON
				}
				else{
					GPIOA->ODR &= ~(1u<<5);		//led OFF
				}
			}
			else{
				GPIOA->ODR &= ~(1u<<0) &~ (1u<<1) &~ (1u<<4) &~ (1u<<5);
				GPIOB->ODR &= ~(1u<<0);
			}
			//Update Bit States for all
			bit_13 = (GPIOC->IDR & (1u<<13))>>13;
			sw0 = (GPIOB->IDR & (1u<<4))>>4;
			sw1 = (GPIOB->IDR & (1u<<6))>>6;
			sw2 = (GPIOB->IDR & (1u<<8))>>8;
			sw3 = (GPIOB->IDR & (1u<<9))>>9;
			black = (GPIOB->IDR & (1u<<5))>>5;
			}
		//feedback code also goes here
		//updating guess after black button is pressed
		nibble = read_nibble();
		guess = (guess<<4)|nibble;
		count++;
			
		// debounce + wait for release
		// purely cuz the black button was being pressed multiple times
    for (volatile uint32_t d = 0; d < 80000; ++d) { /* debounce */ }
    while (((GPIOB->IDR >> 5) & 1) == 0) { /* wait release */ }
    for (volatile uint32_t d = 0; d < 80000; ++d) { /* debounce */ }
		}
	return guess;
}

static void setup(){
	RCC->APB2ENR |= (1u<<2) | (1u<<4) | (1u<<3);
	
	GPIOA->CRL |= (1u<<20) | (1u<<21);
	
	GPIOA->CRL &= ~(1u<<22) &~ (1u<<23);	//from lab sample (CNF5/MODE5 = 0011)
	
	GPIOC->CRH &= ~((3u<<22)|(3u<<20));		//clear bits for CNF13 and MODE13
	GPIOC->CRH |= (2u<<22);								//CNF13/MODE13 = 1000 (Input with pullup/pulldown, reset state)
	GPIOC->IDR |= (1u<<13);								//enable reading input (PC13)
	
	/*sw0- PB4
	sw1 - PB6
	sw2- PB8
	sw3 - PB9
	BLACK Button - PB5*/
	
	GPIOB->CRL &= ~((3u<<18)|(3u<<16)|(3u<<26)|(3u<<24)|(3u<<20)|(3u<<22));		//clear bits for CNF4, MODE4 (port B) AND CNF6, MODE6 //UPDATE: CNF5 as well for black button
	GPIOB->CRL |= ((2u<<18)|(2u<<26)|(2u<<22));																//CNF4/MODE4, CNF6/MODE6 to 1000 UPDATE: CNF5/MODE5 to 1000
	GPIOB->CRH &= ~((3u<<2)|(3u<<0)|(3u<<6)|(3u<<4));													//rinse and repeat for cnf8, cnf9 (and mode8, mode9)
	GPIOB->CRH |= ((2u<<2)|(2u<<6));	
	GPIOB->IDR |= ((1u<<4)|(1u<<6)|(1u<<8)|(1u<<9)|(1u<<5));									//added IDR for black button (1u<<5).
	
	/*D0 : PA0
	D1 : PA1
	D2 : PA4
	D3 : PB0*/
	
	GPIOA->CRL |= (1u<<0) | (1u<<1) | (1u<<4) | (1u<<5) | (1u<<16) | (1u<<17);	//setting all modes to 11 -> output mode, max speed 50MHz
	GPIOA->CRL &= ~(1u<<2) &~ (1u<<3) &~ (1u<<6) &~ (1u<<7) &~ (1u<<18) &~ (1u<<19);
	GPIOB->CRL |= (1u<<0) | (1u<<1);																						//same for port B 
	GPIOB->CRL &= ~(1u<<2) &~ (1u<<3);
	//setup ends
}