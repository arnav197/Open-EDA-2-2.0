/**
 * @file ATPGStructures.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-12-20
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef ATPGStructures_h
#define ATPGStructures_h

#include "sat/SATStructures.hpp"
#include "faults/FaultStructures.hpp"

 //Forward declaration
template <class _valueType>
class ATPGLine;

/*
 * The combination of features required to perfrom ATPG.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class ATPGNode : public SATNode<_valueType>, public FaultyNode<_valueType> {
public:
	ATPGNode() : SATNode<_valueType>(), FaultyNode<_valueType>() {
	}

	/*
	 * Create an ATPGNode.
	 *
	 * @param _function The function of this node (nullptr = "copy node")
	 * @param (optional) _name The name of the node.
	 * @param (optional) _inputs Input lines to the given node.
	 * @param (optional) _outputs Output lines to the given node.
	 */
	ATPGNode(
		Function<_valueType>* _function,
		std::string _name,
		std::unordered_set<ATPGLine<_valueType>*> _inputs = std::unordered_set<ATPGLine<_valueType>*>(),
		std::unordered_set<ATPGLine<_valueType>*> _outputs = std::unordered_set<ATPGLine<_valueType>*>()
	) :
		SATNode<_valueType>(
			_function,
			_name,
			std::unordered_set<SATLine<_valueType>*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<SATLine<_valueType>*>(_outputs.begin(), _outputs.end())
			),
		FaultyNode<_valueType>(
			_function,
			_name,
			std::unordered_set<FaultyLine<_valueType>*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<FaultyLine<_valueType>*>(_outputs.begin(), _outputs.end())
			),
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
		) {
	}

};

/*
 * A Line used in conjunction with ATPGNode.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class ATPGLine : virtual public SATLine<_valueType>, virtual public FaultyLine<_valueType> {
public:
	/*
	 * Create a with a "UNDEF" name.
	 */
	ATPGLine() : SATLine<_valueType>(), FaultyLine<_valueType>(), Tracable<_valueType>(), Connecting("UNDEF") {
	};

	/*
	 * Create a Line with a given name
	 *
	 * @param _name The name of the Line
	 */
	ATPGLine(std::string _name) :
		SATLine<_valueType>(_name),
		FaultyLine<_valueType>(_name),
		Tracable<_valueType>(
			std::unordered_set<Tracable<_valueType>*>(),
			std::unordered_set<Tracable<_valueType>*>(),
			_name
			),
		Connecting(_name)
	{
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