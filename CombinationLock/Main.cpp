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
#include <mutex>

//0 Non-threaded
//1 Threaded
#define THREADED 1

/*
	File: CombinationLockSolver:Main.cpp
	Author: Matthew Thomson 2015

	Notes:
		-Can choose between threaded or non threaded implementation (as above)
			-JobCompleter automates all threading using a job queue system. Reusable.
		-Uses a NumberSet (similar to bitset, with customizable base) to enumerate all
		 possible combinations.
		-Set used for dictionary storage as binary search for words increases efficiency.
			-Could have reserved space (to prevent multiple reallocations), but chose not to
			as cannot assume amount of memory available, or accidentally overestimate on small
			cases.
		-The specification states that "there is no need to remove duplicate words and the
		 order of the output word list does not matter." Therefore the program outputs
		 all words that it finds, including duplicates, and the number of words found, 
		 even though the supplied output file has duplicates removed.
*/

/* Essentially main for this task, but wrapped for brevity */
void SolveCombinationLock(const char* wheelFile, 
	const char* dictionaryFile,
	std::vector<std::string>& output);

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
void FindWordsFunction(unsigned int index,
	const std::set<std::string>& dictionary,
	const Matrix<char>& wheel,
	std::vector<std::string>& output,
	std::mutex& outputListMutex);

int main(int argc, char* argv[]){

	if (argc < 2) std::cerr << "Please pass the arguments in the format programName WheelsFilePath DictionaryFilePath"; 

	if (!argv[1] || !argv[2]){
		std::cerr << "Please pass the arguments in the format programName WheelsFilePath DictionaryFilePath";
		return -1;
	}

	std::vector<std::string> output;

	try {
		SolveCombinationLock(argv[1], argv[2], output);
	}
	catch (std::exception e){
		std::cerr << e.what() << std::endl;
		return -2;
	}

	//Now to output all words found by the SolveCombinationLock method
	for (auto itr = output.begin(); itr != output.end(); ++itr){
	std::cout << argv[0] << ">" << (*itr) << std::endl;
	}
	std::cout << argv[0] << ">Found " << output.size() << " words";

	system("pause");

	return 0;
}

void SolveCombinationLock(const char* wheelFile, const char* dictionaryFile, std::vector<std::string>& foundWords){

	std::set<std::string> dictionary;
	//std::vector<std::string> foundWords;

	//Load in the wheel and dictionary
	Matrix<char> wheel = ParseWheel(wheelFile);
	ParseDictionary(dictionaryFile, dictionary);

	//Discover their dimensions
	int noWheels, noLetters;
	noWheels = wheel.getHeight();
	noLetters = wheel.getWidth();
	
#if THREADED
	//Create jobCompleter with as many threads as is optimal for our solution
	JobCompleter jobCompleter(noLetters);

	std::mutex outputListMutex;

	//Pass all the jobs which are based on a given start index to the 
	//job completer. This will run the tasks at a thread density as requested
	//in the constructor.
	for (int i = 0; i < noLetters; ++i){
		jobCompleter.GiveJob(
			[i, &dictionary, &wheel, &foundWords, &outputListMutex]{ 
			FindWordsFunction(i, dictionary, wheel, foundWords, outputListMutex);
		}
		);
	}

	//Wait for job completer to finish its jobs.
	jobCompleter.CloseOnceComplete();

#else
	//Perform the search sequentially.
	FindAllWords(dictionary, wheel, foundWords);
#endif

}

bool ParseDictionary(const char* filename, std::set<std::string>& dictionary){
	std::ifstream file;

	file.open(filename);

	if (file.is_open()){

		while (!file.eof()){

			//Load in the string
			std::string temp(" ");
			file >> temp;
			
			if (temp == " ") throw std::exception("Error loading dictionary file");

			//Ensure that the word is indeed in lower case, and insert into dictionary
			std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
			dictionary.insert(temp);
		}

	}
	else {
		throw std::exception("Error loading dictionary file");
	}

	file.close();

	return true;
}

Matrix<char> ParseWheel(const char* filename){

	std::ifstream file(filename);

	unsigned int noWheels = 0, noLetters = 0;

	if (file.is_open()){
		file >> noWheels; //N
		file >> noLetters; //M

		if (noWheels < 1 || noLetters < 1) throw std::exception("Bad wheel file. Number of wheels or number of letters not defined correctly.");

		//This format was chosen to better aid cache efficiency
		Matrix<char> wheel(noWheels, noLetters);

		for (unsigned int i = 0; i < noWheels; ++i){

			int x = i;

			for (unsigned int j = 0; j < noLetters; ++j){
				char temp = ' ';
				file >> temp;
				
				if (temp == ' ') throw std::exception("Bad wheel file. Doesn't match specification");
				wheel.setElement(i, j, tolower(temp));
			}

		}

		file.close();

		return wheel;
	}
	else {
		throw std::exception("Cannot open wheel file");
	}
}

void FindWordsFunction(unsigned int index,
	const std::set<std::string>& dictionary,
	const Matrix<char>& wheel,
	std::vector<std::string>& output,
	std::mutex& outputListMutex){

	//First discover what we need
	const int& noWheels = wheel.getXDim();
	const int& noLetters = wheel.getYDim();

	//Make sure we aren't out of bounds.
	if (index >= (unsigned int)noLetters){
		std::cerr << "Error: Requested index larger than matrix" << std::endl;
		throw std::exception("Error: Requested index larger than matrix");
	}

	//For each wheel we will start at
	for (int i = 0; i < noWheels - 1; ++i){

		//For all possible lengths of words we can look for given the wheel
		//we are starting at
		for (int j = 2; j <= noWheels - i; ++j){

			//Store the word we are going to search for, which is of size j
			std::string word = std::string(j, ' ');

			//Initialize the first element to the value of the wheel we are currently
			//starting at, and the index we are currently using as our start index
			word[0] = wheel.getElement(i, index);
			
			//We now want to try every combination of the wheels left, with the word length
			//we want. So we need a number set of size j-1, with base noLetters
			NumberSet possibleCombination(noLetters, j - 1);
			bool finished = false;

			do {

				//Make our words change their letters appropriately
				for (int letter = 1; letter < j; ++letter){

					// -The letter we want is in the the wheel at i + letter we are looking at.
					// -The letter we are going to choose is based on the possible combination we
					//	are trying next
					word[letter] = wheel.getElement(i + letter, possibleCombination.getDigit(letter - 1));
				}

				//And check if the word exists in the dictionary
				if (dictionary.find(word) != dictionary.end()){
					std::lock_guard<std::mutex> lock(outputListMutex);
					output.push_back(word);
				}

				//Prepare for next combination
				if (possibleCombination.isLargest())
					finished = true;

				++possibleCombination;

			//Until we have tried all combinations
			} while (!finished);
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

		//For all possible lengths of words we can look for given the wheel
		//we are starting at
		for (int j = 2; j <= noWheels - i; ++j){

			//Store the word we are going to search for, which is of size j
			std::string word = std::string(j, ' ');

			//We now want to try every combination of the wheels left, with the word length
			//we want. So we need a number set of size j-1, with base noLetters
			NumberSet possibleCombination(noLetters, j);
			bool finished = false;

			do {

				//Make our words change their letters appropriately
				for (int letter = 0; letter < j; ++letter){

					// -The letter we want is in the the wheel at i + letter we are looking at.
					// -The letter we are going to choose is based on the possible combination we
					//	are trying next
					word[letter] = wheel.getElement(i + letter, possibleCombination.getDigit(letter));
				}

				//And check if the word exists in the dictionary
				if (dictionary.find(word) != dictionary.end()) output.push_back(word);

				//Prepare for the next combination
				if (possibleCombination.isLargest()) finished = true;
				++possibleCombination;

				//Until we have tried all combinations
			} while (!finished);
		}
	}
}
