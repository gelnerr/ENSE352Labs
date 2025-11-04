

#include "stm32F103RB.h"

/*static void delay(void) {
  for (volatile uint32_t i = 0; i < 500000; ++i) { blank }
}*/

int globalCounter = 0;		//global definition

int main(void){
	
	RCC->APB2ENR |= (1u<<2) | (1u<<4) | (1u<<3);
	
	GPIOA->CRL |= (1u<<20) | (1u<<21);
	
	GPIOA->CRL &= ~(1u<<22) &~ (1u<<23);	//from lab sample (CNF5/MODE5 = 0011)
	
	GPIOC->CRH &= ~((3u<<22)|(3u<<20));		//clear bits for CNF13 and MODE13
	GPIOC->CRH |= (2u<<22);								//CNF13/MODE13 = 1000 (Input with pullup/pulldown, reset state)
	GPIOC->IDR |= (1u<<13);								//enable reading input (PC13)
	
	//Phase 3 Port Initializations
	
	/*BLACK- PB4
	RED - PB6
	GREEN- PB8
	BLUE - PB9*/
	
	GPIOB->CRL &= ~((3u<<18)|(3u<<16)|(3u<<26)|(3u<<24));		//clear bits for CNF4, MODE4 (port B) AND CNF6, MODE6
	GPIOB->CRL |= ((2u<<18)|(2u<<26));											//CNF4/MODE4, CNF6/MODE6 to 1000
	GPIOB->CRH &= ~((3u<<2)|(3u<<0)|(3u<<6)|(3u<<4));				//rinse and repeat for cnf8, cnf9 (and mode8, mode9)
	GPIOB->CRH |= ((2u<<2)|(2u<<6));	
	GPIOB->IDR |= ((1u<<4)|(1u<<6)|(1u<<8)|(1u<<9));
	
	/*D0 : PA0
	D1 : PA1
	D2 : PA4
	D3 : PB0*/
	
	GPIOA->CRL |= (1u<<0) | (1u<<1) | (1u<<4) | (1u<<5) | (1u<<16) | (1u<<17);	//setting all modes to 11 -> output mode, max speed 50MHz
	GPIOA->CRL &= ~(1u<<2) &~ (1u<<3) &~ (1u<<6) &~ (1u<<7) &~ (1u<<18) &~ (1u<<19);
	GPIOB->CRL |= (1u<<0) | (1u<<1);																						//same for port B 
	GPIOB->CRL &= ~(1u<<2) &~ (1u<<3);
	
	/*for (int i=0; i<10; ++i)
	{
		GPIOA->ODR |= (1u<<5);
		delay();
		GPIOA->ODR &= ~(1u<<5);
		delay();
	}*/
	
	int bit_13 = (GPIOC->IDR & (1u<<13))>>13;
	int black = (GPIOB->IDR & (1u<<4))>>4;
	int red = (GPIOB->IDR & (1u<<6))>>6;
	int green = (GPIOB->IDR & (1u<<8))>>8;
	int blue = (GPIOB->IDR & (1u<<9))>>9;
	while (1)
	{
		if((black==0u)||(red==0u)||(green==0u)||(blue==0u)||(bit_13==0u))
		{
			if (black == 0u){		//black button pressed
				GPIOA->ODR |= (1u<<0);		//led ON
			}
			else{
				GPIOA->ODR &= ~(1u<<0);		//led OFF
			}
			
			if (red == 0u){		//red button pressed
				GPIOA->ODR |= (1u<<1);		//led ON
			}
			else{
				GPIOA->ODR &= ~(1u<<1);		//led OFF
			}
			
			if (green == 0u){		//green button pressed
				GPIOA->ODR |= (1u<<4);		//led ON
			}
			else{
				GPIOA->ODR &= ~(1u<<4);		//led OFF
			}
			
			if (blue == 0u){		//blue button pressed
				GPIOB->ODR |= (1u<<0);		//led ON
			}
			else{
				GPIOB->ODR &= ~(1u<<0);		//led OFF
			}
			
			
			if (bit_13 == 0u){
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
		bit_13 = (GPIOC->IDR & (1u<<13))>>13;
		black = (GPIOB->IDR & (1u<<4))>>4;
		red = (GPIOB->IDR & (1u<<6))>>6;
		green = (GPIOB->IDR & (1u<<8))>>8;
		blue = (GPIOB->IDR & (1u<<9))>>9;
	}
	return 0;
}