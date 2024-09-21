#include "ch32v003fun.h"

#include "ch32v003_GPIO_branchless.h"

#include <stdio.h>

int main()
{
	SystemInit();

	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA;

	// GPIO D0 Push-Pull
	/* GPIOD->CFGLR &= ~( 0xf << ( 4 * 0 ) );
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);

	// GPIO D4 Push-Pull
	GPIOD->CFGLR &= ~(0xf<<(4*4));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*4);

	// GPIO D6 Push-Pull
	GPIOD->CFGLR &= ~(0xf<<(4*6));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*6); */

	// GPIO C7 Push-Pull
	GPIOC->CFGLR &= ~(0xf<<(4*7));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*7);
	GPIO_pinMode( GPIOv_from_PORT_PIN( GPIO_port_C, 3 ), GPIO_pinMode_I_pullUp, GPIO_Speed_In );

	while(1)
	{
		//GPIOD->BSHR = (1<<0) | (1<<4) | (1<<6);	 // Turn on GPIOs
		//GPIOC->BSHR = (1<<0);
		//GPIOA->BSHR = ( 1 << 2 );
		//Delay_Ms( 250 );
		//GPIOD->BSHR = (1<<16) | (1<<(16+4)) | (1<<(16+6)); // Turn off GPIOs
		//GPIOC->BSHR = (1<<16);
		//GPIOA->BSHR = ( 1 << (16+2) );
		//Delay_Ms( 250 );


		uint8_t button_is_pressed = !GPIO_digitalRead( GPIOv_from_PORT_PIN( GPIO_port_C, 3 ) );
		if (button_is_pressed)
		{
			GPIOC->BSHR = ( 1 << 7 );
		}
		else
		{
			GPIOC->BSHR = ( 1 << ( 16 + 7 ) );
		}
	}
}
