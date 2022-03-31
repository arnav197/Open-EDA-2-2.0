/**
 * @file FaultStructures.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-05-12
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef POWER_TPI_Structures_h
#define POWER_TPI_Structures_h

#include "FaultStructures.hpp"
#include "COPStructures.hpp"

 //Forward declaration
template <class _valueType>
class Faulty;

/*
 * A line which can be used for cop-based TPI and fault simulation.
 *
 * NOTE: This structure has no added functionallity.
 *
 * @param _width The width of simulation (bool or unsigned long long it).
 */
template <class _width>
class POWER_TPI_Line : virtual public COPLine<_width>, virtual public FaultyLine<_width> {
public:
	/*
	 * Create a with a "UNDEF" name.
	 */
	POWER_TPI_Line() : COPLine<_width>(), FaultyLine<_width>(), Connecting("UNDEF") {
	};

	/*
	 * Create a Line with a given name
	 *
	 * @param _name The name of the Line
	 */
	POWER_TPI_Line(std::string _name) :
		COPLine<_width>(_name),
		FaultyLine<_width>(_name),
		Connecting(_name) {
	};

	virtual Connecting* clone() const {
		return new POWER_TPI_Line<_width>(this->name());
	}
};

/*
 * A node which can accept FaultyLines as inputs and outputs.
 *
 * NOTE: This structure has no added functionallity.
 *
 * @param _width The width of simulation (bool or unsigned long long it).
 */
template <class _width>
class POWER_TPI_Node : public virtual COPNode<_width>, virtual public FaultyNode<_width> {
public:
	/*
	 * Create a node with no inputs, no outputs, and a "copy" function.
	 */
	POWER_TPI_Node() : COPNode<_width>(), FaultyNode<_width>() {

	}

	/*
	 * Create a Boolean node.
	 *
	 * @param _function The function of this node (nullptr = "copy node")
	 * @param (optional) _name The name of the node.
	 * @param (optional) _inputs Input lines to the given node.
	 * @param (optional) _outputs Output lines to the given node.
	 */
	POWER_TPI_Node(
		Function<_width>* _function,
		std::string _name,
		std::unordered_set<POWER_TPI_Line<_width>*> _inputs = std::unordered_set<POWER_TPI_Line<_width>*>(),
		std::unordered_set<POWER_TPI_Line<_width>*> _outputs = std::unordered_set<POWER_TPI_Line<_width>*>()
	) :
		COPNode<_width>(
			_function,
			_name,
			std::unordered_set<COPLine<_width>*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<COPLine<_width>*>(_outputs.begin(), _outputs.end())
			),
		FaultyNode<_width>(
			_function,
			_name,
			std::unordered_set<FaultyLine<_width>*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<FaultyLine<_width>*>(_outputs.begin(), _outputs.end())
			),
		Valued<_width>(
			_function
			),
		Connecting(
			std::unordered_set<Connecting*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<Connecting*>(_outputs.begin(), _outputs.end()),
			_name
		)
	{
	}


};

#endif //POWER_TPI_Structures_h

