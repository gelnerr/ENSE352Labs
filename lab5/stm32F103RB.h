
#ifndef STM32F103RB_H
#define STM32F103RB_H

#include <stdint.h>

typedef struct{
	volatile uint32_t CR;
	volatile uint32_t CFGR;
	volatile uint32_t CIR;
	volatile uint32_t APB2RSTR;
	volatile uint32_t APB1RSTR;
	volatile uint32_t AHBENR;
	volatile uint32_t APB2ENR;
	volatile uint32_t APB1ENR;
	volatile uint32_t BDCR;
	volatile uint32_t CSR;
	
	uint32_t RESERVED;
	volatile uint32_t CFGR2;
} RCC_TypeDef;

typedef struct{
	volatile uint32_t CRL;
	volatile uint32_t CRH;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
} GPIO_TypeDef;

#define RCC			((RCC_TypeDef *) 0x40021000)
#define GPIOA		((GPIO_TypeDef *) 0x40010800)
#define GPIOC		((GPIO_TypeDef *) 0x40011000)
#define GPIOB		((GPIO_TypeDef *) 0x40010C00)

#endif