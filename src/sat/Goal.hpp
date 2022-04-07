/**
 * @file Objective.hpp
 * @author Soham Roy (szr0075@auburn.edu)
 * @version 0.1
 * @date 2019-11-22
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef Objective_h
#define Objective_h

#include <vector>
#include "simulation/Value.hpp"

/**
 * **ABSTRACT CLASS**
 *
 * When performing any satisfiability(-like) problem, a Goal is a "final"
 * Objective to satisfy. Once a Goal is met, it will tell the SAT algorithm to
 * stop. Alternatively, a Goal can tell when failure is inevitable, and will
 * tell the SAT algorithm to stop.
 *
 * Goals are abstract objects, and their international definition of "success",
 * "impossible" (i.e., a backtrace is required), and "frontier" (i.e., a list
 * of next possible choices to make.
 *
 * @param _valueType The type of value being simulated, e.g.,
 *                   Value/FaultyValue
 */
template <class _valueType>
class Goal {
public:
	/*
	 * Is the Goal satisfied (i.e., successful)?
	 *
	 * @return True if the goal is satisfied.
	 */
	virtual bool success() = 0;

	/*
	 * Is the Goal impossible (i.e., a backtrace is needed)?
	 *
	 * @return True if the goal is not possible
	 */
	virtual bool impossible() = 0;

	/*
	 * Return a "frontier" of possible values to set.
	 *
	 * NOTE: by this definition, the "frontier" are the Values to SET.
	 *
	 * @return A list of location - Value pairs corresponding to choices which
	 *         may satisfy this Goal.
	 */
	virtual std::vector<std::pair<Evented<_valueType>*, _valueType>> frontier() = 0;
};




#endif 