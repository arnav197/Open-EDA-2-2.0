/**
 * @file Testpoint.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-02-05
 *
 * @copyright Copyright (c) 2019
 *
 */


#ifndef Testpoint_h
#define Testpoint_h

#include "structures/Circuit.h"
#include "faults/FaultStructures.hpp"
#include "simulation/Value.hpp"


/*
 * An object capible of modifying a circuit when activated.
 *
 * This class is pure virtual. Other methods must define the actions
 * corresponding to activation.
 *
 * A testpoint is "valued" in the sense that it will return value of its
 * location (presuming it is active).
 *
 * @parameter _nodeType When new needs are created, what kind of node should be created?
 * @parameter _lineType When new lines are created, what kind of line should be created?
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _nodeType, class _lineType, class _valueType>
class Testpoint : virtual public Evented<_valueType> {
public:
	/*
	 * Create a testpoint on a given Line.
	 *
	 * The testpoint will not be activated (i.e., no circuit modification will be
	 * done.
	 *
	 * @param _location The location of the testpoint
	 * @param (optional) _value The "value" of this testpoint.
	 */
	Testpoint(_lineType* _location, _valueType _value = _valueType()) {
		this->location_ = _location;
	}

	/*
	 * Two Testpoints are equal if they are on the same location.
	 *
	 * @param _other The other Testpoint being compared to.
	 * @return True if both are on the same location.
	 */
	virtual bool operator == (const Testpoint<_nodeType, _lineType, _valueType>& _other) const {
		return (this->location_ == _other.location());
	}

	/*
	 * See the overloaded operator "==", as this is the logical opposite.

	 * @param _other The other Testpoint being compared to.
	 * @return True if the Testpoints are different.
	 */
	virtual bool operator != (const Testpoint<_nodeType, _lineType, _valueType>& _other) const {
		return !(*this == _other);
	}

	/*
	 * Compaitor function (needed for sorted hashes of faults).
	 *
	 * The first "priotiry" of the sort is the pointer to the location.
	 *
	 * @param _other The other Testpoints being compared to.
	 */
	virtual bool operator < (const Testpoint<_nodeType, _lineType, _valueType>& _other) const {
		return (this->location_ < _other.location());
	}

	/*
	 * Activate the Testpoint by modifying the circuit.
	 *
	 * Lines and Nodes may be created by this function.
	 *
	 * @param _circuit The circuit which is modified (extra
	 *        nodes/pis/pos may be added/removed). 	 
	 * @return A set of new events for simulation.
	 */
	virtual std::vector<Event<_valueType>> activate(Circuit* _circuit) = 0;

	/*
	 * Deactivate the Testpoint by modifying the circuit.
	 *
	 * Lines and Nodes may be deleted by this function.
	 *
	 * @param  _circuit The circuit which is modified (extra 
	 *        nodes/pis/pos may be added/removed).
	 * @return A set of new events for simulation.
	 */
	virtual std::vector<Event<_valueType>> deactivate(Circuit* _circuit) = 0;

	/*
	 * The location of the TP.
	 *
	 * @return The location of the TP.
	 */
	_lineType* location() const {
		return this->location_;
	}


protected:
	/*
	 * The location of the Testpoint.
	 *
	 * @return The location of the Testpoint.
	 */
	_lineType* location_;
};




#endif
