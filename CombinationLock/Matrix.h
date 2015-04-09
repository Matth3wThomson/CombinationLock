#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include <stdexcept>
//#include "Vector.h"

#ifndef NULL
#define NULL 0
#endif

/*
NOTES:
		-Performance of exceptions that arent thrown or caught identical to functions that dont throw,
		 therefore exceptions used on functions where matrices must be of identical size, as program
		 crash is desired. (BENCHMARK: 10000 valid add operations)
		-Division of matrices not done since it is technically the multiplication of the inverse,
		 and the inverse of a matrix isn't easy to compute!
		-There are functions with duplicate purposes, this is simply to make the class easier for a third
		 party to understand its implementation
		-Square brackets and at function implemented without safeguards purposefully, to make the class feel
		 like a pass through/native matrix
		-Move semantics have been implemented for this class to increase performance in creating copies.
*/




template<typename T>
class Matrix {

public:
	//Default Values
	static const int defaultX = 3;
	static const int defaultY = 3;

	//Construction
	Matrix(const int xSize = defaultX, const int ySize = defaultY);
	Matrix(const Matrix<T>& src); 
	Matrix(Matrix<T>&& rval); //<-- Move constructor
	~Matrix(void);
	Matrix<T>& operator=(const Matrix<T>& rhs);
	Matrix<T>& operator=(Matrix<T>&& rval); //<-- Move assignment operator

	//Arithmetic
	Matrix<T> transpose();

	//T arithmetic
	template<typename E>
	friend Matrix<E> operator*(const Matrix<E>& lhs, const E& rhs);
	template<typename E>
	friend Matrix<E> operator*(const E& lhs, const Matrix<E>& rhs);
	Matrix<T>& operator*=(const T& rhs);


	//Matrix arithmetic
	template<typename E>
	friend Matrix<E> operator*(const Matrix<E>& lhs, const Matrix<E>& rhs);
	Matrix<T>& operator*=(const Matrix<T>& rhs);
	
	template<typename E>
	friend Matrix<E> operator+(const Matrix<E>& lhs, const Matrix<E>& rhs);
	Matrix<T>& operator+=(const Matrix<T>& rhs);

	template<typename E>
	friend Matrix<E> operator-(const Matrix<E>& lhs, const Matrix<E>& rhs);
	Matrix<T>& operator-=(const Matrix<T>& rhs);

	//Vector arithmetic
	//Vector<T> operator*(const Vector<T>& rhs);

	//Insertion
	inline void setElement(const int x, const int y, const T& elem){ mat[x][y] = elem; };

	//Access
	inline T getElement(const int x, const int y) const { return mat[x][y];  }
	inline T& getElement(const int x, const int y){ return mat[x][y]; };
	inline T* operator[](const int location) const{ return mat[location]; };

	//Size operations
	inline int getXDim() const{ return x; };
	inline int getYDim() const{ return y; };

	inline int getHeight() const{ return x; };
	inline int getWidth() const { return y; };

	//Display
	template<typename E>
	friend std::ostream& operator<<(std::ostream& os, const Matrix<E>& m);

private:
	int x;
	int y;
	T** mat;
};

#include "MatrixDefinition.h"