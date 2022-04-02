/**
 * @file SimulationNet.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-18
 *
 * @copyright Copyright (c) 2019
 *
 */



#ifndef SimulationNet_h
#define SimulationNet_h

#include "simulation/SimulationStructures.hpp"
#include "simulation/Net.hpp"

//Forward declarations.
template <class _valueType>
class SimulationLine;
template <class _valueType>
class SimulationNode;

/*
 * An extension of Net with explicit simulation functions.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class SimulationNet : public Net< SimulationLine<_valueType> , SimulationNode<_valueType> > {
public:
	/*
	 * Create a Net from a given Line.
	 *
	 * The Line given can be any Line on a Connection of Lines, i.e., the "root"
	 * or any fan-out.
	 *
	 * @param _line The root or fan-out Line of the Net to create.
	 */
	SimulationNet(SimulationLine<_valueType>* _line) : Net<SimulationLine<_valueType>, SimulationNode<_valueType>>(_line){

	}

	/*
	 * Set the Value on this Net (starting at the root).
	 *
	 * Note: the Value given will not necessarly be the value assigned to all 
	 * Lines if the root does not "take" the Value (i.e., return the value
	 * after it is assigned).
	 *
	 * @param _value The Value to assign.
	 */
	void value(_valueType _value) {
		this->root_->value(_value);
		for (SimulationLine<_valueType>* fanout : this->fanouts_) {
			fanout->value(_value);
		}
	}
};

#endif