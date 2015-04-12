
//Basic includes
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
#include <string>
#include <stdio.h>

//Helper class includes
#include "Matrix.h"
#include "NumberSet.h"

//Self-made job completer
#include "JobCompleter.h"

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

	-For each word that is in the dictionary push to a words found vector (we want duplicates).

	-Output results to the screen

*/

/*
	TASKS:
		-Revisit error handling... it is a console application!
		-Make threading work
		-Neaten up code
		-Explain all the uses for everything!
*/

/* Essentially main for this task, but wrapped for brevity */
void SolveCombinationLock(const char* wheelFile, const char* dictionaryFile);

/* Parses all information from supplied file into set passed by reference */
bool ParseDictionary(const char* filename, std::set<std::string>& dictionary);

/* Parses all information from supplied file into matrix of MxN */
Matrix<char> ParseWheel(const char* filename);


/* Searches through a wheel through every possible combination to find any words in the 
dictionary. Intended for sequential purposes. */
void FindAllWords(const std::set<std::string>& dictionary,
	const Matrix<char>& wheel,
	std::vector<std::string>& output);

/* Searches through a wheel, from a given start index, comparing words to dictionary.
	Allows for threading */
void FindWordsFunction(int index,
	const std::set<std::string>& dictionary,
	const Matrix<char>& wheel,
	std::vector<std::string>& output);

//TODO: Remove
void testNumberSet();

/* Parses the output file and couts all words that were not found in program output */
void Validator(const char* filename, std::vector<std::string>& programOutput);

int main(int argc, char* argv[]){

	if (!argv[1] || !argv[2]){
		std::cerr << "Please pass the arguments in the format programname wheelsfile dictionaryfile";
		return -1;
	}

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
	std::vector<std::string> foundWords;

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
	//FindWordsFunction(0, dictionary, wheel, foundWords);

	//Just try and find all words at the last index of each wheel
	//FindWordsFunction(9, dictionary, wheel, foundWords);

	//Sequential test
	/*for (int i = 0; i < noLetters; ++i){
		std::cout << "Starting from letter " << i << std::endl;
		FindWordsFunction(i, dictionary, wheel, foundWords);
	}*/

	FindAllWords(dictionary, wheel, foundWords);
	

	//THREADING START
	//Create jobCompleter with as many threads as is optimal for our solution
	//JobCompleter jobCompleter(noLetters);

	////Pass all the jobs which are based on a given start index to the 
	////job completer
	//for (int i = 0; i < noLetters; ++i){
	//	jobCompleter.GiveJob(
	//		[i, &dictionary, &wheel, &foundWords]{ 
	//		FindWordsFunction(i, dictionary, wheel, foundWords);
	//	}
	//	);
	//}

	//jobCompleter.CloseOnceComplete();
	//THREADING END

	Validator("output.txt", foundWords);

	//Found words now contains all words found by the find words function
	/*int foundWordsCount = 0;
	for (auto itr = foundWords.begin(); itr != foundWords.end(); ++itr){
		std::cout << (*itr) << std::endl;
		++foundWordsCount;
	}
	std::cout << "Found " << foundWords.size() << " words" << std::endl;
	std::cout << "Found " << foundWordsCount << " words" << std::endl;*/

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

	std::ifstream file(filename);

	//file.open(filename);

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

void FindWordsFunction(int index,
	const std::set<std::string>& dictionary,
	const Matrix<char>& wheel,
	std::vector<std::string>& output){

	//First discover what we need
	const int& noWheels = wheel.getXDim();
	const int& noLetters = wheel.getYDim();

	//Make sure we aren't out of bounds.
	if (index >= noLetters){
		std::cerr << "Error: Requested index larger than matrix" << std::endl;
		throw std::exception("Error: Requested index larger than matrix");
	}

	//For each wheel we will start at
	for (int i = 0; i < noWheels - 1; ++i){

		std::cout << "Thread " << std::this_thread::get_id() << " testing from wheel " << i << std::endl;

		//For all possible lengths of words we can look for given the wheel
		//we are starting at
		for (int j = 2; j <= noWheels - i; ++j){

			std::cout << "Thread " << std::this_thread::get_id() << " testing words of length " << j << std::endl;

			//Store the word we are going to search for, which is of size j
			//char* const word = new char[j];
			std::string word = std::string(j, ' ');

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
			if (dictionary.find(word) != dictionary.end()) output.push_back(word);

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
				if (dictionary.find(word) != dictionary.end()) output.push_back(word);

			//Until we have tried all combinations
			} while (!possibleCombination.isLargest());
		}
	}


}

void FindAllWords(const std::set<std::string>& dictionary,
	const Matrix<char>& wheel,
	std::vector<std::string>& output){

	//First discover what we need
	const int& noWheels = wheel.getXDim();
	const int& noLetters = wheel.getYDim();

	//For each wheel we will start at
	for (int i = 0; i < noWheels - 1; ++i){

		std::cout << "Thread " << std::this_thread::get_id() << " testing from wheel " << i << std::endl;

		//For all possible lengths of words we can look for given the wheel
		//we are starting at
		for (int j = 2; j <= noWheels - i; ++j){

			std::cout << "Thread " << std::this_thread::get_id() << " testing words of length " << j << std::endl;

			//Store the word we are going to search for, which is of size j
			//char* const word = new char[j];
			std::string word = std::string(j, ' ');

			//We now want to try every combination of the wheels left, with the word length
			//we want. So we need a number set of size j-1, with base noLetters
			NumberSet possibleCombination(noLetters, j);

			//Get the next word by retrieving the indexes we want
			for (int letter = 0; letter < j; ++letter){

				// -The letter we want is in the the wheel at i + letter we are looking at.
				// -The letter we are going to choose is based on the possible combination we
				//	are trying next. This is in the combination at letter - 1 (it is of size j - 1 )
				word[letter] = wheel.getElement(i + letter, possibleCombination.getDigit(letter));
			}

			//And check if its a word
			if (dictionary.find(word) != dictionary.end()) output.push_back(word);

			do {
				//Try the next combination
				++possibleCombination;

				//Make our words change their letters appropriately
				for (int letter = 0; letter < j; ++letter){

					// -The letter we want is in the the wheel at i + letter we are looking at.
					// -The letter we are going to choose is based on the possible combination we
					//	are trying next
					word[letter] = wheel.getElement(i + letter, possibleCombination.getDigit(letter));
				}

				//And check if the word exists in the dictionary
				if (dictionary.find(word) != dictionary.end()) output.push_back(word);

				//Until we have tried all combinations
			} while (!possibleCombination.isLargest());
		}
	}


}

void Validator(const char* filename, std::vector<std::string>& programOutput){

	std::ifstream file;

	file.open(filename);

	if (file.is_open()){

		std::set<std::string> outputFile;

		while (!file.eof()){
			std::string temp;
			file >> temp;

			if (temp == "Found") 
				break;

			std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
			outputFile.insert(temp);
		}

		int wordsDifferent = 0;

		std::cout << "Output file contains: " << outputFile.size() << std::endl;
		std::cout << "Program's output contains: " << programOutput.size() << std::endl;
		
		for (auto itr = programOutput.begin(); itr != programOutput.end(); ++itr){
			if (outputFile.find(*itr) == outputFile.end())
				std::cout << *itr << " not in output file. " << std::endl;
		}

		for (auto itr = outputFile.begin(); itr != outputFile.end(); ++itr){

			bool found = false;

			for (auto itr2 = programOutput.begin(); itr2 != programOutput.end(); ++itr2){
				if ((*itr2) == (*itr)){ found = true;  break; }
			}

			if (!found){
				++wordsDifferent;
				std::cout << (*itr) << " not present in program output. " << std::endl;
			}
		}

		std::cout << "Missing words: " << wordsDifferent << std::endl;
	}
}