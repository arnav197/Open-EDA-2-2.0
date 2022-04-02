/**
 * @file Value.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2018-12-24
 *
 * @copyright Copyright (c) 2018
 *
 */

#ifndef Value_h
#define Value_h

#include <limits> //For minumum/maximum numeric limits.
#include <vector>

#include "simulation/Function.hpp"

template <class _width>
class Function;

/*
 * A simultable value.
 *
 * A Value holds a value created during simulation. As opposed to raw data
 * types (bool, char, float, etc.), a Value holds extra informationa and has
 * support functions.
 *
 * @param _width The underlying data type. For most practical purposes, this will
 *        be "bool", but other nuanced uses include "char" for encrypted
 *		  logic.
 */
template <class _width>
class Value {
public:
	/*
	 * Create a defaul_width Value.
	 *
	 * The magnitude will have a "maximum max", "minimum min", and will not be
	 * valid.
	 */
	Value();

	/*
	 * Create a Value of given parameters.
	 *
	 * By default, the max/min magnitude with be the type maximmum/minimum.
	 *
	 * By default, if not given, the Value will be valid.
	 *
	 * @param _magnitude Magnitude of the Value.
	 * @param _valid (Optional) Is the value valid.
	 * @param _min (Optional) Minimum possible value of the Value.
	 * @param _max (Optional) Maximum possible value of the Value.
	 */
	Value(
		_width _magnitude, 
		_width _valid = 0xFFFFFFFFFFFFFFFF, 
		_width _min = std::numeric_limits<_width>::min(),
		_width _max = std::numeric_limits<_width>::max()
	);

	/*
	 * Two values are equal if they represent the same value (X, 0, or 1)
	 *
	 * @param _other The other value being compared to.
	 * @return Both invalid or both valid and different magnitudes.
	 */
	bool operator == (const Value<_width>& _other) const;

	/*
	 * Two values are not equal if they represent  different values (X, 0, or 1)
	 *
	 * @param _other The other value being compared to.
	 * @return True different valids or different magnitudes.
	 */
	bool operator != (const Value<_width>& _other) const;

	/*
	 * Compare two Values. It's only used for sorting values in certain data
	 * structures, e.g., sets. The order of sorting is X->0->1.
	 *
	 * @param _other The other value being compared to.
	 * @return True if both are valid and the magnitude of this Value is
	 *         < the magnitude of _other
	 */
	bool operator < (const Value<_width>& _other) const;

	/*
	 * Perform a bitwise "AND" (or equivalent) operation.
	 *
	 * @param _other The other value of the operation.
	 * @return The AND operation of the two Values.
	 */
	Value<_width> operator & (const Value<_width>& _other) const;

	/*
	 * Perform a bitwise "OR" (or equivalent) operation.
	 *
	 * @param _other The other value of the operation.
	 * @return The OR operation of the two Values.
	 */
	Value<_width> operator | (const Value<_width>& _other) const;

	/*
	 * Perform a bitwise "XOR" (or equivalent) operation.
	 *
	 * @param _other The other value of the operation.
	 * @return The XOR operation of the two Values.
	 */
	Value<_width> operator ^ (const Value<_width>& _other) const;

	/*
	 * Perform a bitwise "NOT" (or equivalent) operation.
	 *
	 * @return The NOT operation of the Value.
	 */
	Value<_width> operator ~ () const;

	/*
	 * Return the magnitude of the Value.
	 *
	 * @return The magnitude of the value.
	 */
	_width magnitude() const;

	/*
	 * Set the magnitude of the Value.
	 *
	 * @param _magnitude The new magnitude of the value.
	 * @return The assigned magnitude of the value./
	 */
	_width magnitude(_width _magnitude) ;

	/*
	 * Flip (i.e. invert) the magnitude of the Value and return the new magnitude.
	 * This is a convenience function: because the "~" operator does not apply
	 * to "bool", this allows a bool and a non-bool version to be embedded into
	 * the value type.
	 *https://stackoverflow.com/questions/29402367/does-using-bitwise-not-operator-on-boolean-values-invoke-undefined-behavior
	 *
	 * @return The magnitude of the value after flipping.
	 */
	virtual _width flip();

	 /*
	  * Return the validity of this Value.
	  *
	  * @return (bool) Is this Valid valid.
	  */
	 virtual _width valid() const;

	 /* 
	  * Set the validity of this Value.
	  *
	  * @param _valid The new validity of this Value.
	  * @return (bool) The new validity of thsi Value.
	  */
	 _width valid(_width _valid);

	 /*
	  * Increment's this Value's magnitude.
	  *
	  * return (bool) True of the magnitude "warpped around" and is now at its 
	  *        minimum.*/
	 virtual bool increment();

	 /*
	  * Return this Value's minimum value.
	  *
	  * @return This Value's minimum value.
	  */
	 _width min() const;

	 /*
	  * Return the theoretical minimum value of this Value's type.
	  *
	  * This is useful for creating "default" Values.
	  *
	  * @return The Value's theoretical minimum value.
	  */
	 static _width MIN() {
		 return std::numeric_limits<_width>::min();
	 }

	 /*
	  * Return this Value's maximum value.
	  *
	  * @return This Value's maximum value.
	  */
	 _width max() const;

	 /*
	  * Return the theoretical maximum value of this Value's type.
	  *
	  * This is useful for creating "default" Values.
	  *
	  * @return The Value's theoretical maximum value.
	  */
	 static _width MAX() {
		 return std::numeric_limits<_width>::max();
	 }

	 /*
	  * Return the cardinalit of this value, i.e., how many magnitudes it can hold.
	  *
	  * @return (unsigned long int) The number of magnitudes it can hold
	  */
	 unsigned long int cardinality() const;

	 /*
	  * A useful function for debugging, but it should not be used for any
	  * funcitonal purpose.
	  *
	  * @return The "name" of the value as a string (i.e., "X", "0", or "1").
	  */
	 virtual std::string name() {
		 if (this->valid() == 0x0)
		 {
			 return "X";
		 }
		 if (this->magnitude() == 0x0)
		 {
			 return "0";
		 }
		 return "1";
	 }

protected:
	//The  magnitude of the Value.
	_width magnitude_;

	//Is the Value valid?
	_width valid_;

	//The minimum magnitude.
	_width min_;

	//The maximum magnitude.
	_width max_;
};


/*
 * This object holds and optionally calculates Values.
 *
 * A Valued object must be able to hold and return a value. It can optionally
 * contain a Function, which if given, will calculate a new Value based on
 * Values given to it.
 *
 * NOTE: When the object is deconstructed, the function given will NOT be
 * deleted. This allows functions to be shared across differen_width Valued objects.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class Valued {
public:
	/*
	 * Initialize the value to the given Value.
	 *
	 * @param (optional) The function to use when calculating values.
	 * @param (optional) _value The initial value of this Valued oject.
	 */
	Valued(
		Function<_valueType>* _function = nullptr, 
		_valueType _value = _valueType()
	) {
		this->value_ = _value;
		this->function_ = _function;
	}

	/*
	 * Return the Value held by this Valued object (will not recalculate).
	 *
	 * @return the Value held by this object.
	 */
	virtual _valueType value() const {
		return this->value_;
	}

	/*
	 * Set the Value to the given Value.
	 *
	 * Inheriting members may interpret this function as needed.
	 *
	 * @param _value The Value to set to (by default, only the firs_width Value will be
	 *        used. Any vector of size != 1 will throw an error).
	 * @return The Value after the object is set.
	 */
	virtual _valueType value(std::vector<_valueType> _values) {
		if (this->function_ != nullptr) { //This Valued object has a function
			this->value_ = this->function_->evaluate(_values);
		} else {
			this->value_ = _values.at(0);
		}
		return this->value_;
	}

	/*
	 * Returns this object's Function.
	 *
	 * @return This object's Function.
	 */
	Function<_valueType>* function() const {
		return this->function_;
	}

	/*
	 * Set this object's Function.
	 *
	 * If this object already has a Function, the old one will be returned.
	 *
	 * @return This object's Function (after assignment)
	 */
	Function<_valueType>* function(Function<_valueType>* _function) {
		Function<_valueType>* toReturn = this->function_;
		this->function_ = _function;
		return toReturn;
	}
	
private:
	/*
	 * The current Value held by this object.
	 */
	_valueType value_;

	/*
	 * This Valued object's function. If nullptr, this object holds and stores
	 * Values without calculating.
	 */
	Function<_valueType>* function_;
};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////

template <class _width>
inline Value<_width>::Value() {
	min_ = std::numeric_limits<_width>::min();
	max_ = std::numeric_limits<_width>::max();
	magnitude_ = min_;
	valid_ = (_width)0x0000000000000000;
}

template<class _width>
inline Value<_width>::Value(_width _magnitude, _width _valid, _width _min, _width _max) {
	magnitude_ = _magnitude;
	valid_ = _valid;
	min_ = _min;
	max_ = _max;
}

template<class _width>
inline bool Value<_width>::operator==(const Value<_width>& _other) const {
	return !(*this != _other);
}

template<class _width>
inline bool Value<_width>::operator!=(const Value<_width>& _other) const {
	if (this->valid() ^ _other.valid()) { //One's valid and the other is not.
		return true;
	}
	if ((this->valid() & this->magnitude()) ^ (_other.valid() & _other.magnitude())) { //Magnitudes of valid bits mismatch
		return true;
	}
	return false;
}

template<class _width>
inline bool Value<_width>::operator<(const Value<_width>& _other) const {
	if (this->valid() < _other.valid()) {
		return true;
	} else if (!(_other.valid())) {
		return false; //This is needed, otherwise other factors may be processed
	}
	return this->magnitude() < _other.magnitude();
}

template<class _width>
inline Value<_width> Value<_width>::operator&(const Value<_width>& _other) const {
	_width valid = (_other.valid() & this->valid_) | (_other.valid() & ~(_other.magnitude())) | (this->valid_ & ~(this->magnitude()));
	return Value<_width>((_width)this->magnitude() & _other.magnitude(), valid);
}

//Technically, the "~" operator does not function properly on bool. Therefore,
//it is manaully defined here.
//https://stackoverflow.com/questions/29402367/does-using-bitwise-not-operator-on-boolean-values-invoke-undefined-behavior
//The same is not done fo rthe "OR" operator (|) because an inversion is not needed.
template<>
inline Value<bool> Value<bool>::operator&(const Value<bool>& _other) const {
	bool valid = (_other.valid() & this->valid_) | (_other.valid() & !(_other.magnitude())) | (this->valid_ & !(this->magnitude()));
	return Value<bool>(this->magnitude() & _other.magnitude(), valid);
}

template<class _width>
inline Value<_width> Value<_width>::operator|(const Value<_width>& _other) const {
	_width valid = ((_other.valid() & this->valid_) | (_other.valid() & (_other.magnitude())) | (this->valid_ & this->magnitude()));
	return Value<_width>((_width)this->magnitude() | _other.magnitude(), valid);
}

template<class _width>
inline Value<_width> Value<_width>::operator^(const Value<_width>& _other) const {
	_width valid = (_other.valid() & this->valid_);
	return Value<_width>((_width)this->magnitude() ^ _other.magnitude(), valid);
}

template<class _width>
inline Value<_width> Value<_width>::operator~() const {
	return Value<_width>(~this->magnitude_, this->valid_);
}

//Technically, the "~" operator does not function properly on bool. Therefore,
//it is manaully defined here.
//https://stackoverflow.com/questions/29402367/does-using-bitwise-not-operator-on-boolean-values-invoke-undefined-behavior
template<>
inline Value<bool> Value<bool>::operator~() const {
	return Value<bool>(!this->magnitude_, this->valid_);
}



template<class _width>
inline _width Value<_width>::magnitude() const {
	return this->magnitude_;
}

template<class _width>
inline _width Value<_width>::magnitude(_width _magnitude) {
	if (_magnitude < this->min() || _magnitude > this->max()) {
		throw "Cannot set the magnitude of a Value out-of-bounds.";
	}
	this->magnitude_ = _magnitude;
	return _magnitude;
}

template<class _width>
inline _width Value<_width>::flip() {
	this->magnitude_ = ~this->magnitude_;
	return this->magnitude_;
}

template<>
inline bool Value<bool>::flip() {
	if (this->magnitude_) {
		this->magnitude_ = false;
	} else {
		this->magnitude_ = true;
	}
	return this->magnitude_;
}

template<class _width>
inline _width Value<_width>::valid() const {
	return this->valid_;
}

template<class _width>
inline _width Value<_width>::valid(_width _valid) {
	this->valid_ = _valid;
	return _valid;
}

template<class _width>
inline bool Value<_width>::increment() {
	if (this->magnitude_ == this->max()) {
		this->magnitude_ = this->min();
		return true;
	}
	this->magnitude_ = this->magnitude_ + 1;
	return false;
}

template<>
inline bool Value<unsigned long long int>::increment() {
	if (this->magnitude_) { //there's a 1
		this->magnitude_ = 0x0000000000000000;
		return true;
	}
	this->magnitude_ = 0xFFFFFFFFFFFFFFFF;
	return false;
}

template<class _width>
inline _width Value<_width>::min() const {
	return this->min_;
}

template<class _width>
inline _width Value<_width>::max() const {
	return this->max_;
}

template<class _width>
inline unsigned long int Value<_width>::cardinality() const {
	return (unsigned long int) this->max() - (unsigned long int) this->min() + 1;
}


#endif

