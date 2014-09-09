/************************************************************************/
/* File: Sorter.h														*/
/* Author: Joe Gibson and Jesse Millwood								*/
/* Date: 11/5/13														*/
/* Course: EGR 326														*/
/* Description: Sorter.h implements the Sorter class, which contains	*/
/*				
/*																		*/
/* Grand Valley State University, 2013									*/
/************************************************************************/

#ifndef SORTER_H_
#define SORTER_H_

#include "Global.h"
#include "Marble.h"
#include "Servo.h"

class Sorter
{
	public :
	
    //Public Members
    bool Sorting;					//Flag: Whether sorter is currently sorting
    
    T_ErrorCode Error;				//Error code

    T_MarbleCount MarbleCount;		//Count for number of black, white, and total marbles sorted

    Marble MarbleZero;				//Marble at position zero
    Marble MarbleOne;				//Marble at position one
    
    Servo ServoZero;				//Servo at position zero
    Servo ServoOne;					//Servo at position one

    //Public Methods
    Sorter()	//Default constructor
    {
        //Initialize sorter members
        Sorting = false;
        ErrorCode = ERR_NO_ERROR;
        MarbleCount.BlackCount = 0;
        MarbleCount.WhiteCount = 0;
        MarbleCount.TotalCount = 0;
        MarbleZero.MarbleType = NoMarble;
        MarbleOne.MarbleType = NoMarble;
    }

    ~Sorter()	//Default destructor
    {
        /* */
    }
    
    T_ErrorCode Sort(void)
    {
        T_ErrorCode s0err, s1err;
        
        //Set Servo zero to nominal
        ServoZero.SetServoToAngle(0);
        
        //Set Servo one to nominal
        ServoOne.SetServoToAngle(0);
        
        //Sense the marble at position zero
        MarbleZero.SenseMarble();
        
        //Sense the marble at position one
        MarbleOne.SenseMarble();
        
        //Sort the marbles
        s0err = ServoZero.SetServo(MarbleZero.GetMarbleType());
        s1err = ServoOne.SetServo(MarbleOne.GetMarbleType());
        
        if(s0err == ERR_NO_ERROR)
        {
            MarbleCount.Count++;
            
            if(MarbleZero.GetMarbleType() == Marble.White)
            {
                MarbleCount.WhiteCount++;
            }
            else
            {
                MarbleCount.BlackCount++;
            }
        }
        
        if(s1err == ERR_NO_ERROR)
        {
            if(MarbleZero.GetMarbleType() == Marble.White)
            {
                MarbleCount.WhiteCount++;
            }
            else
            {
                MarbleCount.BlackCount++;
            }
        }
    }
};


#endif /* SORTER_H_ */