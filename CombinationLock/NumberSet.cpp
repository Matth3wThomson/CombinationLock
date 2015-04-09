#include "NumberSet.h"
#include <assert.h>
#include <string> //memset is here...

//In the cpp to prevent infecting other files that include
typedef unsigned int uint; 

NumberSet::NumberSet(const uint base, const uint digits){
	//You cant have a number system of lower than base 2
	assert(base > 1);
	this->base = base;

	//There must be at least 2 digits!
	assert(digits > 1);
	this->digits = digits;
	numberArr = new int[digits];

	//Initialize to zero.
	memset(numberArr, 0, digits * sizeof(int));
}

NumberSet::NumberSet(const NumberSet& rhs){
	this->base = rhs.base;
	this->digits = rhs.digits;

	//Perform deep copy
	numberArr = new int[digits];
	for (uint i = 0; i < digits; ++i){
		numberArr[i] = rhs.numberArr[i];
	}
}

NumberSet::~NumberSet()
{
	delete numberArr;
}

int NumberSet::getDigit(const uint digit){

	assert(digit < digits);

	return numberArr[digit];
}

//Prefix
NumberSet& NumberSet::operator++(){
	Increment();
	return *this;
}

//Postfix
NumberSet NumberSet::operator++(int dummy){
	NumberSet copy(*this);
	Increment();
	return copy;
}

void NumberSet::Increment(){

	//Loop from the lowest significant number to the highest significant number
	for (int i = digits - 1; i >= 0; --i){

		//If any digit is lower than the maximum base number
		if (numberArr[i] < (int) base - 1){

			//We can increment the current number
			++numberArr[i];

			//And then reset the rest of the numbers to zero
			for (uint j = i+1; j < digits; ++j){
				numberArr[j] = 0;
			}

			//The number set has been incremented
			break;
		}
	}

	//If we reach here then the number cannot be incremented higher than it currently
	//is. So we will just loop back around, setting all digits to zero.
	for (uint i = 0; i < digits; ++i){
		numberArr[i] = 0;
	}
}

bool NumberSet::isLargest(){

	//Loop through from lowest significant number to the highest significant number
	for (int i = digits - 1; i >= 0; --i){

		//If any digit is lower than the maximum base number, then it's clearly
		//not the largest number possible!
		if (numberArr[i] < (int) base - 1) return false;
	}

	//Every number in the array as at the maximum base number!
	return true;
}

void NumberSet::becomeLargest(){
	
	for (uint i = 0; i < digits; ++i){
		numberArr[i] = base - 1;
	}
}