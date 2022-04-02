/**
 * @file ValueVectorFunctions.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-15
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef ValueVectorFunctions_h
#define ValueVectorFunctions_h

#include <vector>
#include <random>
#include <algorithm> //rotate
#include <typeinfo> //type_info

#include "simulation/Value.hpp"

 /*
  * A static container for Value-containing vector manipulation.
  *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
  */
template <class _valueType>
class ValueVectorFunction {
public:
	/*
	 * Return true if two vectors of Values "mismatch" (two values are known and not equal).
	 *
	 * An exception will be thrown if the two vectors ar not the same size.
	 *
	 * @param a The first vector to compare.
	 * @param b The second vector to compare.
	 * @param (optional) _compare A vector corresponding to the values to compare (false = do not compare).
	 *        If not given, all values will be compared.
	 * @return True if the two values (with unkowns) cannot be equal.
	 */
	static bool mismatch(
		const std::vector<_valueType> a,
		const std::vector<_valueType> b,
		std::vector<bool> _compare = std::vector<bool>()
	) {
		if (a.size() != b.size()) {
			throw "Cannot compare two value vectors of different sizes.";
		}
		if (_compare.empty() == true) {
			_compare = std::vector<bool>(a.size(), true);
		} else if (_compare.size() != a.size()) {
			throw "Cannot compare two value vectors: comparison vector does not match the given vector sizes.";
		}
		for (unsigned int i = 0; i < a.size(); i++) {
			if (_compare.at(i) == false) {
				continue;
			}
			_valueType aValue = a.at(i);
			_valueType bValue = b.at(i);
			if ((aValue != bValue) && (aValue.valid() & bValue.valid())) { //The "valid" part is needed for the mismatch:
				//... we don't care if they "!=" if they are not valid.
				return true;
			}
			//DELETE: this is now obsolete with non-bool values.
			/*if (a.at(i).valid() == true && b.at(i).valid() == true) {
				if (a.at(i).magnitude() != b.at(i).magnitude()) {
					return true;
				}
			}*/
		}
		return false;
	}

	/*
	 * "Backtrack" a vector of values. Return false if it can no longer be back-tracked.
	 *
	 * In this context, "backtrack" means (attempt to) skip to the next possible
	 * entry in a binary progression without incrementing.
	 *
	 * @param val Entry to backtrack.
	 * @return False if the entry cannot be backtracked.
	 */
	static bool backtrack(std::vector<_valueType> & val) {
		int index = -1;

		//First, find the largest index which has a valid value.
		for (size_t i = 0; i < val.size(); i++) {
			if (val.at(i).valid()) {
				index = i;
			} else {
				break;
			}
		}

		//Then work backwards trying to flip the value.
		while (index >= 0) {
			if (val.at(index).increment() == false) {
				return true;
			}
			val.at(index).valid(false);
			index--;
		}
		return false;
	}

	/*
	 * "Back trace" a vector. Return false if it cannot be done.
	 * 
	 * In this context, "Back trace" is not increment. Instead, move the vector
	 * to the next place in a binary tree progression.
	 *
	 * @param val Entry to forward track.
	 * @return False if the entry cannot be forward trawked (i.e., the vector is full). 
	 */
	static bool backtrace(std::vector<_valueType> & val/*, std::vector<_valueType> & memVal*/) {
		int index = -1;
		for (size_t i = 0; i < val.size(); i++) {
			if (val.at(i).valid() == false) {
				index = i; break;
			}
		}


		if (index < 0) {
			return false;
		} //All elements fillled, no forward track can be done.
		val.at(index).valid(true); val.at(index).magnitude(val.at(index).min());
		return true;
	}


	/*
	 * Increment the vector while holding a given value.
	 *
	 * An exception may be thrown if a non-valid Value is present.
	 *
	 * @param _vec The vector to increment.
	 * @param _hold The value to hold (i.e., skip). If not given, no Values will be skipped.
	 * @return False if the vector "rolled over".
	 */
	static bool increment(std::vector <_valueType> & _vec, int _hold = -1) {
		std::vector<_valueType> constants = incrementingConstants(_vec.size());

		for (std::size_t i = 0; i < _vec.size(); i++) {
			if (i == _hold) { continue;	}
			if (i < constants.size()) {
				_vec[i] = constants.at(i);
				continue;
			}
			//Beyond constant values, valid values will be not considered. It will also
			// presumed that all values beyond the "constants" are at their appropriate
			// values.
			if (_vec.at(i).flip()) { //It's (all) 1 after flipping: return.
				return true;
			}
		}
		return false;
	}

	/*
	 * Create a random vector with a given (optional) set of weights.
	 *
	 * If no weight set is given, "even weights" (i.e., pure random) will be returned.
	 *
	 * If the size given does not equal the number of weights, throw an error.
	 *
	 * @param _size The size of the vector to return.
	 * @param (optional) _weights The weight assigned to each input, by default, "50%".
	 * @return The generated random vector.
	 */
	static std::vector<_valueType> random(size_t _size, std::vector<float> _weights = std::vector<float>());

	/*
	 * Shift a vector left.
	 *
	 * Vectors will be shifted towards 0, i.e., the LSB will be destroyed
	 * (returned) and the MSB will become the given value.
	 *
	 * @param (reference) _vector The vector to shift left.
	 * @param _input The new value to shift in as the new MSB.
	 * @return The previously LSB of the vector.
	 */
	static _valueType shift(std::vector<_valueType> & _vector, _valueType _input) {
		std::rotate(_vector.begin(), _vector.begin() + 1, _vector.end());
		_valueType toReturn = _vector.back(); _vector.pop_back();
		_vector.push_back(_input);
		return toReturn;
	}

private:
	/*
	 * Return a vector of constants which should be used when incrementing.
	 *
	 * @param _size The number of input to be constant (needed for "valid bits").
	 * @return The constants to for incrementing.
	 */
	static std::vector<_valueType> incrementingConstants(size_t _size) {
		_size = (_size > 7) ? 6 : _size;
		unsigned long long int validsULLI[7] = {
			0x0000000000000001, //0 input (which doesn't make sense)
			0x0000000000000003, //1 input : 01
			0x000000000000000F, //2 inputs : 0101
			0x00000000000000FF, //3
			0x000000000000FFFF, //4
			0x00000000FFFFFFFF, //5
			0xFFFFFFFFFFFFFFFF  //6 (and more)
		};
		const std::type_info& type = typeid(_valueType().magnitude()); //Get the "_primitive" of the _valueType.
		std::vector<_valueType> toReturn;
		if (typeid(bool) == type || _size == 0) {
			return toReturn; //For bool, there are no "constants".
		} else if (typeid(unsigned long long int) == type) {
			//For unsigned long long int (64 bits), the fist 6 values are of a particular type.
			toReturn = {
				_valueType(0xAAAAAAAAAAAAAAAA, validsULLI[_size]), //1010
				_valueType(0xCCCCCCCCCCCCCCCC, validsULLI[_size]), //1100
				_valueType(0xF0F0F0F0F0F0F0F0, validsULLI[_size]), //00001111
				_valueType(0xFF00FF00FF00FF00, validsULLI[_size]), //0000000011111111
				_valueType(0xFFFF0000FFFF0000, validsULLI[_size]), //00000000000000001111111111111111
				_valueType(0xFFFFFFFF00000000, validsULLI[_size])  //...
			};
			return toReturn;
		} else {
			throw "ValueVectorFunction ERROR: I don't know how to handle this data type.";
		}
	}

};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////


template<>
inline std::vector<Value<bool>> ValueVectorFunction<Value<bool>>::random(size_t _size, std::vector<float> _weights) {
	if (_weights.size() == 0) {
		_weights = std::vector<float>(_size, 0.5);
	}
	if (_size != _weights.size()) {
		throw "Cannot generate random vector: incorrect number of weights given";
	}
	std::vector<Value<bool>> toReturn;
	static std::default_random_engine e;
	for (size_t a = 0; a < _weights.size(); a++) {
		std::bernoulli_distribution b(_weights.at(a));
		toReturn.push_back(Value<bool>(b(e)));
	}
	return toReturn;
}

//template<>
//inline std::vector<FaultyValue<bool>> ValueVectorFunction<FaultyValue<bool>>::random(size_t _size, std::vector<float> _weights) {
//	if (_weights.size() == 0) {
//		_weights = std::vector<float>(_size, 0.5);
//	}
//	if (_size != _weights.size()) {
//		throw "Cannot generate random vector: incorrect number of weights given";
//	}
//	std::vector<FaultyValue<bool>> toReturn;
//	static std::default_random_engine e;
//	for (size_t a = 0; a < _weights.size(); a++) {
//		std::bernoulli_distribution b(_weights.at(a));
//		toReturn.push_back(FaultyValue<bool>(b(e)));
//	}
//	return toReturn;
//}

//NOTE: This declaration presumes "unsigned long long int". It will work with smaller (non-bool)
//      declarations, but it will be inefficient.
template<class _valueType>
inline std::vector<_valueType> ValueVectorFunction<_valueType>::random(size_t _size, std::vector<float> _weights) {
	if (_weights.size() == 0) {
		_weights = std::vector<float>(_size, 0.5);
	}
	if (_size != _weights.size()) {
		throw "Cannot generate random vector: incorrect number of weights given";
	}
	std::vector<_valueType> toReturn;
	static std::default_random_engine e;
	for (size_t a = 0; a < _weights.size(); a++) {
		//std::bernoulli_distribution b(_weights.at(a));
		//unsigned long long int num = rand(); //Only 16 bits, because, why not make things confusing.
		//num = num << 16; num = num | rand();
		//num = num << 16; num = num | rand();
	 //   num = num << 16; num = num | rand();
		//toReturn.push_back(_valueType(num));
		static std::default_random_engine e;		
		
			unsigned long long int num = 0x0000000000000000;
			
			std::bernoulli_distribution b(_weights.at(a));
			for (size_t i = 0; i < 64; i++)
			{
				unsigned long long int weight_rand = (b(e));
				//printf("%llu", weight_rand);
				num = num | weight_rand;
				num = num << 1;
			}
			toReturn.push_back(_valueType(num));
		
		
	}
	return toReturn;
}

#endif