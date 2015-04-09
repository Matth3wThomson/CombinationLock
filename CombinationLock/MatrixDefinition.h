template<typename T>
Matrix<T>::Matrix(const int xSize, const int ySize):
	x(xSize), y(ySize){
		mat = new T*[x];
		for (int i=0; i<x; ++i){
			mat[i] = new T[y];
			memset(mat[i], 0, (y * sizeof(T)));
		}
}

template<typename T>
Matrix<T>::Matrix(const Matrix<T>& src):
	x(src.x), y(src.y)
{
	mat = new T*[x];
	for(int i = 0; i < x; ++i)	
	{
		mat[i] = new T[y];
		memcpy_s(mat[i], (y * sizeof(T)), src.mat[i], (y * sizeof(T)));
	}
}

template<typename T>
Matrix<T>::Matrix(Matrix&& rval):
mat(rval.mat), x(rval.x), y(rval.y){
	//We must nullify the other object, as the move constructor is called
	//when the original object is uninitialized
	rval.mat = 0;
	rval.x = 0;
	rval.y = 0;
}

template<typename T>
Matrix<T>::~Matrix(){
	for (int i = 0; i<x; i++){
		delete[] mat[i];
	}
	delete[] mat;
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& rhs){
	if (this == &rhs) return (*this);

	//Release Old memory
	for (int i=0; i < x; ++i){
		delete[] mat[i];
	}
	delete[] mat;

	x=rhs.x;
	y=rhs.y;

	//Allocate new memory
	mat = new T*[x];
	for (int i=0; i< x; ++i){
		mat[i] = new T[y];
		memset(mat[i], 0, (y * sizeof(T)));
	}

	//Copy Values
	for (int i=0; i<rhs.x; ++i){
		for (int j=0; j<rhs.y; ++j){
			mat[i][j] = rhs.mat[i][j];
		}
	}

	return *this;
}

template<typename T>
Matrix<T>& Matrix<T>::operator=(Matrix<T>&& rval){
	std::swap(mat, rval.mat);
	std::swap(x, rval.x);
	std::swap(y, rval.y);
	return *this;
}


template<typename T>
Matrix<T> Matrix<T>::transpose(){
	Matrix<T> trans(y, x);
	
	for (int i=0; i<x; ++i)
		for (int j=0; j<y; ++j)
			trans[j][i] = mat[i][j];

	return trans;
}

//T ARITHMETIC
template<typename E>
Matrix<E> operator*(const Matrix<E>& lhs, const E& rhs){
	Matrix<E> result(lhs);

	for (int i=0; i<result.x; ++i)
		for (int j=0; j<result.y; ++j)
			result[i][j]*=rhs;

	return result;
}

template<typename E>
Matrix<E> operator*(const E& lhs, const Matrix<E>& rhs){
	return rhs * lhs;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(const T& rhs){
	return *this = rhs * *this;
}

//MATRIX ARITHMETIC

template<typename E>
Matrix<E> operator*(const Matrix<E>& lhs, const Matrix<E>& rhs){

	if (lhs.getYDim() != rhs.getXDim()) throw std::invalid_argument("MatrixMatrix multiply requires number of rows equal to columns");

	Matrix<E> result(lhs.x, rhs.y);

	for (int i=0; i< lhs.x; ++i)
		for (int j=0; j < rhs.y; ++j)
			for (int k=0; k<lhs.y; ++k)
				result.mat[i][j] += lhs.mat[i][k] * rhs.mat[k][j];

	return result;
}

template<typename T>
Matrix<T>& Matrix<T>::operator*=(const Matrix<T>& rhs){
	return *this = *this * rhs;
}

template<typename E>
Matrix<E> operator+(const Matrix<E>& lhs, const Matrix<E>& rhs){
	if (lhs.x != rhs.x || lhs.y != rhs.y) throw std::invalid_argument("Matrix addition requires matrices of same size");

	Matrix<E> temp(lhs);

	for (int i=0; i<lhs.getXDim(); ++i)
		for (int j=0; j<lhs.getYDim(); ++j)
			temp[i][j] += rhs[i][j];
	
	return temp;
}

template<typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix<T>& rhs){
	return *this = *this + rhs;
}

template<typename E>
Matrix<E> operator-(const Matrix<E>& lhs, const Matrix<E>& rhs){
	if (lhs.x != rhs.x || lhs.y != rhs.y) throw std::invalid_argument("Matrix subtraction requires matrices of same size");

	Matrix<E> temp(lhs);

	for (int i=0; i<lhs.getXDim(); ++i)
		for (int j=0; j<lhs.getYDim(); j++)
			temp[i][j] -= rhs[i][j];

	return temp;
}
template<typename T>
Matrix<T>& Matrix<T>::operator-=(const Matrix<T>& rhs){
	return *this = *this - rhs;
}

//VECTOR ARITHMETIC

//template<typename E>
//Vector<E> Matrix<E>::operator*(const Vector<E>& rhs){
//	if (this->y != rhs.size()) throw std::invalid_argument("Matrix/Vector multiplication requires same number of matrix columns to vector size");
//
//	Vector<E> temp(this->x);
//
//	for (int i=0; i<this->x; ++i)
//		for (int j=0; j<this->y; ++j)
//			temp[i] += (*this)[i][j] * rhs[j];
//
//	return temp;
//}

//DISPLAY
template<typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& m){
	for (int i=0; i<m.x; ++i){
			for (int j=0; j<m.y; ++j) os << std::right << std::setw(4) <<  m[i][j];
			os << std::endl;
		}
	return os;
}