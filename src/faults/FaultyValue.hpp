/**
 * @file FaultyValue.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-12-29
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef FaultyValue_h
#define FaultyValue_h

#include "simulation/Value.hpp"

/*
 * A FaultyValue extends the functionality of a regular Value by modeling two-
 * frame simulation: the first "frame" represents the effect of a fault in a 
 * good (fault-free) circuit whilst the second "frame" represents the state of a
 * circuit in the presence of a fault.
 *
 * All inherited functions (magnitude, valid, etc.) return the value in the 
 * "faulty frame". To find if the value if from a fault, call the "faulty"
 * function.
 *
 * @param _primitive The underlying data type. For most practical purposes, this
 *        will be "bool", but other nuanced uses include "char" for encrypted
 *		  logic and "unsigned long long int" for 64-bit vector simulation.
 */
template <class _primitive>
class FaultyValue : virtual public Value<_primitive> {

public:
	/*
	 * Create a default Value.
	 *
	 * The magnitude will have a "maximum max", "minimum min", and will not be
	 * valid.
	 */
	FaultyValue() : Value<_primitive>() {
		this->originalFrame_ = Value<_primitive>();
	}

	/*
	 * Create a faulty-free FaultyValue of given parameters.
	 *
	 * With this constructor, the faulty and original frame will be identical.
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
	FaultyValue(
		_primitive _magnitude,
		_primitive _valid = 0xFFFFFFFFFFFFFFFF,
		_primitive _min = std::numeric_limits<_primitive>::min(),
		_primitive _max = std::numeric_limits<_primitive>::max()
	) : Value<_primitive>(_magnitude, _valid, _min, _max) {
		this->originalFrame_ = Value<_primitive>(_magnitude, _valid, _min, _max);
	}

	/*
	 * Create a FaultyValue with different original and faulty frames.
	 *
	 * This constructor is the only way to create a value which holds a fault.
	 *
	 * @param _goodFrame The Value of the good (original) frame.
	 * @param _faultyFrame The Value of the faulty frame.
	 */
	FaultyValue(Value<_primitive> _goodFrame, Value<_primitive> _faultyFrame) : Value<_primitive>(_faultyFrame) {
		this->originalFrame_ = _goodFrame;
	}

	/*
	 * Is a fault seen on this value?
	 *
	 * @return For each primitive bit, 1's correspond to bits which see a fault.
	 */
	_primitive faulty() const;

	////////////////////////////////////////////////////////////////////////////
	// Inherited functions
	////////////////////////////////////////////////////////////////////////////
		/*
	 * Two values are not equal if they represent  different values (X, 0, 1, D, or U)
	 *
	 * @param _other The other value being compared to.
	 * @return True different valids or different magnitudes.
	 */
	bool operator != (const FaultyValue<_primitive>& _other) const;

	/*
	 * Compare two Values. It's only used for sorting values in certain data
	 * structures, e.g., sets.
	 *
	 * The order of sorting is based on good-frame, then faulty-frame. Given
	 * regular values are sorted on a X->0->1 order, this is the new sorting
	 * order...
	 *
	 * XX, X0, X1, 0X, 00 (0), 01 (U), 1X, 10 (D), 11 (1)
	 *
	 * @param _other The other value being compared to.
	 * @return True if both are valid and the magnitude of this Value is
	 *         < the magnitude of _other
	 */
	bool operator < (const FaultyValue<_primitive>& _other) const;

	/*
	 * Perform a bitwise "AND" (or equivalent) operation.
	 *
	 *   0 1 D U X
	 * 0 0 0 0 0 0
	 * 1 0 1 D U X
	 * D 0 D D 0 X  <-- NOTE: This was originally "0", but it shouldn't be
	 * U 0 U 0 U X
	 * X 0 X X X X
	 *
	 * @param _other The other value of the operation.
	 * @return The AND operation of the two FaultyValues.
	 */
	FaultyValue<_primitive> operator & (const FaultyValue<_primitive>& _other) const;

	/*
	 * Perform a bitwise "OR" (or equivalent) operation.
	 *
	 *   0 1 D U X
	 * 0 0 1 D U X
	 * 1 1 1 1 1 1
	 * D D 1 D 1 X
	 * U U 1 1 U X  <-- NOTE: This was originally "1", but it shouldn't be
	 * X X 1 X X X
	 *
	 * @param _other The other value of the operation.
	 * @return The OR operation of the two FaultyValues.
	 */
	FaultyValue<_primitive> operator | (const FaultyValue<_primitive>& _other) const;

	/*
	 * Perform a bitwise "XOR" (or equivalent) operation.
	 *
	 *   0 1 D U X
	 * 0 0 1 D U X
	 * 1 1 0 U D X
	 * D D U 0 1 X
	 * U U D 1 0 X
	 * X X X X X X
	 *
	 * @param _other The other value of the operation.
	 * @return The XOR operation of the two FaultyValue.
	 */
	FaultyValue<_primitive> operator ^ (const FaultyValue<_primitive>& _other) const;

	/*
	 * Perform a bitwise "NOT" (or equivalent) operation.
	 *
	 * @return The NOT operation of the FaultyValue.
	 */
	FaultyValue<_primitive> operator ~ () const;

	/*
	 * Return the validity of this Value.
	 *
	 * Unlike a regular Value, the bits of a FaultyValue are set only if both
	 * the original and faulty frame are valid.
	 *
	 * @return (bool) Is this Valid valid.
	 */
	virtual _primitive valid() const {
		return this->valid_ & this->originalFrame_.valid();
	}

	/*
	 * Flip (i.e. invert) the magnitude of the Value and return the new magnitude.
	 * @return The magnitude of the value after flipping.
	 */
	virtual _primitive flip() {
		this->originalFrame_.flip();
		return this->Value<_primitive>::flip();
	}

	/*
	 * Increment's this Value's magnitude.
	 *
	 * return (bool) True of the magnitude "warpped around" and is now at its
	 *        minimum.*/
	virtual bool increment() {
		this->originalFrame_.increment();
		return this->Value<_primitive>::increment();
	}

	/*
	 * A useful function for debugging, but it should not be used for any
	 * funcitonal purpose.
	 *
	 * @return The "name" of the value as a string. Possibilities include:
	 *          0: Both time frames are 0
	 *          1: Both time frames are 1
	 *          X: Both time frames are X
	 *          D: Original time frame is 1, faulty is 0
	 *          U: Original time frame is 0, faulty is 1
	 *         X1: Original time frame is X, faulty is 1
	 *         X0: Original time frame is X, faulty is 0
	 *         1X: Original time frame is 1, faulty is X
	 *         0X: Original time frame is 0, faulty is X.
	 */
	virtual std::string name();

	/*
	 * Return the original frame value.
	 *
	 * @return The original frame value.
	 */
	Value<_primitive> originalFrame() const {
		return this->originalFrame_;
	}

	/*
	 * Return a Value representing the faulty frame.
	 *
	 * NOTE: The reason for this function is to allow the "valid" function to 
	 *       be set only if both the faulty and the good frame are valid.
	 *
	 * @return A Value representing the faulty frame.
	 */
	Value<_primitive> faultyFrame() const {
		return Value<_primitive>(this->magnitude_, this->valid_, this->min_, this->max_);
	}

private:
	/*
	 * The function of a FaultyValue is built using two regualar Values. The
	 * "original" Value (which this inherits from) holds the "faulty" frame,
	 * while the embedded value below holds the "non-faulty" frame.
	 */
	Value<_primitive> originalFrame_;

};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////

template<class _primitive>
inline bool FaultyValue<_primitive>::operator!=(const FaultyValue<_primitive>& _other) const {
	if (this->originalFrame_ != _other.originalFrame())
	{
		return true;
	}
	Value<_primitive> thisFaultyFrame = this->faultyFrame();
	Value<_primitive> otherFaultyFrame = _other.faultyFrame();
	if (thisFaultyFrame != otherFaultyFrame) {
		return true;
	}
	return false;
}

template<class _primitive>
inline bool FaultyValue<_primitive>::operator<(const FaultyValue<_primitive>& _other) const {
	if (this->originalFrame_ < _other.originalFrame()) {
		return true;
	} else if (_other.originalFrame() < this->originalFrame_) {
		return false;
	}

	Value<_primitive> thisFaultyFrame = Value<_primitive>(*this);
	Value<_primitive> otherFaultyFrame = Value<_primitive>(_other);
	if (thisFaultyFrame < otherFaultyFrame) {
		return true;
	}
	return false;
}

template<class _primitive>
inline FaultyValue<_primitive> FaultyValue<_primitive>::operator&(const FaultyValue<_primitive>& _other) const
{
	Value<_primitive> thisFaultyFrame = Value<_primitive>(*this);
	Value<_primitive> otherFaultyFrame = Value<_primitive>(_other);
	return FaultyValue<_primitive>(
		this->originalFrame_ & _other.originalFrame(),
		thisFaultyFrame & otherFaultyFrame
		);
}

template<class _primitive>
inline FaultyValue<_primitive> FaultyValue<_primitive>::operator|(const FaultyValue<_primitive>& _other) const
{
	Value<_primitive> thisFaultyFrame = Value<_primitive>(*this);
	Value<_primitive> otherFaultyFrame = Value<_primitive>(_other);
	return FaultyValue<_primitive>(
		this->originalFrame_ | _other.originalFrame(),
		thisFaultyFrame | otherFaultyFrame
		);
}


template<class _primitive>
inline FaultyValue<_primitive> FaultyValue<_primitive>::operator^(const FaultyValue<_primitive>& _other) const
{
	Value<_primitive> thisFaultyFrame = Value<_primitive>(*this);
	Value<_primitive> otherFaultyFrame = Value<_primitive>(_other);
	return FaultyValue<_primitive>(
		this->originalFrame_ ^ _other.originalFrame(),
		thisFaultyFrame ^ otherFaultyFrame
		);
}

template<class _primitive>
inline FaultyValue<_primitive> FaultyValue<_primitive>::operator~() const
{
	Value<_primitive> faultyFrame = Value<_primitive>(*this);
	return FaultyValue<_primitive>(~this->originalFrame_, ~faultyFrame);
}


template<class _primitive>
inline _primitive FaultyValue<_primitive>::faulty() const
{
	_primitive valids = this->valid_ & this->originalFrame_.valid();
	_primitive changedMagnitudes = this->magnitude() ^ this->originalFrame_.magnitude();
	return valids & changedMagnitudes;
}

template <class _primitive>
inline std::string FaultyValue<_primitive>::name() {
	int index = this->originalFrame_.valid() ? 8 : 0;
	index += this->originalFrame_.magnitude() ? 4 : 0;
	index += this->valid_ ? 2 : 0;
	index += this->magnitude() ? 1 : 0;

	switch (index)
	{
	case 0:  return "X"  ; break;
	case 1:  return "X" ; break;
	case 2:  return "X0"  ; break;
	case 3:  return "X1" ; break;
	case 4:  return "X" ; break;
	case 5:  return "X"  ; break;
	case 6:  return "X0" ; break;
	case 7:  return "X1"  ; break;
	case 8:  return "0X"  ; break;
	case 9:  return "0X" ; break;
	case 10: return "0"  ; break;
	case 11: return "U" ; break;
	case 12: return "1X" ; break;
	case 13: return "1X"  ; break;
	case 14: return "D" ; break;
	case 15: return "1"  ; break;
	default: throw "Index does not correspond to value string"; break;
	}
	return "X";
}

#endif