/**
 * @file Fault.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-17
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef Fault_h
#define Fault_h

#include <set>
#include <vector>

#include "simulation/Value.hpp"
#include "simulation/EventQueue.hpp"
#include "faults/FaultStructures.hpp"

 // Forward declaration.
template <class _valueType>
class Faulty;
template <class _valueType>
class FaultyLine;

/*
 * A modification to a Valued (Faulty) object.
 *
 * A Fault, when activated, will change the Value returned by a Faulty object.
 *
 * The "event" of the fault is to "activate and change fault values".
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class Fault : public Evented<_valueType> {
public:
	
	/*
	 * The default fault has no (nullptr) location and a default value.
	 */
	Fault();

	/*
	 * The given object is stuck at the given Value.
	 *
	 * @param _location The location of the Fault.
	 * @param _value The stuck-at value of the Fault.
	 */
	Fault(FaultyLine<_valueType> * _location, _valueType _value);

	/*
	 * When a fault is destoryed, nothing will happen: the location will be
	 * untouched.
	 */
	~Fault();

	/*
	 * Copy constructor: copy the fault's location and value.
	 *
	 * @param _fault The fault to copy.
	 */
	Fault(const Fault<_valueType>& _fault);

	/*
	 * Assignment operator.
	 *
	 * @param _fault The fault to assign (copy).
	 */
	Fault<_valueType> operator = (const Fault<_valueType> _fault);

	/*
	 * Two faults are equal if they are on the same location and hold the same value.
	 *
	 * @param _other The other fault being compared to.
	 * @return True if both magnitudes are the same and they are both valid.
	 */
	bool operator == (const Fault<_valueType>& _other) const;

	/*
	 * See the overloaded operator "==", as this is the logical opposite.

	 * @param _other The other fault being compared to.
	 * @return True if the faults are different.
	 */
	bool operator != (const Fault<_valueType>& _other) const;

	/*
	 * Compaitor function (needed for sorted hashes of faults).
	 *
	 * The first "priotiry" of the sort is the pointer to the location.
	 * The second "priotiry" is the value of the fault.
	 *
	 * @param _other The other fault being compared to.
	 */
	bool operator < (const Fault<_valueType>& _other) const;

	/*
	 * Return the value of this Fault.
	 *
	 * @return The stuck-at value of the fault.
	 */
	_valueType value() const;

	/*
	 * Calculate the value returned by this fault based on the given value.
	 *
	 * For the stuck-at fault model, this will do one of the following:
	 * 1) Return the stuck-at value regardless of the Values given.
	 *
	 * @param (ignored) _value The Value to set to.
	 * @return The Value of this fault.
	 */
	virtual _valueType value(std::vector<_valueType> _values);

	/*
	 * Return the loaction of this Fault.
	 *
	 * @return The location of this fault.
	 */
	FaultyLine<_valueType>* location() const;

	/*
	 * (De)activate this Fault and return all output new events which need to be
	 * re-evaluated.
	 *
	 * @return New events (and their priority) created by activating this event.
	 */
	virtual std::vector<Event<_valueType>> go();



private:
	/*
	 * Activate the Fault.
	 *
	 * @return True if activating the fault changes the line's value.
	 */
	bool activate();

	/*
	 * Deactivate the Fault.
	 *
	 * @return True if deactivating the fault changes the line's value.
	 */
	bool deactivate();


	/*
	 * The location of the Fault.
	 */
	FaultyLine<_valueType>* location_;

};

/*
 * An object which can contain and act on a Fault.
 *
 * A Faulty object acts like a normal Valued object, except the Value it
 * returns can be overwritten by an active Fault.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class Faulty : public virtual Valued<_valueType> {
public:
	/*
	 * Initialize with the "Default" Value, which is not valid.
	 */
	Faulty();

	/*
	 * Return the Value held by this Valued object.
	 *
	 * If there is an active Fault, then the Fault's value will be returned.
	 *
	 * @return the Value held by this object (with the effect of the Fault).
	 */
	virtual _valueType value() const;



	/*
	 * Set the Value to the given Value.
	 *
	 * If a Fault is active, no effect may happen.
	 *
	 * The underlying Valued object will have its Value set, but the return Value
	 * will include the effect of the Fault.
	 *
	 * @param _value The Value to set to.
	 * @return The Value after the object is set.
	 */
	virtual _valueType value(std::vector<_valueType> _values);

	/*
	 * Activate a given Fault.
	 *
	 * An exception will be thrown if a Fault is already active.
	 *
	 * @param The Fault to activate.
	 * @return The Value of the object after the Fault is activated.
	 */
	_valueType activate(Fault<_valueType>* _fault);

	/*
	 * Deactivate a given Fault.
	 *
	 * An exception will be thrown if the Fault given does not match the currently
	 * active Fault or no Fault is currently active.
	 *
	 * @param The Fault to deactivate.
	 * @return The Value of the object after the Fault is deactivated.
	 */
	_valueType deactivate(Fault<_valueType>* _fault);

	/*
	 * Return true if the given fault is currently active on this line.
	 *
	 * @param _fault the Fault to check if active.
	 * @return True if the fault is currently cative.
	 */
	bool isFaultActive(Fault<_valueType>* _fault);

	///////////////////////////////////////////////////////////////////////////
	// Fault rememberance functions.
	/*
	 * Add fault to this object's remembered faults.
	 *
	 * @param _fault The Fault to add.
	 *
	 */
	void addFault(Fault<_valueType>*_fault) {
		this->faults_.emplace(_fault);
	}

	/*
	 * Delete a fault from this object's remembered faults.
	 *
	 * @param _fault The Fault to delete.
	 *
	 */
	void deleteFault(Fault<_valueType>*_fault) {
		this->faults_.erase(_fault);
	}

	/*
	 * Return the list of faults which CAN occur on this object.
	 *
	 * When a fault is created for a given location, it will will add itself to
	 * this the Faulty object's list.
	 *
	 * @return The list of faults which were created for this object.
	 */
	std::unordered_set<Fault<_valueType>*>getfaults() {
		return this->faults_;
	}

private:
	/*
	 * The set of (possible) Faults on the line.
	 */
	std::unordered_set<Fault<_valueType>*> faults_;

	/*
	 * Is there a fault currently active?
	 */
	bool active_;

	/*
	 * What is the currently active fault?
	 */
	Fault<_valueType>* fault_;

};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////

template <class _valueType>
inline Fault<_valueType>::Fault() {
	this->location_ = nullptr;
}

template <class _valueType>
inline Fault<_valueType>::Fault(FaultyLine<_valueType> * _location, _valueType _value) {
	this->location_ = _location;
	this->Valued<_valueType>::value(std::vector<_valueType>({ _value }));
	this->location_->addFault(this);
	std::string valueName = _value.valid() ?
		_value.magnitude() ? "1" : "0"
		: "X";
	this->name("fault_" + this->location_->name() + "_" + valueName);
}

template <class _valueType>
inline Fault<_valueType>::~Fault() {
	if (this->location_->isFaultActive(this) == true) {
		throw "Cannot delete a fault which is currently active.";
	}
	this->location_->deleteFault(this);
}

template <class _valueType>
inline Fault<_valueType>::Fault(const Fault<_valueType>& _fault) {
	this->location_ = _fault.location();
	this->Valued<_valueType>::value(std::vector<_valueType>({ _fault.value() }));
}

template <class _valueType>
inline Fault<_valueType> Fault<_valueType>::operator=(const Fault<_valueType> _fault) {
	this->location_ = _fault.location();
	this->Valued<_valueType>::value(std::vector<_valueType>({ _fault.value() }));
	return *this;
}

template <class _valueType>
inline bool Fault<_valueType>::operator==(const Fault<_valueType>& _other) const {
	if (this->value() != _other.value()) {
		return false;
	}
	if (this->location_ != _other.location()) {
		return false;
	}
	return true;
}

template <class _valueType>
inline bool Fault<_valueType>::operator!=(const Fault<_valueType>& _other) const {
	return !(*this == _other);
}

template <class _valueType>
inline bool Fault<_valueType>::operator<(const Fault<_valueType>& _other) const {
	if (this->location_ < _other.location()) {
		return true;
	}
	if (this->location_ > _other.location()) {
		return false;
	}
	if (this->value() < _other.value()) {
		return true;
	}
	return false;
}

template <class _valueType>
inline _valueType Fault<_valueType>::value() const {
	return this->Valued<_valueType>::value();
}

template <class _valueType>
inline _valueType Fault<_valueType>::value(std::vector<_valueType> _values) {
	return this->value();
}

template <class _valueType>
inline FaultyLine<_valueType>* Fault<_valueType>::location() const {
	return this->location_;
}

template <class _valueType>
inline bool Fault<_valueType>::activate() {
	_valueType oldValue = this->location_->value();
	_valueType newValue = this->location_->activate(this);
	return oldValue != newValue;
}

template <class _valueType>
inline bool Fault<_valueType>::deactivate() {
	_valueType oldValue = this->location_->value();
	_valueType newValue = this->location_->deactivate(this);
	return oldValue != newValue;
}

template <class _valueType>
inline std::vector<Event<_valueType>> Fault<_valueType>::go() {
	bool forwardUpdateCall = false;
	//Is this fault currently active?
	//std::unordered_set<Connecting*> DEBUG_inputs = this->location()->inputs();
	//unsigned int sizeBefore = DEBUG_inputs.size();

	if (this->location_->isFaultActive(this) == true) {
		forwardUpdateCall = this->deactivate();
	}
	else {
		forwardUpdateCall = this->activate();
	}

	if (forwardUpdateCall == true) { //The value changed, and therefore we need to return outputs which (may) need to be re-evaluated.
		std::vector<Event<_valueType>> toReturn;
		for (Connecting* output : this->location_->outputs()) {
			Evented<_valueType>* cast = dynamic_cast<Evented<_valueType>*>(output);
			size_t eventLevel = cast->inputLevel();
			toReturn.push_back(
				cast->selfAsEvent()
			);
		}
		return toReturn;
	}

	return std::vector<Event<_valueType>>();
}

template <class _valueType>
inline Faulty<_valueType>::Faulty() {
	this->active_ = false;
	this->fault_ = nullptr;
}

template <class _valueType>
inline _valueType Faulty<_valueType>::value() const {
	//Is there an active fault on this line?
	if (this->active_ == false) {//no
		return this->Valued<_valueType>::value();
	}

	//Yes return the (potentially) faulty value.
	_valueType faultyValue = this->fault_->value();
	_valueType goodValue = this->Valued<_valueType>::value();

	return _valueType(goodValue, faultyValue); //NOTE: the _valueType needs to have a good/faulty value type constructor.
}

template <class _valueType>
inline _valueType Faulty<_valueType>::value(std::vector<_valueType> _values) {
	this->Valued<_valueType>::value(_values);
	return this->value();
	//_valueType goodValue = this->Valued<_valueType>::value(_values); //Will always be calculated.
	//if (this->active_ == false) {
	//	return goodValue;
	//}
	//_valueType faultyValue = this->fault_->value(_values);
	//return faultyValue; //Will redirect to faulty value if needed.
}

template <class _valueType>
inline _valueType Faulty<_valueType>::activate(Fault<_valueType>* _fault) {
	if (this->active_ == true) {
		throw "Cannot active fault: already active.";
	}
	this->active_ = true;
	this->fault_ = _fault;
	return this->fault_->value();
}

template <class _valueType>
inline _valueType Faulty<_valueType>::deactivate(Fault<_valueType>* _fault) {
	if (this->active_ == false) {
		throw "Cannot deactivate fault: no fault is active.";
	}
	if (_fault != this->fault_) {
		throw "Cannot deactiveate fault: incorrect fault.";
	}
	this->active_ = false;
	this->fault_ = nullptr;
	return this->value();
}

template <class _valueType>
inline bool Faulty<_valueType>::isFaultActive(Fault<_valueType>* _fault) {
	return fault_ == _fault;
}

#endif