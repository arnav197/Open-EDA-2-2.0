/**
 * @file Function.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-05-06
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <vector>
#include <unordered_set>

#include "simulation/Value.hpp"
#include "simulation/ValueVectorFunctions.hpp"	//shift
#include "simulation/Function.hpp" //XOR

#ifndef PRPG_H
#define PRPG_h

 /*
  * A peudo-random pattern generator (PRPG).
  *
  * A PRPG implements a LFSR and calculates returned vectors of values based on
  * this LFSR. By using this object as opposed to pure-random vectors (i.e.,
  * using the built-in ValueVectorFunction::random function), generated stimulus
  * will be 1) more predictable, which will make debugging attempts more
  * consistent, and 2) more representitive of hardware-applied tests.
  *
  * The LFSR implemented here is a 31-bit external LFSR. All vectors generated
  * are presumed to be part of a scan chain fed by the LSB of this LFSR. This
  * implies sequential vectors generated by the PRPG are highly correlated to
  * eachother, as they will be the shifted result of the previous vector with
  * a single bit difference.
  *
  * Unless otherwise specified, the LFSR is loaded with an "all 1s" seed.
  *
  * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
  */
template <class _valueType>
class PRPG {
public:
	/*
	 * Create the PRPG of a given size and given seed.
	 *
	 * @param _size The number of bits in the PRPG (note: the size of the LFSR
	 *              which generates the patterns is always 31 bits).
	 * @param (optional) _seed The seed to load the PRPG with.
	 */
	PRPG(
		size_t _size,
		unsigned long long int _seed = (unsigned long long int)1
		//std::vector<Value<_primitive>> _seed = std::vector<Value<_primitive>>()
	);

	/*
	 * Fetch a vector from the PRPG (after incrementing it by a single shift).
	 *
	 * @returns The vector generated by the PRPG.
	 */
	std::vector<_valueType> increment();

	/*
	 * Fetch a vector from the PRPG (after performing a full load).
	 *
	 * @returns The vector generated by the PRPG.
	 */
	std::vector<_valueType> load();

private:
	/*
	 * The current state of the PRPG.
	 */

	/*
	 * The last-returned vector.
	 */
	std::vector<_valueType> last_;

	/*
	 * The current state of the LFSR.
	 */
	unsigned long long int lfsr_;

	/*
	 * The LFSR XORed indicies (exponents).
	 *
	 * The "1s" are the indicies.
	 *
	 * The stnadard XOR bits for a 64-bit LFSR are 1, 3, 4 ('0' is part of the wrap-around, but is not "inverted")
	 * Since bits are shifted right, these positions are tranlated to 62, 60, and 59 ("63" is the MSB which "wraps around").
	 * //DELETE: old external LFSR //The standard XORs bits for the 31-bit lfsr are 0 and 3.
	 */
	//DELETE: 31-bit external lfsr //unsigned long int exp_ = 0x00000009;
	unsigned long long int exp_ = 0x5800000000000000;
	
};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////

template<class _valueType>
inline PRPG<_valueType>::PRPG(size_t _size, unsigned long long int _seed) {
	//PRPG<_primitive>::PRPG(size_t _size, std::vector<Value<_primitive>> _seed) {
		//if (_seed.size() == 0) { //No seed defined, create the "default"
		//	_seed = 0xFFFFFFFF; // std::vector<Value<_primitive>>(31, Value<_primitive>((_primitive)1));
		//}
		//if (_seed.size() != 31) {
		//	throw "PRPG seed size must be 31 bits.";
		//}

	this->lfsr_ = _seed;
	this->last_ = std::vector<_valueType>(_size, _valueType(0));
	this->load();
}

//A special conversion case is needed for Boolean values.
template<class _primitive>
inline _primitive prpgToPrimitive(unsigned long long int _prpg) {
	return (_primitive)_prpg;
}

template<>
inline bool prpgToPrimitive(unsigned long long int _prpg) {
	return (bool)(_prpg % 2);
}

template<class _valueType>
inline std::vector<_valueType> PRPG<_valueType>::increment() {
	//std::vector<Value<_primitive>> XORedValues{ 
	//	this->lfsr_.at(0), //"0"
	//	this->lfsr_.at(3)  //"3"
	//};
	//BooleanFunction function = BooleanFunction("xor");
	//Value<_primitive> newValue = function.evaluate(XORedValues);
	//newValue = ValueVectorFunction<bool>::shift(this->lfsr_, newValue);
	//ValueVectorFunction<bool>::shift(this->last_, newValue);


	//Find the "new bit"
	//DELETE This is for external type LFSRs, which are not as efficient for
	//unsigned int XORedValues = this->lfsr_ ^ this->exp_;
	//bool parity = false;  // parity will be the parity of XORedValues
	//while (XORedValues) {
	//	parity = !parity;
	//	XORedValues = XORedValues & (XORedValues - 1);
	//}
	////Shift and set the 31st bit.
	//this->last_ = this->lfsr_ >> 1;
	//if (parity == true) {
	//	this->lfsr_ = this->lfsr_ | 0x40000000;
	//} else {
	//	this->lfsr_ = this->lfsr_ & 0xBFFFFFFF;
	//}

	//Load the chain
	//Go through this LFSRs entries and shift-and-load them.
	for (size_t i = this->last_.size() - 1; i > 0; i--) { //Need to go in reverse.
		this->last_[i] = this->last_.at(i - 1);
	}
	this->last_[0] = _valueType(this->lfsr_ & _valueType::MAX()); //NOTE: formerly "prpg to primitive".

	//Update the LFSR
	bool parity = false;
	if (this->lfsr_ & 0x1) { //last bit is "1"
		parity = true;
	}
	this->lfsr_ = (this->lfsr_ >> 1) | (this->lfsr_ << 63); //C doesn't have a native "rotate". This implments the rotate.
	if (parity == true) {
		this->lfsr_ = this->lfsr_ ^ this->exp_;
	}

	return this->last_;
}

template<class _valueType>
inline std::vector<_valueType> PRPG<_valueType>::load() {
	for (size_t i = 0; i < this->last_.size() - 1; i++) { //NOTE: the "-1" is so the last value can be easily returned.
		this->increment();
	}
	return this->increment(); //NOTE: see the above note on "-1".
}

#endif //PRPG_h