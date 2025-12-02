
// using PB5/D4 for our black button, and having the same ports (as lab 4) for switches 

#include "main.h"
#include <stdlib.h>
#include <stdint.h>

static void delay(void) {
  for (volatile uint32_t i = 0; i < 500000; ++i) {  }
}

void feedBack(uint16_t);
static void setup();
static inline void digitDisplay(uint8_t);
static inline void blinker(int,int,int,int);
static void revealAnswer(uint16_t);
static void revealAttempts(uint8_t);
static inline uint8_t read_nibble(void);
static inline uint16_t readGuess(void);
static inline void led_on(int);
static inline void led_off(int);
static inline void debounce();


uint16_t hc_guess = 0x137A;
int n_tries = 3;

int main(void){
	//initializing GPIOs
	setup();

    while (1) {                 // replay forever
        n_tries = 3;
        int won = 0;            // track if we won this round

        for (int i = 0; i < n_tries; i++) {
            uint16_t guess_real = readGuess();
            if (guess_real == hc_guess) {
                // WIN state
                blinker(1,1,1,1);                  // finite blink, then return
                revealAttempts((uint8_t)(i + 1)); 
                won = 1;
                break;
            }
						else{feedBack(guess_real);}
        }

        // LOSS State
        if (!won) {
          blinker(0,1,0,1);
					blinker(1,0,1,0);
					blinker(0,1,0,1);
					blinker(1,0,1,0);
					revealAnswer(hc_guess);                // show 4 nibbles on button presses
        }
			debounce();
    }
    return 0;   // never reached
}

void feedBack(uint16_t guess){
		// Break secret and guess into 4 nibbles: index 0 = most-significant digit
    uint8_t secret[4];
    uint8_t g[4];
    for (int i = 0; i < 4; i++) {
        int shift = 12 - 4 * i;
        secret[i] = (hc_guess >> shift) & 0xF;
        g[i]      = (guess    >> shift) & 0xF;
    }

    // Mastermind-style bookkeeping
    uint8_t used_secret[4] = {0,0,0,0};
    uint8_t used_guess[4]  = {0,0,0,0};
    uint8_t status[4]      = {0,0,0,0};
    // status: 0 = wrong, 1 = correct place, 2 = correct digit wrong place

    // First pass: exact matches (right digit, right spot)
    for (int i = 0; i < 4; i++) {
        if (g[i] == secret[i]) {
            status[i]      = 1;   // exact
            used_secret[i] = 1;
            used_guess[i]  = 1;
        }
    }

    // Second pass: correct digit, wrong spot
    for (int i = 0; i < 4; i++) {
        if (used_guess[i]) continue;  // already exact

        for (int j = 0; j < 4; j++) {
            if (used_secret[j]) continue;
            if (g[i] == secret[j]) {
                status[i]      = 2;   // present but misplaced
                used_secret[j] = 1;
                break;
            }
        }
    }

        // Count how many exact and misplaced
    int exact = 0;
    int misplaced = 0;
    for (int i = 0; i < 4; i++) {
        if (status[i] == 1) exact++;
        else if (status[i] == 2) misplaced++;
    }

    // Cap just in case (though exact + misplaced = 4 anyway)
    if (exact > 4) exact = 4;
    if (misplaced > 4 - exact) misplaced = 4 - exact;

    // Blink pattern:
    // - LEDs 1..exact        = solid ON (correct spot)
    // - LEDs exact+1..exact+misplaced = blinking (right digit, wrong spot)
    // - Remaining LEDs       = OFF

    for (int cycle = 0; cycle < 6; cycle++) {   // 6 blink cycles
        int blink_on = (cycle % 2) == 0;        // ON every second cycle

        for (int led = 1; led <= 4; led++) {
            int index = led - 1; // 0..3

            if (index < exact) {
                // exact matches ? always ON
                led_on(led);
            } else if (index < exact + misplaced) {
                // misplaced ? blinking
                if (blink_on) {
                    led_on(led);
                } else {
                    led_off(led);
                }
            } else {
                // wrong ? OFF
                led_off(led);
            }
        }
        delay();
    }

    // Final state: keep only the exact matches lit, others OFF
    for (int led = 1; led <= 4; led++) {
        int index = led - 1;
        if (index < exact) {
            led_on(led);
        } else {
            led_off(led);
        }
    }
}
/*
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
    return (uint8_t)((b0 << 3) | (b1 << 2) | (b2 << 1) | b3);
}

static inline uint16_t readGuess(void) {
	//dip switch reading
	int black = (GPIOB->IDR & (1u<<5))>>5;
	
	int count = 0;
	uint16_t guess = 0;
	uint8_t nibble;
	
	while (count<4)
	{
		black = (GPIOB->IDR & (1u<<5))>>5;
		while (black == 1u)
		{
			if (count==0){
				led_on(1);
				led_off(2); led_off(3); led_off(4);
			}
			else if (count==1){
				led_on(2);
				led_off(3); led_off(4);
			}
			else if (count==2){
				led_on(3);
				led_off(4);
			}
			else if (count==3){
				led_on(4);
			}
			black = (GPIOB->IDR & (1u<<5))>>5;
			}
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
static inline void digitDisplay(uint8_t nib) {
    if (nib & 0x8) led_on(1); else led_off(1);  // bit 3
    if (nib & 0x4) led_on(2); else led_off(2);  // bit 2
    if (nib & 0x2) led_on(3); else led_off(3);  // bit 1
    if (nib & 0x1) led_on(4); else led_off(4);  // bit 0
}
static inline void blinker(int led1, int led2, int led3, int led4){
	for (int i = 0; i<4; i++){	
		if (led1) led_on(1);
		if (led2) led_on(2);
		if (led3) led_on(3);
		if (led4) led_on(4);
		
		delay();
		
		if (led1) led_off(1);
		if (led2) led_off(2);
		if (led3) led_off(3);
		if (led4) led_off(4);
		
		delay();
	}
}
static void revealAnswer(uint16_t code){
	int index = 0;
	uint8_t nibble = (code >> (12-4*index))&0xF;
	digitDisplay(nibble);
	int looper = 0;
	
	while(looper!=3){
		//wait for button press (active low: 1 = idle, 0 = pressed)
		while ( ((GPIOB->IDR >> 5) & 1u) == 1u ) {
		/*idle*/
			}

		// debounce after press
		for (volatile uint32_t d = 0; d < 80000; ++d) { }

    index = (index + 1) & 0x3;  // 0,1,2,3
    uint8_t nibble = (code >> (12 - 4*index)) & 0xF;
    digitDisplay(nibble);       

    looper++;
		
		while ( ((GPIOB->IDR >> 5) & 1u) == 0u ) {}

    // debounce after release
    for (volatile uint32_t d = 0; d < 80000; ++d) { }
    }
}

static void revealAttempts(uint8_t attempt){
	attempt &= 0x0F;
	digitDisplay(attempt);
	
	for (volatile uint32_t d = 0; d < 80000; ++d) { }
	while ( ((GPIOB->IDR >> 5) & 1u) == 0u ) {}
  for (volatile uint32_t d = 0; d < 80000; ++d) { }
}
static inline void debounce(){
	// wait for press
        while (((GPIOB->IDR >> 5) & 1u) == 1u) { }
        // wait for release
        while (((GPIOB->IDR >> 5) & 1u) == 0u) { }
}
static inline void led_on(int num){
	if (num==1){
		GPIOA->ODR |= (1u<<0);
	}
	else if (num==2){
		GPIOA->ODR |= (1u<<1);
	}
	else if (num==3){
		GPIOA->ODR |= (1u<<4);		
	}
	else if (num==4){
		GPIOB->ODR |= (1u<<0);
	}
}

static inline void led_off(int num){
	if (num==1){
		GPIOA->ODR &= ~(1u<<0);
	}
	else if (num==2){
		GPIOA->ODR &= ~(1u<<1);
	}
	else if (num==3){
		GPIOA->ODR &= ~(1u<<4);		
	}
	else if (num==4){
		GPIOB->ODR &= ~(1u<<0);
	}
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