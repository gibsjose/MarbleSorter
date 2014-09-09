/************************************************************************/
/* File: Global.h														*/
/* Author: Joe Gibson and Jesse Millwood								*/
/* Date: 11/5/13														*/
/* Course: EGR 326														*/
/* Description:	Global.h contains the definitions, structures, and		*/
/*				other data types used throughout the project			*/
/*																		*/
/* Grand Valley State University, 2013									*/
/************************************************************************/

#ifndef GLOBAL_H_
#define GLOBAL_H_

//General Definitions
#define PRESS_TIME		100				//Button press time in ms
#define HOLD_TIME		700				//Button hold time in ms
#define CYCLES_1		156				//Number of cycles at 1:1024 prescale for 10ms delay on Timer 0
#define CYCLES_2		250				//Number of cycles at 1:64 prescale for 1ms delay on Timer 2
#define SORT_THRESHOLD	10				//Number of marbles to be considered a successful sort
#define No_MORE_MARBLES_THRESHOLD 80	//Number of ms to wait when checking for more marbles

//EEPROM Addresses
#define MIN_ADDR			0x00	//Address for minutes
#define SEC_ADDR			0x01	//Address for seconds
#define BLACK_COUNT_ADDR	0x02	//Address for black count
#define WHITE_COUNT_ADDR	0x03	//Address for white count

//Pin Definitions
//OUTPUTS
#define LED_RED				_BV(3)				//Red LED on PD3						(Digital Pin 3)
#define LED_GREEN			_BV(2)				//Green LED on PD2						(Digital Pin 2)
#define LED_YELLOW			LED_RED | LED_GREEN	//Yellow LED (Red and Green)		

#define LCD_SDA				_BV(4)				//SDA on PC4							(Analog Pin 4)
#define LCD_SCL				_BV(5)				//SCL on PC5							(Analog Pin 5)

#define SERVO_EN			_BV(5)				//Servo Power Supply Enable on PD5		(Digital Pin 5)
#define SERVO_PWM			_BV(2)				//Servo PWM on PB2 (OC1B)				(Digital Pin 10)
#define SWITCH_S0			_BV(4)				//Switch Select 0 on PD4				(Digital Pin 4)
#define SWITCH_S1			_BV(2)				//Switch Select 1 on PB2				(Digital Pin 10)

#define SENSOR_EN			_BV(0)				//Sensor Enable on PB0					(Digital Pin 8)

//INPUTS
#define START_STOP_BTN		_BV(6)				//Start/Stop Button on PD6				(Digital Pin 6)
#define RESET_BTN			_BV(7)				//Reset Button on PD7					(Digital Pin 7)

#define SENSOR_0			_BV(0)				//Sensor 0 on PC0						(Analog Pin 0)
#define SENSOR_1			_BV(1)				//Sensor 1 on PC1						(Analog Pin 1)
	
//LCD Definitions
#define I2C_ADDRESS 0x27		//LCD I2C Address
#define EN          2			//Enable Pin
#define RW          1			//Read/Write Pin
#define RS          0			//Register Select Pin
#define D4          4			//Data [4..7]
#define D5          5
#define D6          6
#define D7          7
#define BL          3			//Backlight Pin
#define BL_POL      POSITIVE	//Backlight Polarity

#define LINE_LEN   20			//Length of line
#define NUM_LINES   4			//Number of lines
#define LINE_1      0			//Indexing of lines 1 to line 4
#define LINE_2      1
#define LINE_3      2
#define LINE_4      3

//Sorter Definitions
#define WHITE_THRESHOLD 8			//Threshold for a WHITE marble
#define BLACK_THRESHOLD 20			//Threshold for a BLACK marble

//Servo Definitions
#define PERIOD_CNT 40000		//Period cycle count for 20ms servo PWM period
#define SERVO_0 0				//Servo 0
#define SERVO_1 1				//Servo 1 

//ADC Definitions
#define CHANNEL_0 0				//ADC Channel 0 (Sensor 0) on PC0
#define CHANNEL_1 1				//ADC Channel 1 (Sensor 1) on PC1 

//Warning/Error Code Definitions
#define WAR_NO_MARBLE			-1				//No marble found. Not necessarily an error

#define ERR_NO_ERROR			0				//No error
#define ERR_WDT_TIMEOUT			-200			//Watchdog timer has timed out; at this point
												//	the total marble count should be checked
#define ERR_INVALID_SERVO_ANGLE -201			//The servo angle was not between 0 and 180 degrees

//Error Code Types
typedef int T_ErrorCode;		//Typedef for error code type

//Structures and Enumerations
typedef enum T_Color
{
	Red = LED_RED,
	Green = LED_GREEN,
	Yellow = LED_YELLOW,
	Off
}T_Color;

//Public Function Prototypes for main.cpp
void LoadingBar(void);
void ClearLine(int line);
void InitPortDirections(void);
void InitTimers(void);
void InitWDT(void);
void InitADC(void);
void InitEEPROM(void);
void InitSorter(void);
void InitLCD(void);
void PrintIdleScreen(void);

#endif /* GLOBAL_H_ */