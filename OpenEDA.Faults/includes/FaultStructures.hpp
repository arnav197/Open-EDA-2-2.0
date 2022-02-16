/**
 * @file FaultStructures.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-22
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef FaultStructures_h
#define FaultStructures_h

#include "SimulationStructures.hpp"
#include "Fault.hpp"

 //Forward declaration
template <class _valueType>
class Faulty;
template <class _valueType>
class Fault;
/*
 * A Line with a potential Fault on it.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class FaultyLine : virtual public SimulationLine<_valueType>, virtual public Faulty<_valueType> {
public:
	/*
	 * Create a with a "UNDEF" name.
	 */
	FaultyLine() : SimulationLine<_valueType>(), Faulty<_valueType>(), Connecting("UNDEF") {
	};

	/*
	 * Create a Line with a given name
	 *
	 * @param _name The name of the Line
	 */
	FaultyLine(std::string _name) :
		SimulationLine<_valueType>(_name),
		Faulty<_valueType>(),
		Connecting(_name)
	{
	};


	/*
	 * Return Create an copy of this object.
	 *
	 * @return A new FaultyLine.
	 */
	virtual Connecting* clone() const {
		return new FaultyLine<_valueType>(this->name());
	}

};

/*
 * A node which can accept FaultyLines as inputs and outputs.
 *
 * NOTE: This structure has no added functionallity.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class FaultyNode : public virtual SimulationNode<_valueType>  {
public:
	/*
	 * Create a node with no inputs, no outputs, and a "copy" function.
	 */
	FaultyNode() : SimulationNode<_valueType>() {

	}

	/*
	 * Create a Boolean node.
	 *
	 * @param _function The function of this node (nullptr = "copy node")
	 * @param (optional) _name The name of the node.
	 * @param (optional) _inputs Input lines to the given node.
	 * @param (optional) _outputs Output lines to the given node.
	 */
	FaultyNode(
		Function<_valueType>* _function,
		std::string _name = "UNDEF",
		std::unordered_set<FaultyLine<_valueType>*> _inputs = std::unordered_set<FaultyLine<_valueType>*>(),
		std::unordered_set<FaultyLine<_valueType>*> _outputs = std::unordered_set<FaultyLine<_valueType>*>()
	) :
		SimulationNode<_valueType>(
			_function,
			_name,
			std::unordered_set<SimulationLine<_valueType>*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<SimulationLine<_valueType>*>(_outputs.begin(), _outputs.end())
			),
		Valued<_valueType>(
			_function
			),
		Connecting(
			std::unordered_set<Connecting*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<Connecting*>(_outputs.begin(), _outputs.end()),
			_name
		) 
	{
	}

	/*
	 * Create an copy of this node.
	 *
	 * @return A new FaultyNode
	 */
	virtual Connecting* clone() const {
		return new FaultyNode<_valueType>(this->function(), this->name());
	}
};

#endif