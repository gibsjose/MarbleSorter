/************************************************************************/
/* File: main.cpp														*/
/* Author: Joe Gibson and Jesse Millwood								*/
/* Date: 11/5/13														*/
/* Course: EGR 326														*/	
/* Description: main.cpp implements the main file for the marble sorter	*/
/*				software. This file interfaces with the Sorter, Marble,	*/
/*				Servo, and other classes								*/
/*																		*/
/* Grand Valley State University, 2013									*/
/************************************************************************/
#define F_CPU 16000000L

#include <avr/interrupt.h>			//AVR interrupt library
#include <avr/wdt.h>				//AVR Watchdog timer library
#include <avr/eeprom.h>				//AVR EEPROM library
#include <Arduino.h>				//Arduino library
#include <LiquidCrystal_I2C.h>		//I2C LCD library
#include <string.h>					//String library
#include "Global.h"					//Global header file
#include "Marble.h"					//Marble class definition
#include "Servo.h"					//Servo class definition
#include "Sorter.h"					//Sorter class definition

//Create the LCD object
LiquidCrystal_I2C lcd(I2C_ADDRESS, EN, RW, RS, D4, D5, D6, D7, BL, BL_POL);

//Create the sorter object
Sorter sorter;

int ResetCount = 0;
int StartStopCount = 0;

/************************************************************************/
/* SETUP AND LOOP														*/
/************************************************************************/
/************************************************************************/
/* Setup																*/
/************************************************************************/
void setup(void)
{	
	//Reset WDT right away
	wdt_reset();
	
	//Initialize everything
	InitPortDirections();
	InitTimers();
	InitADC();
	InitWDT();
	InitEEPROM();
	InitSorter();
	InitLCD();
	
	//Enable global interrupts
	sei();
}

/************************************************************************/
/* Main Loop															*/
/************************************************************************/
void loop(void)
{	
	static char tmp[LINE_LEN];
	static char dots = 0;
	static bool printIdleScreen = true;
	
	//Print the idle screen if necessary
	if(printIdleScreen)
	{
		PrintIdleScreen();
		printIdleScreen = false;
	}
	
	//Reset WDT
	wdt_reset();
	
	/********/
	/* Sort */
	/********/
	if((sorter.GetStartStopButtonAction() == Press) && (sorter.State == IdleState))
	{	
		sorter.ButtonActionCompleted();
		
		//Check if there are more marbles to be sorted
		if(sorter.MoreMarbles)
		{
			sorter.State = SortState;
			sorter.SetLEDColor(Green);
			
			lcd.clear();
			lcd.home();
			lcd.print("Sorting");
			
			while((sorter.GetStartStopButtonAction() != Press) && !sorter.WDTFlag)
			{	
				//Check for 100ms sorter flag
				if(sorter.Flag)
				{
					//Clear sort flag
					sorter.Flag = false;
					
					//Complete one sort cycle
					sorter.Sort();
					
					//Print dot animation
					lcd.setCursor(7, LINE_1);
					
					switch(dots++)
					{
						case 0:
							lcd.print("   ");
							break;
						case 1:
							lcd.print(".  ");
							break;
						case 2:
							lcd.print(".. ");
							break;
						case 3:
							lcd.print("...");
							dots = 0;
							break;
					}
					
					//Update screen
					lcd.setCursor(0, LINE_3);
					sprintf(tmp, "W: %03d        B: %03d", sorter.MarbleCount.WhiteCount, sorter.MarbleCount.BlackCount);
					lcd.print(tmp);

					lcd.setCursor(0, LINE_4);	
					sprintf(tmp, "     %02d:%02d:%d00    ", sorter.MinutesElapsed, sorter.SecondsElapsed, sorter.TenthsOfSecondsElapsed);
					lcd.print(tmp);
				}
				
				else
				{
					_delay_us(10);
				}
			}
			
			//Exited due to WDT flag
			if(sorter.WDTFlag)
			{
				//Clear WDTFlag
				sorter.WDTFlag = false;
				
				//Check number of marbles sorted
				if(sorter.MarbleCount.TotalCount >= SORT_THRESHOLD)
				{
					//Sorter was able to sort 10 marbles
					sorter.SetLEDColor(Red);
				}
				else
				{
					//Sorter was not able to sort 10 marbles
					//Flash the LED Red and wait until start/stop 
					// button is pressed to acknowledge
					sorter.FlashLED = true;
					lcd.clear();
					lcd.setCursor(0, LINE_2);
					lcd.print("        ERROR       ");
					
					lcd.setCursor(0, LINE_3);
					lcd.print("PRESS S to Continue");
					
					while(sorter.GetStartStopButtonAction() != Press)
					{
						_delay_us(10);
						//wdt_reset();
					}
					
					sorter.ButtonActionCompleted();
					
					sorter.SetLEDColor(Off);
					sorter.FlashLED = false;
				}
			}
			
			//Exited due to pressing start/stop
			else
			{
				sorter.ButtonActionCompleted();
				sorter.SetLEDColor(Yellow);
			}
			
			//Return to idle state
			sorter.ButtonActionCompleted();
			sorter.State = IdleState;
			printIdleScreen = true;
		}
		
		//There are no marbles to sort
		else
		{
			lcd.setCursor(0, LINE_4);
			lcd.print("No More Marbles");
			
			sorter.SetLEDColor(Red);
			_delay_ms(200);
			sorter.SetLEDColor(Off);
			_delay_ms(200);
			sorter.SetLEDColor(Red);
			_delay_ms(200);
			sorter.SetLEDColor(Off);
			
			ClearLine(LINE_4);
		}
	}
	
	/**********************/
	/* Recall Information */
	/**********************/
	if((sorter.GetStartStopButtonAction() == Hold) && (sorter.State == IdleState))
	{
		static uint8_t min = 0;
		static uint8_t sec = 0;
		static uint8_t whiteCount = 0;
		static uint8_t blackCount = 0;
		sorter.ButtonActionCompleted();
		sorter.State = RecallState;
		
		lcd.clear();
		lcd.home();
		lcd.print("Recall Information");
		
		min = eeprom_read_byte((uint8_t *)MIN_ADDR);
		sec = eeprom_read_byte((uint8_t *)SEC_ADDR);
		whiteCount = eeprom_read_byte((uint8_t *)WHITE_COUNT_ADDR);
		blackCount = eeprom_read_byte((uint8_t *)BLACK_COUNT_ADDR);
		
		if(min == 0xFF)
		{
			min = 0;
		}
		if(sec == 0xFF)
		{
			sec = 0;
		}
		if(whiteCount == 0xFF)
		{
			whiteCount = 0;
		}
		if(blackCount == 0xFF)
		{
			blackCount = 0;
		}
		
		lcd.setCursor(0, LINE_2);
		sprintf(tmp, "Time: %02d:%02d:000", min, sec);
		lcd.print(tmp);
		
		lcd.setCursor(0, LINE_3);
		sprintf(tmp, "White Count: %03d", whiteCount);
		lcd.print(tmp);
		
		lcd.setCursor(0, LINE_4);
		sprintf(tmp, "Black Count: %03d", blackCount);
		lcd.print(tmp);
		
		//Wait for start/stop button to be held
		while(sorter.GetStartStopButtonAction() != Hold)
		{
			_delay_us(10);
			wdt_reset();	
		}
		
		sorter.ButtonActionCompleted();
		
		//Return to idle state
		sorter.State = IdleState;
		printIdleScreen = true;
	}
	
	/*********************/
	/* Reset Information */
	/*********************/
	if((sorter.GetResetButtonAction() == Press) && (sorter.State == IdleState))
	{
		sorter.ButtonActionCompleted();
		sorter.State = ResetState;
		
		ClearLine(LINE_4);
		lcd.setCursor(0, LINE_4);
		lcd.print("Reset");
		
		for(int i = 0; i < 3; i++)
		{
			_delay_ms(200);
			lcd.print(".");
		}
		
		sorter.SecondsElapsed = 0;
		sorter.MinutesElapsed = 0;
		
		sorter.MarbleCount.WhiteCount = 0;
		sorter.MarbleCount.BlackCount = 0;
		sorter.MarbleCount.TotalCount = 0;
		sorter.SetLEDColor(Off);
		eeprom_update_byte((uint8_t *)MIN_ADDR, 0);
		eeprom_update_byte((uint8_t *)SEC_ADDR, 0);
		eeprom_update_byte((uint8_t *)WHITE_COUNT_ADDR, 0);
		eeprom_update_byte((uint8_t *)BLACK_COUNT_ADDR, 0);
		
		_delay_ms(1000);
		
		//Return to idle state
		sorter.State = IdleState;
		printIdleScreen = true;
	}
	
	/**************/
	/* TEST STATE */
	/**************/
	if((sorter.GetResetButtonAction() == Hold) && (sorter.State == IdleState))
	{
		sorter.ButtonActionCompleted();
		sorter.State = TestState;
		
		lcd.clear();
		lcd.home();
		lcd.print("TEST STATE");
		
		//Wait for reset button to be held
		while(sorter.GetResetButtonAction() != Hold)
		{
			_delay_us(10);
			wdt_reset();
		}
		
		sorter.ButtonActionCompleted();
		
		//Return to idle state
		sorter.State = IdleState;
		printIdleScreen = true;
	}
}

/************************************************************************/
/* INTERRUPTS															*/
/************************************************************************/
/************************************************************************/
/* 4s Watchdog Timer													*/
/************************************************************************/
ISR(WDT_vect)
{
	//Only set the flag if in the sort state
	if(sorter.State == SortState)
	{
		//Set the flag only if there are no more marbles
		if(!sorter.MoreMarbles)
		{
			sorter.WDTFlag = true;
		}
	}
}

/************************************************************************/
/* Timer 0 Output Compare A                                             */
/************************************************************************/
ISR(TIMER0_COMPA_vect)
{
	static int count = 0;
	static int wdtCount = 0;
	static int timeCount = 0;
	static bool toggle = 0;
	
	//Increment the count
	count++;
	
	//Increment the WDT count
	wdtCount++;
	
	//Increment elapsed time count if sorting
	if(sorter.State == SortState)
	{
		timeCount++;
	}
	
	//2s set flag to mimic WDT
	if(wdtCount >= 200)
	{
		//Clear the WDT count
		wdtCount = 0;
		
		//Only set the flag if in the sort state
		if(sorter.State == SortState)
		{
			//Set the flag only if there are no more marbles
			if(!sorter.MoreMarbles)
			{
				//Set the flag
				sorter.WDTFlag = true;
			}
		}
	}
	
	//100 x 10ms for 1s delay
	if(count >= 100)
	{
		count = 0;
		sorter.Flag = true; //Set flag
	}
	
	//Keep track of seconds and minutes and flash LED if necessary
	if(timeCount >= 100)
	{
		timeCount = 0;
		
		//sorter.TenthsOfSecondsElapsed ++;
		
		sorter.SecondsElapsed++;
		
		/*
		if(sorter.TenthsOfSecondsElapsed == 10)
		{
			sorter.SecondsElapsed++;
			
			lcd.setCursor(0, LINE_2);
			lcd.print(sorter.SecondsElapsed);
			
			sorter.TenthsOfSecondsElapsed = 0;
		}
		*/
		
		if(sorter.SecondsElapsed >= 60)
		{
			sorter.MinutesElapsed++;
			sorter.SecondsElapsed = 0;
		}
		
		toggle ^= true;
		
		if(sorter.FlashLED)
		{
			if(toggle)
			{
				sorter.SetLEDColor(Red);
			}
			else
			{
				sorter.SetLEDColor(Off);
			}
		}
	}
}

/************************************************************************/
/* Timer 2 Output Compare A                                             */
/************************************************************************/
ISR(TIMER2_COMPA_vect)
{
	static int resetCount = 0;
	static int startStopCount = 0;
	static int noMoreMarblesCount = 0;
	
	//Check if marble present
	if(sorter.CheckForMoreMarbles() == WAR_NO_MARBLE)
	{
		noMoreMarblesCount++;
	}
	else
	{
		noMoreMarblesCount = 0;
	}
	
	if(noMoreMarblesCount > No_MORE_MARBLES_THRESHOLD)
	{
		sorter.MoreMarbles = false;
	}
	else
	{
		sorter.MoreMarbles = true;
	}
	
	//Check Reset Button (Active low)
	if(!(PIND & RESET_BTN))
	{
		resetCount++;
		sorter.ButtonActionReady = false;
	}
	else
	{	
		if(resetCount >= PRESS_TIME)
		{
			sorter.ButtonActionReady = true;
		}
		
		resetCount = 0;
	}
	
	ResetCount = resetCount;
	
	//Check Start/Stop Button (Active low)
	if(!(PIND & START_STOP_BTN))
	{
		startStopCount++;
		sorter.ButtonActionReady = false;
	}
	else
	{		
		if(startStopCount >= PRESS_TIME)
		{
			sorter.ButtonActionReady = true;
		}
		
		startStopCount = 0;
	}
	
	StartStopCount = startStopCount;
	
	//Handle reset button
	if(resetCount < PRESS_TIME)
	{
		//sorter.SetResetButtonAction(None);
		;
	}
	else if((resetCount >= PRESS_TIME) && (resetCount < HOLD_TIME))
	{
		sorter.SetResetButtonAction(Press);
		sorter.SetStartStopButtonAction(None);
	}
	else
	{
		resetCount = HOLD_TIME;
		sorter.SetResetButtonAction(Hold);
		sorter.SetStartStopButtonAction(None);
	}
	
	//Handle start/stop button
	if(startStopCount < PRESS_TIME)
	{
		//sorter.SetStartStopButtonAction(None);
		;
	}
	else if((startStopCount >= PRESS_TIME) && (startStopCount < HOLD_TIME))
	{
		sorter.SetStartStopButtonAction(Press);
		sorter.SetResetButtonAction(None);
	}
	else
	{
		startStopCount = HOLD_TIME;
		sorter.SetStartStopButtonAction(Hold);
		sorter.SetResetButtonAction(None);
	}
}

/************************************************************************/
/* GLOBAL FUNCTIONS														*/
/************************************************************************/
/************************************************************************/
/* Initialize Port Directions                                           */
/************************************************************************/
void InitPortDirections(void)
{
	//Clear all bits
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	
	//PORTB OUTPUTS
	DDRB |= SWITCH_S0 | SWITCH_S1 | SENSOR_EN | SERVO_PWM;
	
	//PORTD OUTPUTS
	DDRD |= SERVO_EN | LED_RED | LED_GREEN;
	
	//PORTC INPUTS
	DDRC &= ~(SENSOR_0 | SENSOR_1);
	
	//PORTD INPUTS
	DDRD &= ~(START_STOP_BTN | RESET_BTN);
	
	//Enable pull up resistors
	PORTD |= (START_STOP_BTN | RESET_BTN);
}

/************************************************************************/
/* Initialize Timers                                                    */
/************************************************************************/
void InitTimers(void)
{
	cli();
	
	//Configure Timer 0 to delay 10ms
	TCCR0A = _BV(WGM01);			//CTC Mode
	TCCR0B = _BV(CS02) | _BV(CS00); //1:1024 Prescaler
	TIMSK0 = _BV(OCIE0A);			//Enable compare interrupt
	OCR0A = CYCLES_1;				//Set OCR0A to the correct number of cycles
	
	//Configure Timer 1 for Phase-Correct PWM mode and 20ms period
	TCCR1A = _BV(COM1B1) |  _BV(WGM10) | _BV(WGM11);	//Clear PB2 on rise, set on fall
	TCCR1B = _BV(CS11) | _BV(WGM13);					//1:8 Prescaler
	
	OCR1A = PERIOD_CNT >> 1;							//Set OCR1A to Period/2
	OCR1B = (PERIOD_CNT / 20) >> 1;						//Initially set OCR1B to 1ms on time / 2
	
	//Configure Timer 2 to delay 1ms
	TCCR2A = _BV(WGM21);			//CTC Mode
	TCCR2B = _BV(CS22);				//1:64 Prescaler
	TIMSK2 = _BV(OCIE2A);			//Enable compare interrupt
	OCR2A = CYCLES_2;				//Set OCR2A to the correct number of cycles
	
	sei();
}

/************************************************************************/
/* Initialize ADC														*/
/************************************************************************/
void InitADC(void)
{
	//Configure ADC
	ADMUX = _BV(REFS0) | _BV(ADLAR); //5V Vref, Left aligned, Channel 0
	ADCSRA = _BV(ADEN) | _BV(ADSC) | _BV(ADATE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);	//Enable ADC, Start Conversion,
																						//	Auto-Start, 1:128 Pre-scaler
	ADCSRB = 0; //Free Run Mode
}

/************************************************************************/
/* Initialize EEPROM													*/
/************************************************************************/
void InitEEPROM(void)
{
	;	
}

/************************************************************************/
/* Initialize WDT														*/
/************************************************************************/
void InitWDT(void)
{
	//Disable interrupts
	cli();
	
	//Reset WDT
	wdt_reset();
	
	//Clear WDRF in MCUSR
	MCUSR &= ~_BV(WDRF);
	
	//Set WDCE and WDE to start timed sequence
	WDTCSR |= _BV(WDCE) | _BV(WDE);
	
	//Setup WDTCSR to enable interrupt mode on WDT timeout
	// and start WDT with 4s timeout period by setting bit WDP3
	WDTCSR |= _BV(WDIE) | _BV(WDP3);
	
	//Set WDCE and WDE to start another timed sequence
	WDTCSR |= _BV(WDCE) | _BV(WDE);
	
	//Clear WDE to disable reset mode
	WDTCSR &= ~_BV(WDE);
	
	//Enable global interrupts
	sei();
}

void InitSorter(void)
{
	//Enable servo power and wait for transients
	Servo::Enable();
	_delay_ms(100);
	
	//Initialize Servos to nominal
	sorter.ServoZero.SetServo(NoMarble);
	sorter.ServoOne.SetServo(NoMarble);
	
	//Set sorter state to idle
	sorter.State = IdleState;
}

/************************************************************************/
/* Initialize LCD														*/
/************************************************************************/
void InitLCD(void)
{
	//Start the LCD as a 20x4 display
	lcd.begin(LINE_LEN, NUM_LINES);
	
	lcd.backlight();
	
	lcd.clear();
	lcd.home();
	lcd.print("Gibson-Millwood");
	_delay_ms(1000);
	
	lcd.setCursor(0, LINE_2);
	lcd.print("Marble Sorter");
	_delay_ms(1000);
	
	lcd.setCursor(0, LINE_3);
	lcd.print("V1.00");
	_delay_ms(1000);
	
	LoadingBar();
	
	lcd.clear();
	lcd.home();
}

/************************************************************************/
/* Loading Bar Animation                                                */
/************************************************************************/
void LoadingBar(void)
{
	lcd.setCursor(0, LINE_4);
	
	for(int i = 0; i < LINE_LEN; i++)
	{
		lcd.write(0xFF);
		_delay_ms(100);
	}
	
	lcd.clear();
}

/************************************************************************/
/* Clear An Individual Line on the LCD                                  */
/************************************************************************/
void ClearLine(int line)
{
	lcd.setCursor(0, line);
	
	for(int i = 0; i < LINE_LEN; i++)
	{
		lcd.print(" ");
	}
	
	lcd.setCursor(0, line);
}

void PrintIdleScreen(void)
{
	lcd.clear();
	lcd.home();
	lcd.print("PRESS S -> Sort");
	
	lcd.setCursor(0, LINE_2);
	lcd.print("PRESS R -> Reset");
	
	lcd.setCursor(0, LINE_3);
	lcd.print("HOLD  S -> Recall");
}