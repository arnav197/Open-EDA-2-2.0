/**
 * @file Tracable.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-02-08
 *
 * @copyright Copyright (c) 2019
 *
 */


#ifndef Tracable_h
#define Tracable_h

#include <unordered_set>
#include <limits>

#include "Connection.h"
#include "simulation/Value.hpp"

/*
 * This object is capible of "tracing" forward and backwards and returning
 * related objects.
 *
 * The only added feature of a "Traceable" object is its flags.
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue.
 */
template <class _valueType>
class Tracable : public virtual Connecting, public virtual Valued<_valueType> {
public:
	/*
	 * Create an object connected to the given inputs and outputs.
	 *
	 * Optionally, designate this as a trace-stopping object (default not).
	 *
	 * @param (optional) inputs A vector of input Connecting objects.
	 * @param (optional) outputs A vector of output Connecting objects.
	 * @param (optional) _name Name of the connection (defaults to empty strying).
	 * @param (optional) _function The function of this object.
	 * @param (optional) _value The value of this object.
	 */
	Tracable(std::unordered_set<Tracable*> _inputs = std::unordered_set<Tracable*>(),
			 std::unordered_set<Tracable*> _outputs = std::unordered_set<Tracable*>(),
			 std::string _name = "",
			 Function<_valueType>* _function = nullptr,
			 _valueType _value = _valueType()
	) :
		Connecting(
			std::unordered_set<Connecting*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<Connecting*>(_outputs.begin(), _outputs.end()),
			_name
		),
		Valued<_valueType>(
			_function,
			_value
		)
	{
		this->flag_ = false;
	};

	/*
	 * Return this object's current flag.
	 *
	 * @return The object's flag value.
	 */
	bool flag() const {
		return this->flag_;
	}

	/*
	 * Return this object's current flag while setting it to a new value..
	 *
	 * @param _flag The object's new flag value.
	 * @return The object's flag value (before being set).
	 */
	bool flag(bool _flag) {
		bool toReturn = this->flag_;
		this->flag_ = _flag;
		return toReturn;
	}

private:
	/*
	 * This object's Boolean flag.
	 */
	bool flag_;

};

#endif