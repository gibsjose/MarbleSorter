/*
 * LCD_I2C.c
 *
 * Created: 11/1/2013 9:14:53 AM
 *  Author: Joe
 */ 

#define F_CPU 16000000L

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>
#include "i2clcd.h"
#include "usart.h"

int main(void)
{	
	//PB5 as LED
	DDRB |= _BV(5);
	
	//Configure USART
	USART_Init(MYUBRR);
	
	USART_Send_string("Before i2c_init()\n");
	i2c_init();
	USART_Send_string("After i2c_init()\n");
	
	USART_Send_string("Before lcd_init()\n");
	lcd_init();						//Display initialization
	USART_Send_string("After i2c_init()\n");
	
	USART_Send_string("Before lcd_command()\n");
	lcd_command(LCD_CLEAR);			//Clear LCD
	USART_Send_string("After lcd_command()\n");
	
	while(1)
	{
		USART_Send_string("\nIN LOOP\n");
		PORTB ^= _BV(5);
		_delay_ms(1000);
		lcd_print("HELLO WORLD!");		//Print a string	
	}
}