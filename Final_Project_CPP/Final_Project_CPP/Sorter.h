/************************************************************************/
/* File: Sorter.h														*/
/* Author: Joe Gibson and Jesse Millwood								*/
/* Date: 11/5/13														*/
/* Course: EGR 326														*/
/* Description: Sorter.h implements the Sorter class, which contains	*/
/*																		*/
/*																		*/
/* Grand Valley State University, 2013									*/
/************************************************************************/

#ifndef SORTER_H_
#define SORTER_H_

#include <util/delay.h>
#include <avr/eeprom.h>
#include <string.h>
#include "Global.h"
#include "Marble.h"
#include "Servo.h"

/************************************************************************/
/* Enumerations and Structures											*/
/************************************************************************/
//State enumeration
typedef enum T_State
{
	IdleState,
	SortState,
	RecallState,
	ResetState,
	TestState
}T_State;

//Button Action enumeration
typedef enum T_ButtonAction
{
	None = 0,
	Press = 1,
	Hold = 2
}T_ButtonAction;

//Marble Count structure
typedef struct T_MarbleCount
{
	int BlackCount;
	int WhiteCount;
	int TotalCount;
	
	//Constructor
	T_MarbleCount()
	{
		this->BlackCount = 0;
		this->WhiteCount = 0;
		this->TotalCount = 0;
	}
	
	//Destructor
	~T_MarbleCount()
	{
		
	}
	
}T_MarbleCount;

/************************************************************************/
/* Sorter Class															*/
/************************************************************************/
class Sorter
{
	/************************************************************************/
	/* Private Methods														*/
	/************************************************************************/
	/************************************************************************/
	/* Set the ADC to a channel												*/
	/************************************************************************/
	T_ErrorCode SelectADCChannel(int channel)
	{
		//Set corresponding channel bits
		ADMUX |= _BV(REFS0) | _BV(ADLAR) | channel;
		
		return ERR_NO_ERROR;
	}
	
	/************************************************************************/
	/* Check the sensor on the given channel								*/
	/************************************************************************/
	T_ErrorCode CheckSensorOnChannel(int channel, Marble &marble)
	{
		//Select the channel
		SelectADCChannel(channel);
		
		//Check for White Marble
		if(ADCH <= WHITE_THRESHOLD)
		{
			//Set MarbleType
			marble.SetMarbleType(White);
			
			return ERR_NO_ERROR;
		}
		
		//Check for Black Marble
		else if((ADCH >= WHITE_THRESHOLD) && (ADCH <= BLACK_THRESHOLD))
		{
			//Set MarbleType
			marble.SetMarbleType(Black);
			
			return ERR_NO_ERROR;
		}
		
		//No Marble
		else
		{
			//Set MarbleType
			marble.SetMarbleType(NoMarble);
			
			return WAR_NO_MARBLE;
		}
	}
	
	/************************************************************************/
	/* Update the MarbleCount structure										*/
	/************************************************************************/
	void UpdateCount(T_MarbleType marbleType)
	{	
		if(marbleType == Black)
		{
			this->MarbleCount.BlackCount++;
			this->MarbleCount.TotalCount++;
			
			//Write to EEPROM
			eeprom_update_byte((uint8_t *)BLACK_COUNT_ADDR, (uint8_t)(this->MarbleCount.BlackCount));
		}
		
		if(marbleType == White)
		{
			this->MarbleCount.WhiteCount++;
			this->MarbleCount.TotalCount++;
			
			//Write to EEPROM
			eeprom_update_byte((uint8_t *)WHITE_COUNT_ADDR, (uint8_t)(this->MarbleCount.WhiteCount));
		}
		
		//Write time to EEPROM
		eeprom_update_byte((uint8_t *)MIN_ADDR, (uint8_t)(this->MinutesElapsed));
		eeprom_update_byte((uint8_t *)SEC_ADDR, (uint8_t)(this->SecondsElapsed));
	}
	
	public :
	
	/************************************************************************/
	/* Public Members														*/
	/************************************************************************/
	bool MoreMarbles;						//Flag for whether there are more marbles to sort
		
	bool Flag;								//General flag
	
	bool WDTFlag;							//Watchdog Timer flag
	
	bool FlashLED;							//Flash Red LED flag
	
	bool ButtonActionReady;					//Flag for whether a button action is ready to be processed
	
	T_ButtonAction ResetButtonActon;		//Reset button action
	T_ButtonAction StartStopButtonAction;	//Start/Stop button action
	
	T_State State;							//Sorter state: Idle, Sort, Recall, Reset
		
	T_ErrorCode Error;						//Error code
	
	T_MarbleCount MarbleCount;				//Count for number of black, white, and total marbles sorted
	
	Marble MarbleZero;						//Marble at position zero
	Marble MarbleOne;						//Marble at position one
	Marble NullMarble;						//Null marble
		
	Servo ServoZero;						//Servo at position zero
	Servo ServoOne;							//Servo at position one
		
	int MinutesElapsed;						//Minutes elapsed
	int SecondsElapsed;						//Seconds elapsed
	int TenthsOfSecondsElapsed;				//Tenths of seconds elapsed
	
	/************************************************************************/
	/* Public Methods														*/
	/************************************************************************/
	/************************************************************************/
	/* Default Constructor													*/
	/************************************************************************/
	Sorter()
	{
		//Initialize sorter members
		this->MoreMarbles = true;
		this->Flag = false;
		this->WDTFlag = false;
		this->FlashLED = false;
		this->State = IdleState;
		this->Error = ERR_NO_ERROR;
		this->MarbleCount.BlackCount = 0;
		this->MarbleCount.WhiteCount = 0;
		this->MarbleCount.TotalCount = 0;
		this->MinutesElapsed = 0;
		this->SecondsElapsed = 0;
		this->TenthsOfSecondsElapsed = 0;
		
		this->MarbleZero.SetIndex(0);
		this->MarbleOne.SetIndex(1);
		this->NullMarble.SetIndex(-1);
		this->ServoZero.SetIndex(0);
		this->ServoOne.SetIndex(1);
		this->ServoZero.SetServo(NoMarble);
		this->ServoOne.SetServo(NoMarble);
	}
	
	/************************************************************************/
	/* Default Destructor													*/
	/************************************************************************/
	~Sorter()
	{
		/* */
	}
		
	/************************************************************************/
	/* Set RGB LED color													*/
	/************************************************************************/
	void SetLEDColor(T_Color color)
	{
		//Clear the LED color
		PORTD &= ~(LED_RED | LED_GREEN);
		
		//Set the LED color if necessary
		if(color != Off)
		{
			PORTD |= color;
		}
	}
	
	/************************************************************************/
	/* Perform one sorting cycle											*/
	/************************************************************************/
	T_ErrorCode Sort(void)
	{
		/**********************************/
		/* DEBUG: ONLY SORTING ONE MARBLE */
		/**********************************/
		//Declare error codes
		T_ErrorCode errorCodeChannelZero = ERR_NO_ERROR;
		//T_ErrorCode errorCodeChannelOne = ERR_NO_ERROR;
			
		//Check sensor 0
		errorCodeChannelZero = CheckSensorOnChannel(CHANNEL_0, MarbleZero);
			
		//Check sensor 1
		//errorCodeChannelOne = CheckSensorOnChannel(CHANNEL_1, MarbleOne);
		
		//Update counts
		UpdateCount(MarbleZero.GetMarbleType());
		//UpdateCount(MarbleOne.GetMarbleType());
		
		//IF SORTING ONE MARBLE
		//Discern if channel detected no marble
		if(errorCodeChannelZero == WAR_NO_MARBLE)
		{
			//this->MoreMarbles = false;
			return WAR_NO_MARBLE;
		}
		
		//Marble was detected
		//this->MoreMarbles = true;
		
		//Set servo to sort marble based on type
		ServoZero.SetServo(MarbleZero.GetMarbleType());
		
		//Delay and return to nominal
		_delay_ms(500);
		ServoZero.SetServo(NoMarble);
		
		//Disable servo power
		//Servo::Disable();
		
		//IF SORTING TWO MARBLES
		//Discern if channel one or both channels detected no marble
		/*
		if(errorCodeChannelOne == WAR_NO_MARBLE)
		{
			//One more marble to sort on channel zero
			if(errorCodeChannelZero != WAR_NO_MARBLE)
			{
				Servo::Enable();
				_delay_ms(10);
				
				ServoZero.SetServo(MarbleZero.GetMarbleType());
				
				_delay_ms(10);
				Servo::Disable();
				
				this->MoreMarbles = false;
				return WAR_NO_MARBLE;
			}
			
			//No more marbles to sort
			else
			{
				this->MoreMarbles = false;
				return WAR_NO_MARBLE;	
			}
			
		}
		
		//Enable servo power supply
		Servo::Enable();
		_delay_ms(10);
		
		ServoZero.SetServo(MarbleZero.GetMarbleType());
		ServoOne.SetServo(MarbleOne.GetMarbleType());
		
		//Disable servo power supply
		_delay_ms(10);
		Servo::Disable();
		*/
		
		return ERR_NO_ERROR;
	}
	
	/************************************************************************/
	/* Check to see if there are any more marbles to sort					*/
	/************************************************************************/
	T_ErrorCode CheckForMoreMarbles(void)
	{
		return CheckSensorOnChannel(CHANNEL_0, NullMarble);
	}
	
	/************************************************************************/
	/* A button action has been completed									*/
	/************************************************************************/
	void ButtonActionCompleted(void)
	{
		this->StartStopButtonAction = None;
		this->ResetButtonActon = None;
		this->ButtonActionReady = true;
	}
	
	/************************************************************************/
	/* Set reset button action 												*/
	/************************************************************************/
	void SetResetButtonAction(T_ButtonAction action)
	{
		this->ResetButtonActon = action;
	}
	
	/************************************************************************/
	/* Get reset button action												*/
	/************************************************************************/
	T_ButtonAction GetResetButtonAction(void)
	{
		if(!(this->ButtonActionReady))
		{
			return None;	
		}
		
		return this->ResetButtonActon;
	}
	
	/************************************************************************/
	/* Set start/stop button action											*/
	/************************************************************************/
	void SetStartStopButtonAction(T_ButtonAction action)
	{
		this->StartStopButtonAction = action;
	}
	
	/************************************************************************/
	/* Get start/stop button action											*/
	/************************************************************************/
	T_ButtonAction GetStartStopButtonAction(void)
	{
		if(!(this->ButtonActionReady))
		{
			return None;
		}
		
		return this->StartStopButtonAction;
	}
};


#endif /* SORTER_H_ */