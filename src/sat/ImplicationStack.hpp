/**
 * @file ImplicationStack.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-12-27
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef stack_h
#define stack_h

#include <stack>
#include <vector>

#include "simulation/EventQueue.hpp"
#include "simulation/Value.hpp"

/*
 * AlternativeEvents are extensions to EventQueue events (no functions are
 * overwritten). They allow possible "backtrace" (i.e., undo) values to be
 * tracked and they allow alternative values to be tracked. These additional
 * values are optional, and an AlternativeEvent can say if one of these events
 * (does not) exist.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue.
 */
template<class _valueType>
class AlternativeEvent {
public:
	/*
	 * By default, an "invalid event" which has no location (i.e., nullptr) and
	 * no backtraces/alternatives will be created.
	 */
	AlternativeEvent() : Event<_valueType>(){
		this->location_ = nullptr;
		this->backtrackValue_ = _valueType();
		this->backtrackPossible_ = false;
		this->alternativeValue_ = _valueType();;
		this->alternativePossible_ = false;
	}

	/*
	 * An AlternativeEvent created from a regular event is presumed to have no
	 * backtrack or alternative.
	 *
	 * @param The regular Event.
	 */
	AlternativeEvent(Evented<_valueType>* _location) {
		if (_location == nullptr) {
			throw "Cannot create alternative event from invalid location.";
		}
		this->location_ = _location;
		this->backtrackValue_ = _valueType();
		this->backtrackPossible_ = false;
		this->alternativeValue_ = _valueType();;
		this->alternativePossible_ = false;
	}

	/*
	 * There is no practial prupose to not having BOTH a backtrack and an 
	 * alternative, so there is no "single option" constrctor.
	 *
	 * @param _event The regular Event.
	 * @param _alternativeValue The alternative value.
	 * @param (optional) _backtrackValue the Backtrack value (by default "X").
	 */
	AlternativeEvent(
		Evented<_valueType>* _location,
		_valueType _alternativeValue,
		_valueType _backtrackValue = _valueType()
	) {
		if (_location == nullptr) {
			throw "Cannot create alternative event from invalid location.";
		}
		this->location_ = _location;
		this->backtrackValue_ = _backtrackValue;
		this->backtrackPossible_ = true;
		this->alternativeValue_ = _alternativeValue;
		this->alternativePossible_ = true;
	}

	/*
	 * Return true if there is an alternative assignment which can be made.
	 *
	 * @return True if an alternative assigment can be made, else false.
	 */
	bool isAlternativeAvailable() {
		return this->alternativePossible_;
	}

	/*
	 * Return true if there is a backtrack assignment which can be made.
	 *
	 * @return True if an backtrack assigment can be made, else false.
	 */
	bool isBacktrackAvailable() {
		return this->backtrackPossible_;
	}

	/*
	 * Make the alternative assignment and return a new serries of events which
	 * need to be evaluated. Additionally, mark the alternative assignment as
	 * invalid.
	 *
	 * @return A new series of events which need to be evaluted.
	 */
	std::vector<Event<_valueType>> alternative() {
		if (this->alternativePossible_ == false) {
			throw "Cannot make alternative assigment when none is available.";
		}
		this->alternativePossible_ = false;
		return this->location_->go(std::vector<_valueType>({this->alternativeValue_}));
	}

	/*
	 * Make the backtracking assignment and return a new serries of events which
	 * need to be evaluated. Additionally, mark the backtracking assignment as
	 * invalid.
	 *
	 * @return A new series of events which need to be evaluted.
	 */
	std::vector<Event<_valueType>> backtrack() {
		if (this->backtrackPossible_ == false) {
			throw "Cannot make backtrack assigment when none is available.";
		}
		this->backtrackPossible_ = false;
		return this->location_->go(std::vector<_valueType>({ this->backtrackValue_ }));
	}

	/*
	 * Accessor: return the location of the Event.
	 *
	 * @return The location of the Event.
	 */
	Evented<_valueType>* location() {
		return this->location_;
	}

private:
	/*
	 * The location of the assignments.
	 */
	Evented<_valueType>* location_;

	/*
	 * The backtrack (undo) value of this event, and can this event be backtracked.
	 */
	_valueType backtrackValue_;
	bool backtrackPossible_;

	/*
	 * The alternative assignment of the event, and is an alternative assignment possible.
	 */
	_valueType alternativeValue_;
	bool alternativePossible_;
};

/*
 * An implication stack stores decisions histories, which includes intermediate
 * and final decisions. Using this, final decisions (and all corresponding
 * intermediates) can be undone when alternatives to alternative decisions are
 * given.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue.
 */
template <class _valueType>
class ImplicationStack {
public:
	/*
	 * Add a given list of AlternativeEvents (in 0->end order).
	 *
	 * This method adds events one-by-one using its own single-Event method.
	 *
	 * @return An event queue which needs to be evaluated after backtracking.
	 */
	EventQueue<_valueType> add(std::vector<AlternativeEvent<_valueType>> _events) {
		EventQueue<_valueType> toReturn;
		for (size_t eventNum = 0; eventNum < _events.size(); eventNum++) {
			AlternativeEvent<_valueType> curEvent = _events.at(eventNum);
			this->add(curEvent);
			if (curEvent.isBacktrackAvailable()) { //If it can be backtraced, then an assignment was made.
				toReturn.add(
					Event<_valueType>(curEvent.location()->inputLevel(), curEvent.location())
				);
			}
		}
		return toReturn;
	}

	/*
	 * Add a given Event.
	 *
	 * Behavior can be changed through inheritance.
	 *
	 * @param _event The event to add.
	 */
	void add(AlternativeEvent<_valueType> _event) {
		this->stack_.push(_event);
	}

	/*
	 * Backtrack (i.e., remove) elements until either 1) an alternative is found
	 * (and is returned) or 2) the stack is empty.
	 *
	 * There is no guarantee that after backtracking the stack is empty. Users
	 * must check this after backtracking. If used as expected, processing the
	 * returned EventQueue will reset to the circuit to an X state (presuming
	 * all backtracks are the default "X" assignment.
	 *
	 * An optional value allows this function to be used as a "clear": this will
	 * ignore alternative values and will continue to pop events even if a
	 * backtrack is possible, which will leave the queue empty. By processing
	 * the EventQueue afterwards, the circuit shoul be returned to an "X" state.
	 *
	 * @param (optional) _clear Continue backtracking until the stack is clear.
	 * @return An event queue which needs to be evaluated after backtracking.
	 */
	EventQueue<_valueType> backtrack(bool _clear = false) {
		EventQueue<_valueType> toReturn;
		while (this->stack_.empty() == false) {
			AlternativeEvent<_valueType> curEvent = this->stack_.top();
			this->stack_.pop(); //NOTE: we will push an event back on if we take an alternative.
			if ((curEvent.isAlternativeAvailable() == true) && (_clear == false)) { //Here's an alternative: take it and return
				toReturn.add(curEvent.alternative());
				this->stack_.push(curEvent);
				break;
			}
			if (curEvent.isBacktrackAvailable() == true) { //Here's a backtrack: take it
				toReturn.add(curEvent.backtrack());
			}
			
		}

		return toReturn;
	}



	/*
	 * Return true if the ImplicationStack is Empty.
	 *
	 * This method is useful after backtrack to check if the returned event is
	 * valid. It's cleaner and more meaningful than checking if the location is
	 * a nullptr.
	 *
	 * @return True if the ImplicationStack is Empty.
	 */
	bool isEmpty() {
		return this->stack_.empty();
	}


private:
	/*
	 * The internal stack of events which contains this ImplicationStack's events.
	 */
	std::stack<AlternativeEvent<_valueType>> stack_;
};

//DELETE: obsolete.
//
//template <class _valueType>
//class IntelStack {
//public:
//
//	/*
//	 *Create a object for an implication stack
//	 */
//	std::stack<std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>>> stack;
//
//	/* This function helps to write an implication stack*/
//	std::stack<std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>>> intelStack(_nodeType* _primaryNode,
//		_lineType* _line,
//		_valueType _primaryValue,
//		bool _flag);
//
//
//	/* This function helps to read an implication stack*/
//	std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>> intelStackRead();
//
//	/*This function helps to clear stack contents*/
//	void intelStackClear();
//
//	/*Print Stack Elements from TOP to BOTTOM
//	*/
//	static void printStack(std::stack<std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>>> _stack);
//
//};
//
//////////////////////////////////////////////////////////////////////////////////
//// Inline function declarations.
//////////////////////////////////////////////////////////////////////////////////
//
//
///*
// * This function can be used for doing regular SAT and ATPG based SAT
// *
// * @param _primaryNode A node while doing backtracing.
// * @param _line The above node's output line.
// * @param _primaryValue The above line value.
// * @param _flag Alternatives tried ? If all alternatives tried: TRUE; else: FALSE.
// *
// * @return A stack having a pair of two pairs.
// *	First pair: First element -->  Node; Second element --> Node's output line value
// *	Second pair: First element --> Node's output line; Second Element --> Alternatives applied in case node is PI.
// */
//template<class _lineType, class _nodeType, class _valueType>
//inline std::stack<std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>>> IntelStack<_lineType, _nodeType, _valueType>::intelStack(
//	_nodeType * _primaryNode,
//	_lineType * _line,
//	_valueType _primaryValue,
//	bool _flag)
//{
//	std::stack<std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>>> toReturn;
//	std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>> piNodeValuePair;
//
//	piNodeValuePair.first.first = _primaryNode;
//	piNodeValuePair.first.second = _primaryValue;
//	piNodeValuePair.second.first = _line;
//	piNodeValuePair.second.second = _flag;
//	stack.push(piNodeValuePair);
//	return toReturn;
//}
//
//
///*
// * This function helps to read an implication stack
// *
// *
// * @return A stack after being read.
// */
//template<class _lineType, class _nodeType, class _valueType>
//inline std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>> IntelStack<_lineType, _nodeType, _valueType>::intelStackRead()
//{
//	std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>>  toReturn = stack.top();
//	return toReturn;
//}
//
///*
// * This function helps to clear stack contents
// *
// *
// */
//template<class _lineType, class _nodeType, class _valueType>
//inline void IntelStack<_lineType, _nodeType, _valueType>::intelStackClear()
//{
//	while (!stack.empty()) { stack.pop(); }
//}
//
//
///*
// * This function can be used for doing regular SAT and ATPG based SAT
// *
// * @param _stack An implication stack.
//
// */
//template<class _lineType, class _nodeType, class _valueType>
//inline void IntelStack<_lineType, _nodeType, _valueType>::printStack(std::stack<std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>>> _stack)
//{
//	if (_stack.empty()) { return; }
//	std::pair<std::pair<_nodeType*, _valueType>, std::pair<_lineType*, _valueType>> x = _stack.top();
//	_stack.pop();
//	printf("%s \t %s \t %d \t %d \tAlternativesTried? \t %d\n", x.first.first->name().c_str(), x.second.first->name().c_str(), x.first.second.magnitude(), x.first.second.valid(), x.second.second);
//	printf("/~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~/\n");
//	printStack(_stack);
//	_stack.push(x);
//}
#endif