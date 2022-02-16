/**
 * @file SATStructures.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-15
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef SATStructures_h
#define SATStructures_h

#include "SimulationStructures.hpp"
#include "Tracable.hpp"
#include "Combination.hpp"
#include "Value.hpp"
#include "ValueVectorFunctions.hpp"

//Forward declaration
template <class _valueType>
class SATLine;

/*
 * The combination of Node elements required to perform SAT.
 *
 * Functions are modified to create "boundary simulation" as well as to
 * generate all combinations.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class SATNode : public SimulationNode<_valueType>, public virtual Tracable<_valueType> {
public:
	SATNode() : SimulationNode<_valueType>(), Tracable<_valueType>(){

	}

	/*
	 * Create a SATNode.
	 *
	 * @param _function The function of this node (nullptr = "copy node")
	 * @param (optional) _name The name of the node.
	 * @param (optional) _inputs Input lines to the given node.
	 * @param (optional) _outputs Output lines to the given node.
	 */
	SATNode(
		Function<_valueType>* _function,
		std::string _name,
		std::unordered_set<SATLine<_valueType>*> _inputs = std::unordered_set<SATLine<_valueType>*>(),
		std::unordered_set<SATLine<_valueType>*> _outputs = std::unordered_set<SATLine<_valueType>*>()
	) : 
		SimulationNode<_valueType>(
			_function,
			_name,
			std::unordered_set<SimulationLine<_valueType>*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<SimulationLine<_valueType>*>(_outputs.begin(), _outputs.end())
			) ,
		Tracable<_valueType>(
			std::unordered_set<Tracable<_valueType>*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<Tracable<_valueType>*>(_outputs.begin(), _outputs.end()),
			 _name,
			_function
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
	 * Activate this Node and return all output nodes which need to be evaluated.
	 *
	 * New nodes will only be returned if the node's flag is not set.
	 *
	 * @return New events (and their priority) created by activating this event.
	 */
	virtual std::vector<Event<_valueType>> go(
		std::vector<_valueType> _values = std::vector<_valueType>()
	) {
		/*No restriction given to nodes. Full Simulation allowed*/
		//if (this->flag() == true) {
			return this->Evented<_valueType>::go(_values);
		//}
		//return std::set<std::pair<size_t, Evented<_primitive, _valueType>*>>();
	}


	
};

/*
 * A Line used in conjunction with SATNode.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class SATLine : virtual public SimulationLine<_valueType>, virtual public Tracable<_valueType> {
public:
	/*
	 * Create a with a "UNDEF" name.
	 */
	SATLine() : SimulationLine<_valueType>(), Tracable<_valueType>(), Connecting("UNDEF") {
	};

	/*
	 * Create a Line with a given name
	 *
	 * @param _name The name of the Line
	 */
	SATLine(std::string _name) :
		SimulationLine<_valueType>(_name),
		Tracable<_valueType>(
			std::unordered_set<Tracable<_valueType>*>(),
			std::unordered_set<Tracable<_valueType>*>(),
			_name
		),
		Connecting(_name) {
	};

	/*
	 * Activate this Line and return all outputs which need to be evaluated.
	 *
	 * New objects will only be returned if the line's flag is not set.
	 *
	 * @return New events (and their priority) created by activating this event.
	 */
	virtual std::vector<Event<_valueType>> go(
		std::vector<_valueType> _values = std::vector<_valueType>()
	) {
		/*No restriction given to nodes. Full Simulation allowed*/
		//if (this->flag() == true) {
			return this->Evented<_valueType>::go(_values);
		//}
		//return std::set<std::pair<size_t, Evented<_primitive, _valueType>*>>();
	}
};


#endif