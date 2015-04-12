#pragma once

#include <ostream>

/**
	Class: NumberSet
	Author: Matthew Thomson 2015

	The customizable version of a bitset. Ie, a supplied number-set.
	
	The idea of this class is to basically have a number that has
	a supplied base. For example, binary is base 2, decimal is base 10,
	this will represent a number of a supplied base, and a supplied number
	of digits.

	Notes: With base being an integer, this theoretically allows for numbers up
			to base 2^32. This would allow you to represent absolutely huge numbers,
			and is not necessary for the purposes of this task, and could be optimized
			by using a short. However, in the interest of reusability this optimization
			has been ignored.
*/

//TODO: numberArr can be unsigned too.
class NumberSet
{
	//Properties
private:

	int* numberArr;
	unsigned int base, digits;

	unsigned long long int biggestNumber;
	unsigned long long int baseTen;

public:
	NumberSet(const unsigned int base, const unsigned int digits);
	NumberSet(const NumberSet& rhs);
	~NumberSet();

	//Gets the value of the number of a given digit
	int getDigit(const unsigned int digit);

	NumberSet& operator++(); //Prefix
	NumberSet operator++(int dummy); //Postfix

	/* Returns a boolean to determine whether the numberset is currently
	   the largest number it is possible to be 
	*/
	bool isLargest() const ;

	/* Causes the number to become the largest possible number it can represent */
	void becomeLargest();

	/* Returns the highest number this numberSet can hold in base 10 */
	long long int getPossibleLargest() const;

	friend std::ostream& operator<<(std::ostream& stream, const NumberSet& numberSet);

private:
	//No default construction
	NumberSet();
	void Increment();
	bool bIsLargest;
};

