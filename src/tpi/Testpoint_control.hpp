/**
 * @file Testpoint_control.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-02-05
 *
 * @copyright Copyright (c) 2019
 *
 */


#ifndef Testpoint_control_h
#define Testpoint_control_h

#include "tpi/Testpoint.hpp"
#include "faults/FaultStructures.hpp"
#include "simulation/Value.hpp"

 /*
  * A control testpoint (both control-0 and control-1).
  *
  * @parameter _nodeType When new needs are created, what kind of node should be created?
  * @parameter _lineType When new lines are created, what kind of line should be created?
  * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
  */
template <class _nodeType, class _lineType, class _valueType>
class Testpoint_control : public Testpoint<_nodeType, _lineType, _valueType> {
public:
	/*
	 * Create a control testpoint with a given constant value.
	 *
	 * @param _locaiton The location of the testpoint.
	 * @param _value The value forced by the testpoint.
	 */
	Testpoint_control(_lineType* _location, _valueType _value) :
		Testpoint<_nodeType, _lineType, _valueType>(_location),
		Valued<_valueType>(nullptr, _value)
	{

	};

	/*
	 * Two control testpoints are equal if they are on the same location and
	 * the same value.
	 *
	 * @param _other The other Testpoint being compared to.
	 * @return True if both are on the same location.
	 */
	virtual bool operator == (const Testpoint_control<_nodeType, _lineType, _valueType>& _other) const {
		if (this->value() != _other.value()) {
			return false;
		}
		if (this->location_ != _other.location()) {
			return false;
		}
		return true;
	};

	/*
	 * See the overloaded operator "==", as this is the logical opposite.

	 * @param _other The other Testpoint being compared to.
	 * @return True if the Testpoints are different.
	 */
	virtual bool operator != (const Testpoint_control<_nodeType, _lineType, _valueType>& _other) const {
		return !(*this == _other);
	};

	/*
	 * Compaitor function (needed for sorted hashes of faults).
	 *
	 * The first "priotiry" of the sort is the pointer to the location.
	 *
	 * @param _other The other Testpoints being compared to.
	 */
	virtual bool operator < (const Testpoint_control<_nodeType, _lineType, _valueType>& _other) const {
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
	};

	/*
	 * Activate the Testpoint by modifying the circuit.
	 *
	 * The location will have its input removed and replaced by a constant-value
	 * holding Node (returned).
	 *
	 * @param _circuit The circuit which is modified (extra
	 *        nodes/pis/pos may be added/removed).
	 * @return A set of new events for simulation.
	 */
	virtual std::vector<Event<_valueType>> activate(Circuit* _circuit) {

		if (this->newNode_ != nullptr) {
			throw "Cannot activate control testpoint: already activated.";
		}
		std::string tpName = "tp_const_";
		tpName += this->value().magnitude() ? "1" : "0"; //NOTE: the name "const" is important for COP calculations
		this->newNode_ = new _nodeType(
			new ConstantFunction<_valueType>(this->value()),
			tpName
		);

		std::string name = this->location_->name() + "_pre-control-TP";//motify
		this->newLine_ = new _lineType(
			name
		);

		//float oldcc = this->location_->controllability();
		//float c = this->value().magnitude();
		//float newcc = (c + oldcc) / 2;
		//this->location_->clearControllability();
		//this->location_->controllability(newcc);
		//Remove the old driver connections.
		this->oldDrivers_ = this->location_->inputs();
		for (Connecting* oldDriver : this->oldDrivers_) {
			this->location_->removeInput(oldDriver);
			oldDriver->addOutput(this->newLine_);
		}

		//Make connections.
		this->newLine_->addOutput(this->newNode_);
		this->location_->addInput(this->newNode_);
		_circuit->addNode(this->newNode_);
		return this->newNode_->go();
	}

	/*
	 * Deactivate the Testpoint by modifying the circuit.
	 *
	 * Lines and Nodes will be deleted by this function.
	 *
	 * @param  _circuit The circuit which is modified (extra
	 *        nodes/pis/pos may be added/removed).
	 * @return A set of new events for simulation.
	 */
	virtual std::vector<Event<_valueType>> deactivate(Circuit* _circuit) {
		this->location_->removeInput(this->newNode_);

		for (Connecting* oldDriver : this->oldDrivers_) {
			oldDriver->removeOutput(this->newLine_);
			oldDriver->addOutput(this->location_);
		}
		this->oldDrivers_.clear();

		delete this->newLine_;
		this->newLine_ = nullptr;
		_circuit->removeNode(this->newNode_);

		delete this->newNode_->function(); //The function is unique to the node, so it must be deleted.
		this->newNode_ = nullptr;
		delete this->newNode_;
		
		return this->location_->go();

	};

private:
	/*
	 * The object(s) which was/were previously connected to before this TP was activated.
	 *
	 * Can be empty if not yet activated.
	 */
	std::unordered_set<Connecting*> oldDrivers_;

	/*
	 * The new line created by activating this testpoint.
	 *
	 * Can be null.
	 */
	_lineType* newLine_;

	/*
	 * The new node created to implement a constant value.
	 *
	 * This will feed the controlled line as well as be fed by the original driver
	 * (which will force it's observability to 0 if needed).
	 */
	_nodeType* newNode_ = nullptr;


};

#endif