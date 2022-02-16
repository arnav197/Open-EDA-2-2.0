/**
 * @file Testpoint_observe.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-02-13
 *
 * @copyright Copyright (c) 2019
 *
 */


#ifndef Testpoint_observe_h
#define Testpoint_observe_h

#include "Testpoint.hpp"
#include "FaultStructures.hpp"
#include "Value.hpp"

 /*
  * An observe testpoint.
  *
 * @parameter _nodeType When new needs are created, what kind of node should be created?
 * @parameter _lineType When new lines are created, what kind of line should be created?
 * @parameter _valueType The type of value to be simulated. Example: Value/FaultyValue
 */
template <class _nodeType, class _lineType, class _valueType>
class Testpoint_observe : public Testpoint<_nodeType, _lineType, _valueType> {
public:
	/*
	 * Create an observe testpoint with a given constant value.
	 *
	 * @param _locaiton The location of the testpoint.
	 * @param (ignored) _value
	 */
	Testpoint_observe(_lineType* _location, _valueType _value = _valueType()) :
		Testpoint<_nodeType, _lineType, _valueType>(_location) {
		this->newNode_ = nullptr;
	};

	/*
	 * Activate the Testpoint by modifying the circuit.
	 *
	 * Lines and Nodes may be created by this function.
	 *
	 * @param _circuit The circuit which is modified (extra
	 *        nodes/pis/pos may be added/removed).
	 * @return A set of new events for simulation.
	 */
	virtual std::vector<Event<_valueType>> activate(Circuit* _circuit) {
		if (this->newNode_ != nullptr) {
			throw "Obseve point is already active.";
		}
		this->newNode_ = new _nodeType(
			nullptr,
			"TP_observe" //NOTE: the name is important for COP calculations.
			);
		this->location_->addOutput(this->newNode_);
		_circuit->addPO(this->newNode_);
		return this->location_->go(); //In theory, not much should happen, but the O.P.'s value should be updated.
	};

	/*
	 * Deactivate the Testpoint by modifying the circuit.
	 *
	 * Lines and Nodes may be deleted by this function.
	 *
	 * @param  _circuit The circuit which is modified (extra
	 *        nodes/pis/pos may be added/removed).
	 * @return A set of new events for simulation.
	 */
	virtual std::vector<Event<_valueType>> deactivate(Circuit* _circuit){
		this->location_->removeOutput(this->newNode_);
		_nodeType* toReturn = this->newNode_;
		_circuit->removeNode(this->newNode_);
		delete this->newNode_;
		this->newNode_ = nullptr;
		return std::vector<Event<_valueType>>(); //No new event should ever be returned: how can adding an O.P. change any value?
	};

private:
	/*
	 * The new node created to implement the observe point (the PO).
	 */
	_nodeType* newNode_;
};

#endif


