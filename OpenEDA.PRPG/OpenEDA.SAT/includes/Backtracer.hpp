/**
 * @file BackTrace.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-12-18
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef backtrace_h
#define backtrace_h

#include <random>
#include <algorithm>
#include <iostream>
#include <stack>

#include "ImplicationStack.hpp"

/*
 * A backtracer takes one (or more) lines and values in a circuit and will 
 * return 1) a PI assignment which attempts to make the values occur and 2) a
 * list of implied assignments made inside the circuit.
 *
 * PI assignments and implications are returned in the form of AlternativeEvents
 * used in an ImplicationStack.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class Backtracer {
public:
	/*
	 * For a given set of possible locations and values (with no other context),
	 * choose the best direction for backtracing.
	 *
	 * @param _frontier A vector of location-value pairs desired.
	 * @return A vector of implications made in an attempt to satisfy a single
	 *         entry in the frontier.
	 */
	virtual std::vector<AlternativeEvent<_valueType>> backtrace(
		std::vector<std::pair<Evented<_valueType>*, _valueType>> _frontier
	) {
		std::pair<Connecting*, _valueType> backtraceDecision = this->backtraceDirection(_frontier);
		std::vector<AlternativeEvent<_valueType>> throwAway;
		return this->backtrace(backtraceDecision.first, backtraceDecision.second,
							   throwAway
		);
	}

	/*
	 * For a given location and a given value, add to the given vector of events
	 * the implications (and the final assignment) need to make the location and
	 * value occur.
	 *
	 * It is presumed that the location being at the given value is not yet on
	 * provided stack, so this method will add the location/value to the stack.
	 *
	 * NOTE: behavior is undefined for non-valid values.
	 *
	 * @param _location The location of the value.
	 * @param _value    The desired value at the location.
	 * @param (By reference) A vector of previous implications made. "0" is the
	 *                  the first implication while the last is the final PI
	 *                  assignment.
	 * @param A vector of previous implications (the same implications passed by
	 *        reference.
	 */
	virtual std::vector<AlternativeEvent<_valueType>> backtrace(
		Connecting* _location, //Technically, we don't need to keep track of anything besdies connections and functions (and functions are found through a connecting object's name).
		_valueType _value,
		std::vector<AlternativeEvent<_valueType>> & _implications //On first use, pass an empty vector.
	) {
		//First, put the location on the stack (and end if it's a PI.
		if (_location->name() == "pi") { //This is a pi, so this is the end of line. We set the value and finish the implication stack.
			Evented<_valueType>* cast = dynamic_cast<Evented<_valueType>*>(_location);
			cast->value({_value});
			_implications.push_back(
				AlternativeEvent<_valueType>(
					dynamic_cast<Evented<_valueType>*>(_location), 
					~_value) //The alternative has the opposite magnitude.
			);
			return _implications; //At a PI: return.
		} else {
			_implications.push_back(
				AlternativeEvent<_valueType>(
					dynamic_cast<Evented<_valueType>*>(_location)
					)  //Not a PI, so push back a "no alternative" event.
			);
		}

		//Depending on the function of this location, the value we want may flip.
		_value = this->doesFunctionFlipValue(_location->name(), _value);
		//Choose the direction of backtracing.
		std::pair<Connecting*, _valueType> backtraceDecision = this->backtraceDirection(_location->inputs(), _value);
		if (backtraceDecision.first == nullptr) {throw "Could not choose a backtracking direction.";}

		return this->backtrace(backtraceDecision.first, backtraceDecision.second, _implications);
	}

private:
	/*
	 * Depending on the function we're passing through, the value we seek may
	 * flip. This function will flip the value if necessary based on a 
	 * function's name.
	 *
	 * @param _functionName The function's name.
	 * @param _value The value we want on the output of a function.
	 * @return The value we want on the input of a funciton.
	 */
	_valueType doesFunctionFlipValue(std::string _name, _valueType _value) {
		if (
			_name == "nand" ||
			_name == "nor" ||
			_name == "not"
			) {
			return ~_value;
		}
		return _value;
	}

	/*
	 * Given a set of locations, choose the direction of backtracing.
	 *
	 * NOTE: this is a convenience function: the call is forwarded to the 
	 *       "frontier version".
	 *
	 * @param _locations A list of possible locations for backtracing.
	 * @param _value The desired value at all locations.
	 * @return The new location (nullptr if there is no possible direction.
	 */
	std::pair<Connecting*, _valueType> backtraceDirection(std::unordered_set<Connecting*> _locations, _valueType value) {
		std::vector<std::pair<Evented<_valueType>*, _valueType>> toForward;
		for (Connecting* location : _locations) {
			toForward.push_back(
				std::pair<Evented<_valueType>*, _valueType>(
					dynamic_cast<Evented<_valueType>*>(location),
					value
				)
			);
		}
		return this->backtraceDirection(toForward);
	}

	/*
	 * Given a set of locations with a given set of desired values, choose the 
	 * direction of backtracing.
	 *
	 * @param _frontier The desired location-value pairs
	 * @param _value The desired value at all locations.
	 * @return The new location (nullptr if there is no possible direction.
	 */
	std::pair<Connecting*, _valueType> backtraceDirection(std::vector<std::pair<Evented<_valueType>*, _valueType>> _frontier) {
		//Our algorithm is simple: return the first location without a known value.
		for (std::pair<Evented<_valueType>*, _valueType> entry : _frontier) {
			if (entry.first == nullptr) {
				throw "Big problem here...";
			}
			if (entry.first->value().valid() == 0x0) { //Our algorithm is simple: return the first location without a known value.
				return std::pair<Connecting*, _valueType>(entry.first, entry.second);
			}
		}
		throw "Fail to backtrace: no possible direction.";
		return std::pair<Connecting*, _valueType>(nullptr, _valueType());
	}

};

#endif