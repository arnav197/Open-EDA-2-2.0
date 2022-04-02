/**
 * @file EventQueue.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-18
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef EventQueue_h
#define EventQueue_h

#include <utility>
#include <vector>
#include <unordered_set>
#include <set>

#include "simulation/Value.hpp"
#include "Level.h"

//Forward declaration: Event <-> Evented are cross dependent.
template <class _valueType>
class Evented;

/*
 * Events are the entries which make up an event queue. Each event contains 1)
 * an evented object which needs to be updated and 2) the prioirity
 * (e.g., the input level in a circuit) of the event relative to other events.
 *
 * Events can be "invalid" or "null" if the location of the Event is a nullptr.
 * However, this should not be explicitly checked. Instead, "isValid()" should
 * be called.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class Event {
public:
	/*
	 * Create a default (invalid/null) event
	 */
	Event() {
		this->location_ = nullptr;
		this->priority_ = 0;
	}

	/*
	 * Create an event on a given locaiton with a given prioirty.
	 *
	 * @param _priority The priority of the event.
	 * @param _location The location of the event.
	 */
	Event(size_t _priority, Evented<_valueType>* _location) {
		this->priority_ = _priority;
		this->location_ = _location;
	}

	/*
	 * Accessor: get the location of the event.
	 *
	 * @return The location of the event.
	 */
	Evented<_valueType>* location() {
		return this->location_;
	}

	/*
	 * Accessor: get the priority of the event.
	 *
	 * @return The priority of the Event.
	 */
	size_t priority() {
		return this->priority_;
	}

	/*
	 * Is this an invalid/null event?
	 *
	 * @return True if this Event is invalid/null.
	 */
	bool isInvalid() {
		return this->location == nullptr;
	}

private:
	/*
	 * The location of the event.
	 */
	Evented<_valueType>* location_;

	/*
	 * The priority of the event.
	 */
	size_t priority_;

};

/*
 * An object which can be called to do an "event".
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class Evented : public virtual Valued<_valueType>, virtual public Levelized {
public:
	/*
	 * Create an object which are connected to other Levelized objects.
	 *
	 * @param (optional) _inputs A vector of input Levelized objects.
	 * @param (optioanl) _outputs A vector of output Levelized objects.
	 * @param (optional) _name Name of the connection (defaults to empty strying).
	 * @param (optional) _function The Value-evaluating Functino of this object.
	 */
	Evented(
		std::unordered_set<Evented<_valueType>*> _inputs = std::unordered_set<Evented<_valueType>*>(),
		std::unordered_set<Evented<_valueType>*> _outputs = std::unordered_set<Evented<_valueType>*>(),
		std::string _name = "",
		Function<_valueType>* _function = nullptr
	);

	/*
	 * Activate the event associated with this object and fetch subsequent events.
	 *
	 * An optional override of values to base this event on can be given.
	 *
	 * @param (optional) _values Values used to evaluate this event.
	 * @return New events (and their priority) created by activating this event.
	 */
	virtual std::vector<Event<_valueType>> go(
		std::vector<_valueType> _values = std::vector<_valueType>()
	);

	/*
	 * Create an Event from this object.
	 *
	 * By default, the "priority" of an object is its "input level".
	 *
	 * @return An event corresponding to this object.
	 */
	Event<_valueType> selfAsEvent() {
		return Event<_valueType>(this->inputLevel(), this);
	}
};

/*
 * A queue which contains and manages events.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class EventQueue {
public:
	/*
	 * A blank queue.
	 */
	EventQueue();

	/*
	 * Add an evented object to the queue.
	 *
	 * @param The "level" of the Evented object.
	 * @param The Evented object.
	 */
	void add(Event<_valueType>);

	/*
	 * Manually add a list of events to the queue.
	 *
	 * @param The events (and their level) to add to the queue.
	 */
	void add(std::vector<Event<_valueType>> _add);

	/*
	 * Process all elements in the queue.
	 *
	 * If during processing an event is added which has a "level" less than or
	 * equal to the event being processed, an exception will be thrown.
	 *
	 * @return false if there are no events to process.
	 */
	bool process();

	

private:
	/*
	 * Populate the queue with "empty events" for a given number of levels.
	 *
	 * @param _levels The number of priority levels to populate.
	 */
	void populateLevels(size_t _levels);

	/*
	 * The event-containing data structures.
	 *
	 * Each "index" of the vector is a "priority level".
	 * Each "index entry" is a set of Events.
	 */
	std::vector<std::vector<Evented<_valueType>*>> queue_;

	/*
	 * The current maximum level of the queue.
	 */
	size_t maxLevel_;
};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////

template <class _valueType>
Evented<_valueType>::Evented(
	std::unordered_set<Evented<_valueType>*> _inputs,
	std::unordered_set<Evented<_valueType>*> _outputs,
	std::string _name,
	Function<_valueType>* _function) :
	Levelized(
		std::unordered_set<Levelized*>(_inputs.begin(), _inputs.end()),
		std::unordered_set<Levelized*>(_outputs.begin(), _outputs.end()),
		_name
	),
	Valued<_valueType>(_function)
{
}

/*
 * For a given Evented object, get its current input values.
 *
 * @param  An Evented object.
 * @return The vector of current input values to the Evented object.
 */
template <class _valueType>
std::vector<_valueType> inputValues(Evented<_valueType>* _obj) {
	std::vector <_valueType> values;
	for (Connecting* input : _obj->inputs()) {
		Evented<_valueType>* cast = dynamic_cast<Evented<_valueType>*>(input);
		values.push_back(cast->value());
	}
	return values;
}

template <class _valueType>
std::vector<Event<_valueType>> Evented<_valueType>::go(
	std::vector <_valueType> _values
) {
	if (_values.size() == 0) {//If input values are not given, get them.
		_values = inputValues<_valueType>(this);
	}

	_valueType oldValue = this->value();
	_valueType newValue = ~oldValue;
	
	if (_values.size() != 0) { //This is a "PI trick": this following function 
		                       //will throw an exception if this a PI. By having
		                       //the default newValue be the opposite of the old
		                       //value, a PI will always ask its outputs to be
		                       //re-evaluted.
		newValue = this->value(_values);
	}
	std::vector<Event<_valueType>> toReturn;
	if (oldValue != newValue) { //Value changed, so change line values and update the queue.
		for (Connecting* output : this->outputs()) {
			Evented<_valueType>* cast = dynamic_cast<Evented<_valueType>*>(output);
			toReturn.push_back( cast->selfAsEvent() );
		}
	}

	return toReturn; //May be empty if nothing is added.
}

template <class _valueType>
EventQueue<_valueType>::EventQueue() {
	this->maxLevel_ = 0;
}

template<class _valueType>
inline void EventQueue<_valueType>::populateLevels(size_t _levels) {
	if (this->queue_.empty() == true) { //Always populate a "zero level" if needed.
		this->queue_.push_back(std::vector<Evented<_valueType>*>());
	}
	if (_levels > this->maxLevel_) {
		size_t currentMaxLevel = this->queue_.size();
		for (size_t i = currentMaxLevel; i <= _levels; i++) { //These are the levels which are missing.
			this->queue_.push_back(std::vector<Evented<_valueType>*>());
		}
		this->maxLevel_ = _levels;
	}
}

template <class _valueType>
void EventQueue<_valueType>::add(Event<_valueType> _event) {
	this->populateLevels(_event.priority()); //Queue levels may be populated.
	this->queue_.at(_event.priority()).push_back(_event.location());
}

template <class _valueType>
void EventQueue<_valueType>::add(std::vector<Event<_valueType>> _add) {
	for (Event<_valueType> newEvent : _add) {
		this->add(newEvent);
	}
}

template <class _valueType>
bool EventQueue<_valueType>::process() {
	if (this->queue_.size() == 0) {//The loop below does not handle "empty queue" gracefully.
		return false;
	}

	for (size_t currentLevel = 0; currentLevel <= maxLevel_; currentLevel++) {
		std::vector<Evented<_valueType>*> events = this->queue_.at(currentLevel);
		#pragma omp parallel for
		for (int eventNum = 0; eventNum < events.size(); eventNum++) {
			Evented<_valueType>* currentEvent = events.at(eventNum);
			std::vector<Event<_valueType>> newEvents = currentEvent->go();
			for (Event<_valueType> toAdd : newEvents) {
				if (toAdd.priority() <= currentLevel) {
					throw "EventQueue can only add events during processing 'in order'.";
				}
				#pragma omp critical 
				this->add(toAdd);
			}
		}
	}
	this->queue_.clear();
	this->maxLevel_ = 0;
	return true;
}



#endif