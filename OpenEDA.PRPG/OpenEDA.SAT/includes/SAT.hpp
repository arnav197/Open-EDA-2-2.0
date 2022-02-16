/**
 * @file SAT.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-14
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef SAT_h
#define SAT_h

#include <ctime>
#include <algorithm>

#include "Goal.hpp"
#include "ImplicationStack.hpp"
#include "Backtracer.hpp"
#include "EventQueue.hpp"


/*
 * Performs satisfiability (SAT) on Circuit Line-Value combinations.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class SAT {
public:
	/*
	 * Create and set a per-combination time limit (in seconds).
	 *
	 * @param _limit Time limit (in seconds).
	 */
	SAT(int _limit) {
		this->limit_ = _limit * CLOCKS_PER_SEC;
	}

	SAT() = default;

	/*
	  * Perform SAT for a given set of Goals.
	  *
	  * NOTE: This function does not perform the actual SAT, but instead calls the
	  * internal SAT implementation.
	  *
	  * @param _circuit The circuit to apply inputs to.
	  * @param _goals A set of Goals to satisfy.
	  * @return The Goals which were satisfied.
	  */
	std::vector<Goal<_valueType>*> satisfy(
		std::unordered_set<Goal<_valueType>*> _goals
	) {
		std::vector<Goal<_valueType>*> toReturn;

		for (Goal<_valueType>* goal : _goals) {
			bool success = this->satisfy(goal);
			if (success) {
				toReturn.push_back(goal);
			}
		}

		return toReturn;
	}

	/*
	 * Perform sat for a given Goal.
	 *
	 * @param _circuit The circuit to apply inputs to.
	 * @param _goal The Goal to satisfy.
	 * @param _coi The cone of influence. If an empty list is given, a trace will
	 *        be performed (and can be re-used by calling functions).
	 * @return True if the goal was satisfied.
	 */
	bool satisfy(
		Goal<_valueType>* _goal
	) {
		//1. Setup
		std::clock_t startTime = this->setup();
		ImplicationStack<_valueType> implicationStack;

		while (this->timeLeft(startTime) == true) {
			if (_goal->success()) { //Goal complete: return success
				implicationStack.backtrack(true).process(); //clean the circuit back to an X state.
				this->clean();
				return true;
			} else if (_goal->impossible()) { //Goal impossible: backtrack
				implicationStack.backtrack().process(); //An event queue is returned, so process it.
				if (implicationStack.isEmpty()) { //After backtracking, we have an empty stack: we have a failure.
					this->clean();
					return false;
				}
			} else { //No success but no impossible: we need to make a PI assignment using our backtacer 
				implicationStack.add(
					this->backtracer_.backtrace(_goal->frontier())
				).process(); //After adding, an EventQueue of events to simulate (i.e., the single PI assignment) will be returned.
				
			}
		} //Time limit reached
		implicationStack.backtrack(true).process(); //clean the circuit back to an X state.
		this->clean();
		return false;
	}

	/*
	 * With the current clock, is there time left?
	 *
	 * @param The start time of a given process.
	 * @return True if there is time left.
	 */
	bool timeLeft(clock_t _time) {
		if ((clock() - _time) < this->limit_) {
			return true;
		}
		return false;
	}

	/*
	* The per-combination time limit of this object (in clock ticks).
	*/
	clock_t limit_;

private:
	/*
	 * Perform cleaning operations after a SAT operation (i.e., unrestrict a cone of influence)
	 */
	void clean() { //OLD INPUT: std::unordered_set<Tracable*>& _coi
		// simulator_.unssetActiveSimulationNodes(_coi); //for (SATNode<_primitive>* node : _coi) {node->flag(false); } //
	}

	/*
	 * This function "sets up" a satisfiability instance.
	 *
	 * @param _coi The cone-of-influence which needs to be reset. May be empty.
	 * @param _combination The combination to set-up for.
	 * @return The time when setup is complete.
	 */
	std::clock_t setup(
	) {
		std::clock_t startTime = std::clock();
		return startTime;
	}

	/*
	 * The backtracer algorithm to use.
	 */
	Backtracer<_valueType> backtracer_;

};

#endif