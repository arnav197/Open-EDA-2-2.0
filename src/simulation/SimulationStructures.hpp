/**
 * @file SimulationStructures.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-02
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef SimulationStructures_h
#define SimulationStructures_h

#include "simulation/Value.hpp"
#include "Level.h"
#include "simulation/Function.hpp"
#include "simulation/EventQueue.hpp"
#include "simulation/SimulationNet.hpp"
#include <string>

/*
 * A Line which can be used for basic binary Simulation.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class SimulationLine : virtual public Evented<_valueType> {
public:
	/*
	 * Create a simulation line with a "UNDEF" name.
	 */
	SimulationLine() : 
		Evented<_valueType>(
			std::unordered_set<Evented<_valueType>*>(),
			std::unordered_set<Evented<_valueType>*>(),
			"UNDEF"
		) ,
		Connecting(
			"UNDEF"
		)
	{
	};

	/*
	 * Create a Line with a given name
	 *
	 * @param _name The name of the Line
	 */
	SimulationLine(
		std::string _name
	) : 
		Evented<_valueType>(
			std::unordered_set<Evented<_valueType>*>() ,
			std::unordered_set<Evented<_valueType>*>(),
			_name
		) , 
		Connecting(
			_name
		)
	{
	};



	/*
	 * Return Create an copy of this object.
	 *
	 *
	 *
	 * @return new simulationline.
	 */
	virtual Connecting* clone() const {
		return new SimulationLine<_valueType>(this->name());
	}
};


/*
 * A Node which contains all parts needed for event-based simulation.

 * @param _primitive The underlying primitive to implement in Simulation (bool, char, etc.)
 */
template <class _primitive>
class SimulationNode : virtual public Evented<_primitive> {
public:
	/*
	 * Create a node with no inputs, no outputs, and a "copy" function.
	 */
	SimulationNode() : Evented<_primitive>(
		std::unordered_set<Evented<_primitive>*>(),
		std::unordered_set<Evented<_primitive>*>(),
		"UNDEF", 
		nullptr
		), 
		Connecting("UNDEF")
	{
		//DELETE: This is not needed anymore //this->function_ = new BooleanFunction<_primitive>("copy");
	}

	/*
	 * Create a Simulation node.
	 *
	 * @param _function The function of this node (nullptr = "copy node")
	 * @param (optional) _name The name of the node.
	 * @param (optional) _inputs Input lines to the given node.
	 * @param (optional) _outputs Output lines to the given node.
	 */
	SimulationNode(
		Function<_primitive>* _function,
		std::string _name = "UNDEF",
		std::unordered_set<SimulationLine<_primitive>*> _inputs = std::unordered_set<SimulationLine<_primitive>*>(),
		std::unordered_set<SimulationLine<_primitive>*> _outputs = std::unordered_set<SimulationLine<_primitive>*>()
	) :
		Evented<_primitive>(
			std::unordered_set<Evented<_primitive>*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<Evented<_primitive>*>(_outputs.begin(), _outputs.end()),
			_name,
			_function
		),
		Valued<_primitive>(
			_function
			),
		Connecting(
			std::unordered_set<Connecting*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<Connecting*>(_outputs.begin(), _outputs.end()),
			_name
		)
	{
	}

	///*
	// * Return the name of the node's output.
	// *
	// * Throw an exception if there is no output.
	// *
	// * @return This object's name.
	// */
	//virtual std::string name() const {
	//	std::string toReturn = this->function()->string() + " ";
	//	if (this->outputs().size() != 0) {
	//		toReturn += (*(this->outputs().begin()))->name();
	//	} else {
	//		toReturn += "UNKOWN";
	//	}
	//	return toReturn;
	//}


	/*
	 * Create a copy of this simulation object.
	 *
	 * @return The new SimulationNode.
	 */
	virtual Connecting* clone() const {
		return new SimulationNode(this->function(), this->name());
	}


};


#endif