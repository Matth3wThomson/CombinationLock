
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
#include <string>
#include <stdio.h>
//#include <ctype.h>
#include "Matrix.h"
#include "NumberSet.h"

/*
	INFO:
	M Letters per wheel
	N Wheels

	argv[1] = Wheel info (Each char is upper case)
	argv[2] = Dictionary (Each char is lower case)

	PLANS:
	-Parse the entire dictionary into a set (ordered, and avoids duplicates... quicker automatic searching)

	-Parse the wheel into a matrix of N x M dimensions.
	M Letters per wheel
	N Wheels

	-For 0 to M, create threads (up to the maximum allowed by the CPU), and search from their starting index
	through each wheel (0 - M), for (0 - N) wheels.

	-For each word that is in the dictionary push to a words found set.

	-Output results to the screen

*/

/*
	TASKS:
		-Revisit error handling... it is a console application!
*/

/* Essentially main for this task, but wrapped for brevity */
void SolveCombinationLock(const char* wheelFile, const char* dictionaryFile);

/* Parses all information from supplied file into set passed by reference */
bool ParseDictionary(const char* filename, std::set<std::string>& dictionary);

/* Parses all information from supplied file into matrix of MxN */
Matrix<char> ParseWheel(const char* filename);

/* Searches through a wheel, from a given start index, comparing words to dictionary */
void FindWordsFunction(int index,
	const std::set<std::string>& dictionary,
	const Matrix<char>& wheel,
	std::set<std::string>& output);

void testNumberSet();

int main(int argc, char* argv[]){

	SolveCombinationLock(argv[1], argv[2]);
	//testNumberSet();


	return 0;
}

void testNumberSet(){

	int base = 10, digits = 10;

	NumberSet test(base, digits);

	long long int maxNumber = (long long int) pow(digits, base);

	test.becomeLargest();

	for (int j = 0; j < digits; ++j){
		std::cout << test.getDigit(j) << " ";
	}
	std::cout << std::endl;

	std::cout << test.isLargest() << std::endl;

	//for (long long int i = 0; i < maxNumber; ++i){
	//	
	//	//Test increment
	//	++test;

	//	for (int j = 0; j < digits; ++j){
	//		std::cout << test.getDigit(j) << " ";
	//	}
	//	std::cout << std::endl;
	//}

	system("pause");
}

void SolveCombinationLock(const char* wheelFile, const char* dictionaryFile){

	std::set<std::string> dictionary;
	std::set<std::string> foundWords;

	//Load in the wheel and dictionary
	Matrix<char> wheel = ParseWheel(wheelFile);
	ParseDictionary(dictionaryFile, dictionary);

	//Discover their dimensions
	int noWheels, noLetters;
	noWheels = wheel.getHeight();
	noLetters = wheel.getWidth();

	

	//DEBUGGING
	//Print dictionary
	/*std::cout << "Dictionary: " << std::endl;
	for (auto itr = dictionary.begin(); itr != dictionary.end(); ++itr){
	std::cout << (*itr) << std::endl;
	}
	std::cout << "Total words: " << dictionary.size() << std::endl;*/

	//Print wheel
	std::cout << "Wheel: " << std::endl;
	std::cout << "Number of Wheels: " << noWheels << std::endl;
	std::cout << "Number of letters per wheel: " << noLetters << std::endl;
	std::cout << wheel << std::endl;
	//END DEBUGGING

	//Then for every number of letters call the find words function. 
	//TODO: Put threading in here... why have 1 thread process all starting indexes!
	/*for (int i = 0; i < noLetters; ++i){
	FindWordsFunction(i, dictionary, wheel, foundWords);
	}*/

	//Just try and find all words at the first index of each wheel
	FindWordsFunction(0, dictionary, wheel, foundWords);

	//Found words now contains all words found by the find words function
	for (auto itr = foundWords.begin(); itr != foundWords.end(); ++itr){
		std::cout << (*itr) << std::endl;
	}
	std::cout << "Found " << foundWords.size() << " words" << std::endl;

	system("pause");
}

//TODO: Error checking
bool ParseDictionary(const char* filename, std::set<std::string>& dictionary){
	std::ifstream file;

	file.open(filename);

	if (file.is_open()){

		while (!file.eof()){

			//Load in the string
			std::string temp;
			file >> temp;
			
			//Ensure that the word is indeed in lower case, and insert into dictionary
			std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
			dictionary.insert(temp);
		}

	}

	file.close();

	return true;
}

Matrix<char> ParseWheel(const char* filename){

	std::ifstream file;

	file.open(filename);

	int noWheels, noLetters;

	if (file.is_open()){
		file >> noWheels; //N
		file >> noLetters; //M

		//This format was chosen to better aid cache efficiency
		Matrix<char> wheel(noWheels, noLetters);

		for (int i = 0; i < noWheels; ++i){
			for (int j = 0; j < noLetters; ++j){
				char temp;
				file >> temp;
				wheel.setElement(i, j, tolower(temp));
			}
		}

		file.close();

		return wheel;
	}
	else {
		std::cerr << "Wheel file was not loaded correctly!" << std::endl;
		throw std::exception("File was not loaded correctly!");
	}
}

//FIRST ATTEMPT
//void FindWordsFunction(int index,
//	const std::set<std::string>& dictionary,
//	const Matrix<char>& wheel,
//	std::set<std::string>& output){
//
//	//First discover what we need
//	const int& noWheels = wheel.getYDim();
//	const int& noLetters = wheel.getXDim();
//
//	//Make sure we aren't out of bounds.
//	if (index >= noWheels){
//		std::cerr << "Error: Requested index larger than matrix" << std::endl;
//		throw std::exception("Error: Requested index larger than matrix");
//	}
//
//	//For each wheel we will start at
//	for (int i = 0; i < noWheels - 2; ++i){
//
//		//For all possible lengths of words we can look for given the wheel
//		//we are starting at
//		for (int j = 2; j <= noWheels - i; ++j){
//
//			//Store the word we are going to search for, initializing our first character
//			//to the value of the index we have been delegated of each wheel we will test
//			char* word = new char[j];
//			word[0] = wheel.getElement(i, index);
//
//			//Loop through the wheels and initialize them all, other than
//			//the start one to index 0.
//			for (int k = i + 1; k < noWheels; ++k){
//				word[k] = wheel.getElement(k, 0);
//			}
//
//			//Then loop through each wheel once for every wheel
//			//there is
//			for (int k = i + 1; k < noWheels; ++k){
//
//				for (int l = k; l < noWheels; ++l){
//					for (int m = 0; m < noLetters; ++m){
//						word[k] = wheel.getElement(l, m);
//
//						//And check if its a word
//						std::string test(word);
//						if (dictionary.find(test) != dictionary.end()) output.insert(test);
//					}
//				}
//
//			}
//
//		}
//	}
//
//
//}

//SECOND ATTEMPT USING NUMBERSET IMPLMENTATION
void FindWordsFunction(int index,
	const std::set<std::string>& dictionary,
	const Matrix<char>& wheel,
	std::set<std::string>& output){

	//First discover what we need
	const int& noWheels = wheel.getXDim();
	const int& noLetters = wheel.getYDim();

	//Make sure we aren't out of bounds.
	if (index >= noWheels){
		std::cerr << "Error: Requested index larger than matrix" << std::endl;
		throw std::exception("Error: Requested index larger than matrix");
	}

	//For each wheel we will start at
	for (int i = 0; i < noWheels - 2; ++i){

		//For all possible lengths of words we can look for given the wheel
		//we are starting at
		for (int j = 2; j <= noWheels - i; ++j){

			//Store the word we are going to search for, which is of size j
			char* const word = new char[j];

			//Initialize the first element to the value of the wheel we are currently
			//starting at, and the index we are currently using as our start index
			word[0] = wheel.getElement(i, index);
			
			//We now want to try every combination of the wheels left, with the word length
			//we want. So we need a number set of size j-1, with base noLetters
			NumberSet possibleCombination(noLetters, j - 1);
			
			//Get the next word by retrieving the indexes we want
			for (int letter = 1; letter < j; ++letter){

				// -The letter we want is in the the wheel at i + letter we are looking at.
				// -The letter we are going to choose is based on the possible combination we
				//	are trying next. This is in the combination at letter - 1 (it is of size j - 1 )
				word[letter] = wheel.getElement(i+letter, possibleCombination.getDigit(letter - 1));
			}

			//And check if its a word
			std::string test(word);
			if (dictionary.find(test) != dictionary.end()) output.insert(test);

			do {
				//Try the next combination
				++possibleCombination;

				//Make our words change their letters appropriately
				for (int letter = 1; letter < j; ++letter){

					// -The letter we want is in the the wheel at i + letter we are looking at.
					// -The letter we are going to choose is based on the possible combination we
					//	are trying next
					word[letter] = wheel.getElement(i + letter, possibleCombination.getDigit(letter - 1));
				}

				//And check if the word exists in the dictionary
				std::string test(word);
				std::cout << "Testing: " << test << std::endl;
				if (dictionary.find(test) != dictionary.end()) output.insert(test);

			//Until we have tried all combinations
			} while (!possibleCombination.isLargest());

			//We no longer need our word
			delete word;
		}
	}


}