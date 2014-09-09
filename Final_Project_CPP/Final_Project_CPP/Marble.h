/************************************************************************/
/* File: Marble.h														*/
/* Author: Joe Gibson and Jesse Millwood								*/
/* Date: 11/5/13														*/
/* Course: EGR 326														*/
/* Description: Marble.h implements the Marble class, which contains	*/
/*				the marble type data									*/
/*																		*/
/* Grand Valley State University, 2013									*/
/************************************************************************/

#ifndef MARBLE_H_
#define MARBLE_H_

#include "Global.h"

/************************************************************************/
/* Enumerations and Structures											*/
/************************************************************************/
//Marble Type structure
typedef enum T_MarbleType
{
	Black,
	White,
	NoMarble
}T_MarbleType;

/************************************************************************/
/* Marble Class															*/
/************************************************************************/
class Marble
{	
	/************************************************************************/
	/* Private Members														*/
	/************************************************************************/
	int Index;				//Index of marble: Marbles are indexed either 0 or 1, where
							//	an index of 0 refers to the marble at the very end
							//	of the series of marbles
	
	T_MarbleType MarbleType;	//Type of marble: black marble, white marble, or no marble
	
	public :
	
	/************************************************************************/
	/* Public Methods														*/
	/************************************************************************/
	/************************************************************************/
	/* Default Constructor													*/
	/************************************************************************/
	Marble()
	{
		Index = 0;
		MarbleType = NoMarble;
	}
	
	/************************************************************************/
	/* Constructor to set Index												*/
	/************************************************************************/
	Marble(int index)
	{
		Index = index;
		MarbleType = NoMarble;
	}
	
	/************************************************************************/
	/* Default Destructor													*/
	/************************************************************************/
	~Marble()	//Default destructor
	{
		/* */
	}
	
	/************************************************************************/
	/* Get marble type														*/
	/************************************************************************/
	T_MarbleType GetMarbleType(void)
	{
		return MarbleType;
	}
	
	/************************************************************************/
	/* Set marble type														*/
	/************************************************************************/
	void SetMarbleType(T_MarbleType marbleType)
	{
		MarbleType = marbleType;
	}
	
	/************************************************************************/
	/* Get index															*/
	/************************************************************************/
	int GetIndex(void)
	{
		return Index;
	}
	
	/************************************************************************/
	/* Set index															*/
	/************************************************************************/
	void SetIndex(int index)
	{
		Index = index;
	}
};

#endif /* MARBLE_H_ */