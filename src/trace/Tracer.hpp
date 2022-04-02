/**
 * @file Tracer.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-05-17
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef Tracer_h
#define Tracer_h

#include <set>

#include "trace/Tracable.hpp"

#define ALL_CONDITIONS _returnNames, _returnValues, _backupNames, _backupValues, _stopNames, _stopValues

/*
 * A tracer is able to trace forward and backwards in a circuit and return the
 * requested elements.
 *
 * This class is purely static and therefore has no constructors.
 *
 * All public classes require a vector of one or more locations/values to trace
 * on. If a single location/value is desired, then pass the input as a vector
 * of one (e.g., {location}).
 *
 * NOTE: Items to trace must be of the "Tracable" type: this adds flags and
 *       requires items to hold a value (for value-based tracing).
 *
 * @param _objType The type of object being traced. The object type must inherit
 *        from "Tracable".
 * @param  _valueType The type of value being simulated, e.g., 
 *        Value/FaultyValue. 
 */
template <class _objType, class _valueType> 
class Tracer {
public:

	/*
	 * The trace function returns a list points based on your given criteria.
	 * These points fall into one of three categories:
	 *  1) Return: when the condition is seen, add the point to the return list.
	 *  2) Backup: when the condition is seen, return the previous element.
	 *  3) Stop: when the condition is seen, do not trace further but do NOT add
	 *     the element to the list.
	 *
	 * For these three conditions, two types of checks can be performed, which
	 * creates six possible checking conditions:
	 *  1) Check by name: check if a location in the trace matches the name in a
	 *     given list. This is useful for stopping on a given function (e.g., 
	 *     stopping on PIs and POs.
	 *  2) Check by value: check if a location matches a given value.
	 *
	 */
	static std::unordered_set<_objType*> trace(
		std::unordered_set<_objType*> _locations,
		bool _forward,

		std::set<std::string> _returnNames = {},
		std::set<_valueType> _returnValues = {},
		std::set<std::string> _backupNames = {},
		std::set<_valueType> _backupValues = {},
		std::set<std::string> _stopNames = { "pi", "po", "const" },
		std::set<_valueType> _stopValues = {}
	) {
		std::unordered_set<_objType*>  flagged;
		std::unordered_set<_objType*>  toReturn;
		for (_objType* location : _locations) {
			std::unordered_set<_objType*> newPoints =
				trace(location, nullptr, _forward, flagged, ALL_CONDITIONS);
			toReturn.insert(newPoints.begin(), newPoints.end());
		}

		cleanUp(flagged);
		return toReturn;
	}

private:

	/*
	 * Cast a set of connecting objects to a given object type.
	 *
	 * @param _toCast The object ot cast.
	 * @return The same set of objects cast as the templated object type.
	 */
	static std::unordered_set<_objType*> castToObjType(std::unordered_set<Connecting*> _toCast) {
		std::unordered_set<_objType*> toReturn;
		for (Connecting* item : _toCast) {
			toReturn.emplace(dynamic_cast<_objType*>(item));
		}
		return toReturn;
	}

	/*
	 * For a given location and direction, get the next locations to trace on.
	 *
	 * An exception will be thrown if there are no more points to trace on.
	 *
	 * @param _location The location to go forwards/backwards on.
	 * @param _forwards Are we going forwards? (If not, we're going backwards).
	 * @return The next points to trace on.
	 */
	static std::unordered_set<_objType*> nextLocations(_objType* _location, bool _forwards) {
		std::unordered_set<_objType*> nextPointsInTheTrace = castToObjType(
			_forwards == true ?
			_location->outputs() : //Forwards = outputs
			_location->inputs() //Backwards = inputs
		);
		if (nextPointsInTheTrace.size() == 0) {
			throw "Trace failure: no endpoint could be reached.";
		}
		return nextPointsInTheTrace;
	}

	/*
	 * For a given set of names and values to check for, check if any of them
	 * are true for the given location.
	 *
	 * @param _location The location where the conditions are checked for.
	 * @param _names The names to check for on the given location.
	 * @param _values The values to check for on the given location.
	 * @return True if any of the names are values are found.
	 */
	static bool conditionPoint(_objType* _location, std::set<std::string> _names, std::set<_valueType> _values) {
		if (_names.count(_location->name()) != 0) {
			return true;
		}
//DEBUG		_valueType value = _location->value();
		if (_values.count(_location->value()) != 0) {
			return true;
		}
		return false;
	}


	/*
	 * The single-location trace function.
	 *
	 * (see mutiple-location trace function for parameter definitions, since all
	 * parameters are the same exceptiont the _location is a single value and a
	 * "backup" location is given).
	 */
	static std::unordered_set<_objType*> trace(
		_objType* _location, 
		_objType* _backup,
		bool _forward,
		std::unordered_set<_objType*> & _flagged,

		std::set<std::string> _returnNames = {},
		std::set<_valueType> _returnValues = {},
		std::set<std::string> _backupNames = {},
		std::set<_valueType> _backupValues = {},
		std::set<std::string> _stopNames = {"pi", "po", "const"},
		std::set<_valueType> _stopValues = {}
	) {
		if (_location->flag(true) == true)
		{
			return std::unordered_set<_objType*>();
		}
		_flagged.emplace(_location);
	
		if (conditionPoint(_location, _returnNames, _returnValues) == true) {
			//Return! Return this point.
			return std::unordered_set<_objType*>({ _location });
		}
		if (conditionPoint(_location, _backupNames, _backupValues) == true) {
			//Backup! Return the backup object unless it is nullptr.
			if (_backup == nullptr) {
				return std::unordered_set<_objType*>();
			} else {
				return std::unordered_set<_objType*>({ _backup });
			}
		}
		if (conditionPoint(_location, _stopNames, _stopValues) == true) {
			//Stop or flagged! Return nothing.
			return std::unordered_set<_objType*>();
		}
		//No conditions met, so continue...
		//Get all locations in the given directions.
		std::unordered_set<_objType*> toReturn;
		std::unordered_set<_objType*> nextPointsInTheTrace = nextLocations(_location, _forward);
		for (_objType* point: nextPointsInTheTrace) {
			std::unordered_set<_objType*> newPoints =
				trace(point, _location, _forward, _flagged, ALL_CONDITIONS);
			toReturn.insert(newPoints.begin(), newPoints.end());
		}
		return toReturn;
	}


	/*
	 * Clean up a list of visited elements.
	 *
	 * @param _visited The list of visited elements.
	 */
	static void cleanUp(std::unordered_set<_objType*> _visited) {
		for (_objType* visited : _visited) {
			if (visited->flag(false) == false) {
				throw "During trace clean-up, this should be impossible.";
			}
		}
	}
};
 

#endif