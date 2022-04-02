/**
 * @file Testpoint_invert.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-02-19
 *
 * @copyright Copyright (c) 2019
 *
 */


#ifndef Testpoint_invert_h
#define Testpoint_invert_h

#include "tpi/Testpoint.hpp"

/*
 * A control testpoint (both control-0 and control-1).
 *
 * @parameter _nodeType When new needs are created, what kind of node should be created?
 * @parameter _lineType When new lines are created, what kind of line should be created?
 * @parameter _valueType The type of value to be simulated. Example: Value/FaultyValue
 */
template <class _nodeType, class _lineType, class _valueType>
class Testpoint_invert : public Testpoint<_nodeType, _lineType, _valueType> {
public:
	/*
	 * Create a testpoint with an inverstion on the given line.
	 *
	 * @param _locaiton The location of the testpoint.
	 * @param (ignored) _value
	 */
	Testpoint_invert(_lineType* _location, _valueType _value = _valueType()) :
		Testpoint<_nodeType, _lineType, _valueType>(_location) {
		this->newLine_ = nullptr;
		this->newNode_ = nullptr;
	}

	/*
	 * Activate the Testpoint by modifying the circuit.
	 *
	 * The location will have its driver replaced by an inverter (which will be
	 * driven by the old driver).
	 *
	 * An exception will be thrown if already active.
	 *
	 * An exception will be thrown if the TP's location has no driver.
	 *
	 * NOTE: a new line will also be created by this function, but it is NOT
	 *       returned.
	 *
	 * Lines and Nodes may be created by this function.
	 *
	 * @param _circuit The circuit which is modified (extra
	 *        nodes/pis/pos may be added/removed).
	 * @return A set of new events for simulation.
	 */
	virtual std::vector<Event<_valueType>> activate(Circuit* _circuit) {
		if (this->oldDrivers_.size() != 0 || this->newLine_ != nullptr || this->newNode_ != nullptr) {
			throw "This inversion TP is already active.";
		}

		//Create the new node and new line.
		this->newNode_ = new _nodeType(
			new BooleanFunction<_valueType>("not"),
			"TP_INVERT"
			);
		std::string name = this->location_->name() + "_pre-invert-TP";
		this->newLine_ = new _lineType(
			name
			);

		this->oldDrivers_ = this->location_->inputs();
		if (this->oldDrivers_.size() == 0) {
			throw "Cannot add inverstion TP to something which has no driver.";
		}

		//Remove the old driver connections.
		for (Connecting* oldDriver : this->oldDrivers_) {
			this->location_->removeInput(oldDriver);
			oldDriver->addOutput(this->newLine_);
		}
		
		//Make connections.
		this->newLine_->addOutput(this->newNode_);
		this->location_->addInput(this->newNode_);
		//_circuit->addNode(this->newNode_);

		return this->newNode_->go();
	};

	/*
	 * Deactivate the Testpoint by modifying the circuit.
	 *
	 * An exception will be thrown if the node is not activated.
	 *
	 * NOTE: the "new line" created during activation will be DELETED by this 
	 *       function.
	 *
	 * @param  _circuit The circuit which is modified (extra
	 *        nodes/pis/pos may be added/removed).
	 * @return A set of new events for simulation.
	 */
	virtual std::vector<Event<_valueType>> deactivate(Circuit* _circuit) {
		if (this->oldDrivers_.size() == 0 || this->newLine_ == nullptr || this->newNode_ == nullptr) {
			throw "This inversion TP is not active: it cannot be deactivated.";
		}

		for (Connecting* oldDriver : this->oldDrivers_) {
			oldDriver->addOutput(this->location_);
			oldDriver->removeOutput(this->newLine_);
		}
		this->oldDrivers_.clear();

		this->location_->removeInput(newNode_);
		delete this->newLine_;
		this->newLine_ = nullptr;

		_nodeType* toReturn = this->newNode_;
		//_circuit->removeNode(toReturn);

		delete this->newNode_->function(); //The inversion for all inversion TPs is unique to the TP.
		delete this->newNode_;
		this->newNode_ = nullptr;
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
	 */
	_nodeType* newNode_;
};

#endif
