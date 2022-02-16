/**
 * @file Objective.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-12-19
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef DFrontier_h
#define DFrontier_h


#include "Fault.hpp"
#include "Goal.hpp"
#include "Tracer.hpp"
#include "SATStructures.hpp"

/*
 * A D-Frontier is a type of "Goal" used during SAT. In effect, this turns SAT
 * into an ATPG algorithm, with the specific ATPG being dictated by the type of
 * Backtracer algorithm inside SAT.
 *
 * Besides the original Goal methods (which are modified for this new type of
 * Goal), additional functions allow an attached Fault to be manipulated. For
 * ATPG, mutiple Goals can be used with each goal having a different Fault. 
 * The user is responsible for (de)activating the faults before/after SAT. It 
 * is wise to use an "ATPG" class which is NOT a child of SAT, but instead acts 
 * as a "wrapper" around SAT and manages Fault states during ATPG. This ATPG 
 * object can also be useful if the specific test vector associated with a 
 * Fault is needed.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue.
 */
template <class _valueType>
class DFrontier : public Goal<_valueType> {
public:
	/*
	 * Create a DFrontier for a given fault.
	 *
	 * @param _fault The fault to create the DFrontier for.
	 */
	DFrontier(Fault<_valueType>* _fault) {
		this->fault_ = _fault;
	}

	/*
	 * Is the Goal satisfied (i.e., successful)?
	 * 
	 * For a DFrontier, this returns true if any D is seen at a circuit PO. This
	 * is determined by tracing forward from the Fault location to create the
	 * (D-)frontier and to see if it lands on any item named "po".
	 *
	 * The (D-)frotier will always be recalculated when this method is called,
	 * but the internal stored value may not be the exact "D-frontier". Future
	 * methods (impossible() and frontier()) may modify the returned result. It
	 * is presumed that impossible() will always be called after this before
	 * calling frontier().
	 *
	 * @return True if the goal is satisfied.
	 */
#define NON_D_VALUES _valueType(0x0), _valueType(0xFFFFFFFFFFFFFFF), _valueType(), _valueType(_valueType(), _valueType(0xFFFFFFFFFFFFFFF)), _valueType(_valueType(), _valueType(0x0)), _valueType(_valueType(0xFFFFFFFFFFFFFFF), _valueType()), _valueType(_valueType(0x0), _valueType())
	virtual bool success() {
		//Starting from the fault's location, trace forward until a non-D value
		//is found and return the "last D".
		this->frontier_.clear();
		Tracable<_valueType>* faultLocation = dynamic_cast<Tracable<_valueType>*>(this->fault_->location());
		std::unordered_set<Tracable<_valueType>*> dLocations = 
			Tracer<Tracable<_valueType>, _valueType>::trace(
				std::unordered_set<Tracable<_valueType>*>({ faultLocation }),
				true, //Forward = true
				{"po"}, //Return names
				{}, //Return values
				{}, //Backup names
				{ NON_D_VALUES } //Backup values
		);
		for (Tracable<_valueType>* dLocation : dLocations) {
			if (dLocation->name() == "po") {
				return true; //Success!
			}
			_valueType value = dLocation->value();
			this->frontier_.push_back(
				std::pair<Evented<_valueType>*, _valueType>(
					dynamic_cast<Evented<_valueType>*>(dLocation),
					dLocation->value()
					)
			);
		}
		return false;
	}

	

	/*
	 * Is the Goal impossible (i.e., a backtrace is needed)?
	 *
	 * For a DFrontier, this returns true if the (D-)frontier is empty. This 
	 * occurs if there is no item which has a D (i.e., an active faulty and valid
	 * bit) input and it at an X value.
	 *
	 * The (D-)frontier will be recalculated. If there is a valid (D-)frontier,
	 * it will be saved (and later returned) as all D/U values desired on
	 * gates which currently have an X-output.
	 *
	 * @return True if the goal is not possible
	 */
	virtual bool impossible() {
		//At this point, the "frontier_" contains one of two things:
		// 1) The location of the most forward D-values, which implies the fault is
		//    excited. We'll see what X's may be changed, and while doing so, 
		//    we'll assign which D/U values we want to see.
		// 2) Nothing: the fault is not excited. We need to find out why.
		if (this->isNotExcited()) {
			if (this->frontier_.size() == 0) { //If frontier is still empty, the fault cannot be excited.
				return true;
			}
			return false; //The frontier has been updated to excite the fault.
		}

		//The frontier contains the most-forward D values. We'll try to convert
		//these D locations into possible gate inputs (which are X) to set.
		std::vector<std::pair<Evented<_valueType>*, _valueType>> newFrontier;
		for (std::pair<Evented<_valueType>*, _valueType> front : this->frontier_) {
			Evented<_valueType>* location = front.first;
			for (Connecting* output : location->outputs()) {
				Evented<_valueType>* cast = dynamic_cast<Evented<_valueType>*>(output);
				if (cast->value().valid() == 0x0 ) { //we have an X: it's possible.
					std::vector<std::pair<Evented<_valueType>*, _valueType>> toAdd = this->propagatingChoices(cast);
					newFrontier.insert(newFrontier.end(), toAdd.begin(), toAdd.end());
				}
			}
		}
		this->frontier_ = newFrontier;
		return this->frontier_.size() == 0;
	}

	/*
	 * Return a "frontier" of possible values to set.
	 *
	 * NOTE: by this definition, the "frontier" are the Values to SET.
	 *
	 * For a DFrontier, this returns the lines which are currnetly "X" but have
	 * a D as an input.
	 *
	 * No calculation needs to be done, since the last "impossible()" call will
	 * save the frontier.
	 *
	 * @return A list of location - Value pairs corresponding to choices which
	 *         may satisfy this Goal.
	 */
	virtual std::vector<std::pair<Evented<_valueType>*, _valueType>> frontier() {
		return this->frontier_;
	}

private:
	/*
	 * For a given gate driven by a D value, return a frontier of X inputs and
	 * values to set them to.
	 *
	 * @param _gate A gate driven by a D value.
	 * @param A frontier of locations with given values.
	 */
	std::vector<std::pair<Evented<_valueType>*, _valueType>> propagatingChoices(Connecting* _gate) {
		std::vector<std::pair<Evented<_valueType>*, _valueType>> toReturn;
		for (Connecting* input : _gate->inputs()) {
			Evented<_valueType>* cast = dynamic_cast<Evented<_valueType>*>(input);
			_valueType value = this->propagatingValue(_gate->name());
			if (cast->value().valid() == 0x0)
			{
				toReturn.push_back(
					std::pair<Evented<_valueType>*, _valueType>(cast, value)
				);
			}
		}
		return toReturn;
	}

	/*
	 * Check if a fault is not excited, and if it is not, set the frontier to 
	 * excite it.
	 *
	 * After running this function, the frontier_ may be changed.
	 *
	 * @return True if the fault is not excited and it is impossible to excite.
	 */
	bool isNotExcited() {
		if (this->frontier_.size() > 0) {//This is only possible if there's a D
			                             //on the fault location after
			                             //success().
			return false;
		}
		//Check to see if the fault's location has a valid value (in the fault-free circuit)
		if (this->fault_->location()->value().originalFrame().valid()) {//yes
			return true; //And the frontier will be empty, so a backtrack is needed.
		}

		//Therefore, make the frontier the fault's location with oposite value 
		//of the fault in the original (fault-free) circuit.
		this->frontier_.push_back(
			std::pair<Evented<_valueType>*, _valueType>(
				dynamic_cast<Evented<_valueType>*>(*(this->fault_->location()->inputs().begin())),
				_valueType(
					~this->fault_->value(),
					~this->fault_->value()
					)
				)
		);
		return true;
	}

	/*
	 * For a given gate type (by name), what input value is requried to
	 * propagate a value through.
	 *
	 * @param _name The name of the function.
	 * @return The value which propagates a value through.
	 */
	_valueType propagatingValue(std::string _name) {
		if (
			_name == "nand" ||
			_name == "and" ||
			_name == "xor" ||
			_name == "or"
			) {
			return _valueType(0xFFFFFFFFFFFFFFFF);
		}
		else if (
			_name == "or" ||
			_name == "nor" 
			) {
			return _valueType(0x0);
		}
		throw "This doesn't make sense: there's no gate to propigate through.";
		return _valueType();
	}

	/*
	 * The fault associated with this DFrontier
	 */
	Fault<_valueType>* fault_;

	/*
	 * The current "frontier". The different checking calls (success, impossible
	 * and frontier) will change this value. It is presumed that these calls
	 * will be made int he following order.
	 *
	 * 1) success(): the current "frontier" state will be ignored and recreated.
	 *    Starting from the fault location, tracing will stop at the last "D/U"
	 *    value. The result will be saved as the current "frontier".
	 * 2) impossible(): starting from the current "frontier", forward tracing
	 *    will be done to find the first "X" value. Before returning and saving,
	 *    this these X values will be replaced with D/U values needed to replace
	 *    the X values.
	 * 3) frontier(): no calculation will be done. The frontier saved by
	 *    impossible() will be returned.
	 */
	std::vector<std::pair<Evented<_valueType>*, _valueType>> frontier_;

};

#endif
