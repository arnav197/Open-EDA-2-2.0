/**
 * @file Function.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2018-12-25
 *
 * @copyright Copyright (c) 2018
 *
 */

#ifndef Function_h
#define Function_h

#include <string>
#include <vector>

#include <algorithm> //std::transform
#include <cctype> //std::tolower

#include "simulation/Value.hpp"
#include "Connection.h"

template <class T>
class Value;

/*
 * A representation of a output-generating function.
 *
 * A function takes in a vector of inputs and translates them into an output.
 * 
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class Function {
public:
	/*
	 * Create a Function with an unknown name.
	 */
	Function() {
		this->name_ = "UNKNOWN";
	}

	/*
	 * Create the Function with a given name.
	 *
	 * @param _name The name of the Function.
	 */
	Function(std::string _name) {
		std::transform(_name.begin(), _name.end(), _name.begin(), (int(*)(int))std::tolower);
		this->name_ = _name;
	}

	/*
	 * "Clone constructor".
	 *
	 * @return A copy of this Function object (as a function).
	 */
	virtual Function<_valueType>* clone() const = 0;

	/*
	 * For a given input vector, return an output.
	 *
	 * @param _vector The input vector of Values to evaluate.
	 * @return The output corresponding to the input vector.
	 */
	virtual _valueType evaluate(std::vector<_valueType> _vector) const = 0;

	/*
	 * The function as a string.
	 *
	 * The function will be retruned as a string "e.g., 'and'".
	 *
	 * All returned values will be lower case.
	 *
	 * @return The Function name.
	 */
	std::string string() const {
		return this->name_;
	}

protected:
	/*
	 * The name of the Function
	 */
	std::string name_;
};

/*
 * A Boolean function.
 *
 * This class implements a Boolean function which is evaluated algorithmically.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class BooleanFunction : public Function<_valueType> {
public:
	/*
	 * Will throw an exception: there is no "undefined" Boolean function.
	 */
	BooleanFunction();


	/*
	 * Create a boolean function given a string (i.e., AND, OR, etc).
	 *
	 * The given string will determine the Boolean function implemented.
	 *
	 * @param String which determines the Boolean function.
	 */
	BooleanFunction(std::string _functionString);

	/*
	 * Copy constructor.
	 *
	 * @param _obj The function to copy.
	 */
	BooleanFunction(const BooleanFunction & _obj);

	/*
	 * "Clone constructor".
	 *
	 * @return A copy of this Function object (as a function).
	 */
	virtual Function<_valueType>* clone() const;

	/*
	 * For a given input vector, return an output.
	 *
	 * @param _vector The input vector of Values to evaluate.
	 * @return The output corresponding to the input vector.
	 */
	virtual _valueType evaluate(std::vector<_valueType> _vector) const;

private:

	/*
	 * The following are the separate (possible) primitive Booelan functions.
	 */
	_valueType AND(std::vector<_valueType>) const;
	_valueType OR(std::vector<_valueType>) const;
	_valueType NAND(std::vector<_valueType>) const;
	_valueType NOR(std::vector<_valueType>) const;
	_valueType XOR(std::vector<_valueType>) const;
	_valueType XNOR(std::vector<_valueType>) const;
	_valueType BUF(std::vector<_valueType>) const;
	_valueType NOT(std::vector<_valueType>) const;

	/*
	 * This function pointer holds the Boolean function implemented by this function.
	 */
	_valueType(BooleanFunction::*function_)(std::vector<_valueType>) const;

};

/*
 * A constant function: the same Value will always be returned.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class ConstantFunction : public Function<_valueType> {
public:
	/*
	 * Will throw an exception: there is no "undefined" Constant function.
	 */
	ConstantFunction() {
		throw "A constant function must have a value defined.";
	}


	/*
	 * Create a ConstantFunction with a given constant value.

	 * @param _value The Value to always return.
	 */
	ConstantFunction(_valueType _value) {
		this->value_ = _value;
		if (this->value_.valid()) {
			if (this->value_.magnitude()) {
				this->name_ = "const_1";
			} else {
				this->name_ = "const_0";
			}
		} else {
			this->name_ = "const_X";
		}
	}

	/*
	 * Copy constructor.
	 *
	 * @param _obj The function to copy.
	 */
	ConstantFunction(const ConstantFunction & _obj) {
		this->value_ = _obj.evaluate(std::vector<_valueType>());
		if (this->value_.valid()) {
			if (this->value_.magnitude()) {
				this->name_ = "const_1";
			} else {
				this->name_ = "const_0";
			}
		} else {
			this->name_ = "const_X";
		}
	}

	/*
	 * "Clone constructor".
	 *
	 * @return A copy of this Function object (as a function).
	 */
	virtual Function<_valueType>* clone() const {
		return new ConstantFunction<_valueType>(*this);
	}

	/*
	 * For a given input vector, return an output.
	 *
	 * @param _vector The input vector of Values to evaluate.
	 * @return The output corresponding to the input vector.
	 */
	virtual _valueType evaluate(std::vector<_valueType> _vector) const {
		return this->value_;
	}

private:
	/*
	 * The value to always return.
	 */
	_valueType value_ ;

};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////

template<class _valueType>
inline BooleanFunction<_valueType>::BooleanFunction() : Function<_valueType>("invalid") {
	throw "Invalid: there is no 'default' Boolean function.";
}

template<class _width>
inline BooleanFunction<_width>::BooleanFunction(std::string _functionString) : Function<_width>(_functionString) {
	function_ = &BooleanFunction::BUF; //Overwritten, so it does not matter.  This prevents "uninitialized member" analysis.
	std::transform(_functionString.begin(), _functionString.end(), _functionString.begin(), (int(*)(int))std::tolower);
	if (_functionString == "and") {
		function_ = &BooleanFunction::AND;
	} else if (_functionString == "nand") {
		function_ = &BooleanFunction::NAND;
	} else if (_functionString == "or") {
		function_ = &BooleanFunction::OR;
	} else if (_functionString == "nor") {
		function_ = &BooleanFunction::NOR;
	} else if (_functionString == "xor") {
		function_ = &BooleanFunction::XOR;
	} else if (_functionString == "xnor") {
		function_ = &BooleanFunction::XNOR;
	} else if (_functionString == "buf") {
		function_ = &BooleanFunction::BUF;
	} else if (_functionString == "buff") {
		function_ = &BooleanFunction::BUF;
	} else if (_functionString == "copy") {
		function_ = &BooleanFunction::BUF;
	} else if (_functionString == "not") {
		function_ = &BooleanFunction::NOT;
	}

	else {
		throw "Cannot create Boolean function from this string.";
	}
}

template<class _valueType>
inline BooleanFunction<_valueType>::BooleanFunction(const BooleanFunction & _obj) : Function<_valueType>(_obj.string()) {
	std::string _functionString = _obj.string();
	function_ = &BooleanFunction::BUF; //Overwritten, so it does not matter.  This prevents "uninitialized member" analysis.
	std::transform(_functionString.begin(), _functionString.end(), _functionString.begin(), (int(*)(int))std::tolower);
	if (_functionString == "and") {
		function_ = &BooleanFunction::AND;
	} else if (_functionString == "nand") {
		function_ = &BooleanFunction::NAND;
	} else if (_functionString == "or") {
		function_ = &BooleanFunction::OR;
	} else if (_functionString == "nor") {
		function_ = &BooleanFunction::NOR;
	} else if (_functionString == "xor") {
		function_ = &BooleanFunction::XOR;
	} else if (_functionString == "xnor") {
		function_ = &BooleanFunction::XNOR;
	} else if (_functionString == "buf") {
		function_ = &BooleanFunction::BUF;
	} else if (_functionString == "buff") {
		function_ = &BooleanFunction::BUF;
	} else if (_functionString == "copy") {
		function_ = &BooleanFunction::BUF;
	} else if (_functionString == "not") {
		function_ = &BooleanFunction::NOT;
	} else {
		throw "Cannot create Boolean function from this string.";
	}
}

template<class _valueType>
inline Function<_valueType>* BooleanFunction<_valueType>::clone() const {
	return new BooleanFunction<_valueType>(*this);
}

//-------------- Boolean Function Definitions

template<class _valueType>
_valueType BooleanFunction<_valueType>::evaluate(std::vector<_valueType> _vector) const {
	return (this->*function_)(_vector);
}

template<class _valueType>
_valueType BooleanFunction<_valueType>::AND(std::vector<_valueType> i) const {
	if (i.size() < 1) {
		throw "input to a boolean function must have at least one value.";
	}
	//NOTE: Before, the default value was (_width)0xFFFFFFFFFFFFFFFF instad of "maximum".
	//      This new implementation allows no knowledge of the "width".
	_valueType andOut = _valueType( _valueType::MAX() );
	for (_valueType &val : i) {
		andOut = andOut & val;
	}
	return andOut;
}

template<class _valueType>
_valueType BooleanFunction<_valueType>::NAND(std::vector<_valueType> i) const {
	return BooleanFunction::NOT(std::vector<_valueType>(1, BooleanFunction::AND(i)));
}

template<class _valueType>
_valueType BooleanFunction<_valueType>::OR(std::vector<_valueType> i) const {
	if (i.size() < 1) {
		throw "Input to a boolean function must have at least one value.";
	}
	//NOTE: Before, the default value was (_width)0x0 instad of "minimum".
	//      This new implementation allows no knowledge of the "width".
	_valueType orOUT = _valueType( _valueType::MIN() );
	for (_valueType &val : i) {
		orOUT = orOUT | val;
	}
	return orOUT;
}

template<class _valueType>
_valueType BooleanFunction<_valueType>::NOR(std::vector<_valueType> i) const {
	return BooleanFunction::NOT(std::vector<_valueType>(1, BooleanFunction::OR(i)));
}

template<class _valueType>
_valueType BooleanFunction<_valueType>::XOR(std::vector<_valueType> i) const {
	if (i.size() < 1) {
		throw "Input to a boolean function must have at least one value.";
	}
	//NOTE: Before, the default value was (_width)0x0 instad of "minimum".
//      This new implementation allows no knowledge of the "width".
	_valueType xorOUT = _valueType(_valueType::MIN());
	for (_valueType &val : i) {
		xorOUT = xorOUT ^ val;
	}
	return xorOUT;
}

template<class _valueType>
_valueType BooleanFunction<_valueType>::XNOR(std::vector<_valueType> i) const {
	return BooleanFunction::NOT(std::vector<_valueType>(1, BooleanFunction::XOR(i)));
}


template<class _valueType>
_valueType BooleanFunction<_valueType>::BUF(std::vector<_valueType> i) const {
	if (i.size() > 1) {
		throw "Cannot buffer more than 1 value.";
	}
	if (i.size() < 1) {
		throw "Input to a boolean function must have at least one value.";
	}
	return i.at(0);
}

template<class _valueType>
_valueType BooleanFunction<_valueType>::NOT(std::vector<_valueType> i) const {
	if (i.size() > 1) {
		throw "Cannot invert more than 1 value.";
	}
	if (i.size() < 1) {
		throw "Input to a boolean function must have at least one value.";
	}
	return ~i.at(0);
	//DELETE obsolete. return Value<bool>(!(i.at(0).magnitude()), i.at(0).valid());
}


#endif