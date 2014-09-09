/************************************************************************/
/* File: Servo.h														*/
/* Author: Joe Gibson and Jesse Millwood								*/
/* Date: 11/5/13														*/
/* Course: EGR 326														*/
/* Description:	Servo.h implements the Servo class, which contains the	*/
/*				data and methods associated with controlling servos		*/
/*																		*/
/* Grand Valley State University, 2013									*/
/************************************************************************/

#ifndef SERVO_H_
#define SERVO_H_

#include "Global.h"
#include "Marble.h"

/************************************************************************/
/* Servo Class															*/
/************************************************************************/
class Servo
{	
	/************************************************************************/
	/* Private Members														*/
	/************************************************************************/
	int Index;
	
	/************************************************************************/
	/* Private Methods														*/
	/************************************************************************/
	/************************************************************************/
	/* Set servo to an angle in degrees										*/
	/************************************************************************/
	T_ErrorCode SetServoAngle(double degrees)
	{
		//BYPASSING SWITCH
		/*
		//Select the correct line to switch servo
		if(this->Index == 0)
		{
			PORTD |= SWITCH_S0;
			PORTB &= ~SWITCH_S1;	
		}
		
		if(this->Index == 1)
		{
			PORTD &= ~SWITCH_S0;
			PORTB |= SWITCH_S1;
		}
		*/
		
		//Declare an offset variable to attempt to reach
		// max swing
		int offset = 0;
	
		//Check for angles outside of range
		if((degrees < 0) || degrees > 180)
		{
			//Only valid for 0 to 180 degrees
			return ERR_INVALID_SERVO_ANGLE;
		}
	
		//Check to see if necessary
		//Apply negative offset for 0 degrees
		if(degrees == 0)
		{
			offset = -700;
		}
    
		//Apply positive offset for 180 degrees
		if(degrees == 180)
		{
			offset = 550;
		}
	
		//Convert from  0 to 180 degrees to 1.0 to 2.0ms Ton
		OCR1B = (int)(((((degrees / 180) + 1.0) / 20.0) * PERIOD_CNT) + offset)>> 1;
		
		return ERR_NO_ERROR;
	}
		
	public :
		
	/************************************************************************/
	/* Public Methods														*/
	/************************************************************************/
	/************************************************************************/
	/* Default Constructor													*/
	/************************************************************************/
	Servo()
	{
		this->Index = 0;	
	}
	
	/************************************************************************/
	/* Constructor with Index												*/
	/************************************************************************/
	Servo(int index)
	{
		this->Index = index;
	}
	
	/************************************************************************/
	/* Default Destructor													*/
	/************************************************************************/
	~Servo()
	{
			
	}
	
	/************************************************************************/
	/* Get index															*/
	/************************************************************************/
	int GetIndex(void)
	{
		return this->Index;
	}
	
	/************************************************************************/
	/* Set index															*/
	/************************************************************************/
	void SetIndex(int index)
	{
		this->Index = index;
	}
	
	/************************************************************************/
	/* Enable servo power													*/
	/************************************************************************/
	static void Enable(void)
	{
		PORTD |= SERVO_EN;
	}
	
	/************************************************************************/
	/* Disable servo power													*/
	/************************************************************************/
	static void Disable(void)
	{
		PORTD &= ~SERVO_EN;	
	}
	
	/************************************************************************/
	/* Set servo based on the marble type sensed							*/
	/************************************************************************/
	T_ErrorCode SetServo(T_MarbleType marbleType)
	{
		if(marbleType == White)
		{
			if(this->Index == 0)
			{
				this->SetServoAngle(180);
			}
			else
			{
				this->SetServoAngle(0);
			}
		}
		
		else if(marbleType == Black)
		{
			if(this->Index == 0)
			{
				this->SetServoAngle(0);
			}
			else
			{
				this->SetServoAngle(180);
			}
		}
		
		else
		{
			//Set servo to 90 degrees
			this->SetServoAngle(90);
		}
		
		return ERR_NO_ERROR;
	}
};



#endif /* SERVO_H_ */