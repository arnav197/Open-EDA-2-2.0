/**
 * @file FaultSimulator.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-23
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef FaultGenerator_h
#define FaultGenerator_h

#include <set>
#include <unordered_map>

#include "Fault.hpp"
#include "Circuit.h"
#include "SimulationStructures.hpp"
#include "FaultStructures.hpp"

/*
 * An object capible of populating a list of faults for a Circuit.
 *
 * The base implementation generates stuck-at faults using checkpoint theorem
 * (not applied to XOR gates).
 *
 * The base implementation does only binary faults.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class FaultGenerator {
public:
	/*
	 * Return a list of all faults for a given Circuit.
	 *
	 * @param _circuit The circuit to generate Faults for.
	 * @param (optional) _checkpointReduce Reduce the number of faults by doing
	 *        checkpoint equivalence.
	 * @return The list of Faults.
	 */
	static std::unordered_set<Fault<_valueType>*> allFaults(Circuit* _circuit, bool _checkpointReduce = true);



private:
	/*
	 * Return all "checkpoints" (in terms of fault generation) in the circuit.
	 *
	 * @param _circuit The circuit to get the checkpoints for.
	 * @return The list of checkpoint lines.
	 */
	static std::unordered_set<FaultyLine<_valueType>*> checkpoints(Circuit* _circuit);

	/*
	 * Return all fan-out objects starting from the given object.
	 *
	 * @param _line The object to start tracing forward from.
	 * @return The set of all fan-out object.
	 */
	static std::unordered_set<Connecting*> fanouts(Connecting * _base, std::unordered_map<Connecting*, bool> & _visited);


};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////

template <class _valueType>
inline std::unordered_set<Fault<_valueType>*> FaultGenerator<_valueType>::allFaults(Circuit * _circuit, bool _checkpointReduce) {
	std::unordered_set<Fault<_valueType>*> toReturn;
	std::unordered_set<FaultyLine<_valueType>*> circuitCheckpoints; //Lines where faults are generated
	if (_checkpointReduce == true) {
		circuitCheckpoints = checkpoints(_circuit);
	}
	else {
		for (Levelized* node : _circuit->nodes()) {
			for (Connecting* line : node->inputs()) {
				FaultyLine<_valueType>* cast = dynamic_cast<FaultyLine<_valueType>*>(line);
				circuitCheckpoints.emplace(cast);
			}
			for (Connecting* line : node->outputs()) {
				FaultyLine<_valueType>* cast = dynamic_cast<FaultyLine<_valueType>*>(line);
				circuitCheckpoints.emplace(cast);
			}
		}
	}
	for (FaultyLine<_valueType>* checkpoint : circuitCheckpoints) {
		Fault<_valueType>* sa0 = new Fault<_valueType>(checkpoint, _valueType( _valueType::MIN() ) ); //NOTE: "MIN" and "MAX" replaced "(_primitive)0x000000.... and 0xFFFFFFFFFFF"
		Fault<_valueType>* sa1 = new Fault<_valueType>(checkpoint, _valueType( _valueType::MAX() ) );
		toReturn.emplace(sa0);
		toReturn.emplace(sa1);
	}
	return toReturn;

}

template <class _valueType>
inline std::unordered_set<FaultyLine<_valueType>*> FaultGenerator<_valueType>::checkpoints(Circuit * _circuit) {
	std::unordered_set<Connecting*> preCastToReturn;

	//Step 1: get all the PI lines and fanout lines.
	std::unordered_map<Connecting*, bool> visited; //Mapps lines to visited. This allows lines to not have the "Tracable" attribute.
	for (Levelized* pi : _circuit->pis()) {
		if (pi->outputs().size() != 1) {
			throw "Problem: a PI in the circuit does not have an output line.";
		}
		Connecting* piLine = *(pi->outputs().begin());
		preCastToReturn.emplace(piLine);
		std::unordered_set<Connecting*> toAdd = fanouts(piLine, visited);
		preCastToReturn.insert(toAdd.begin(), toAdd.end());
	}

	//Step 2: cast all objects to FaultyLines.
	std::unordered_set<FaultyLine<_valueType>*> toReturn;
	for (Connecting* line : preCastToReturn) {
		FaultyLine<_valueType>* cast = dynamic_cast<FaultyLine<_valueType>*>(line);
		if (cast != nullptr) {
			//DELETE: throw "A fanout/PI object in the circuit is not a FaultyLine.";
			toReturn.emplace(cast);
		}
		//DELETE: toReturn.emplace(cast);
	}

	return toReturn;
}

template <class _valueType>
inline std::unordered_set<Connecting*> FaultGenerator<_valueType>::fanouts(Connecting * _base, std::unordered_map<Connecting*, bool> & _visited) {
	//Case 0: we've been here before.
	if (_visited.find(_base) != _visited.end()) {
		return std::unordered_set<Connecting*>();
	}
	_visited[_base] = true;

	//Case 1: we reached the end.
	if (_base->outputs().size() == 0) {
		return std::unordered_set<Connecting*>();
	}

	//Case 2: we have a single output
	if (_base->outputs().size() == 1) {
		return fanouts(*(_base->outputs().begin()), _visited);
	}

	//Case 3: we have multiple outputs (and hence outputs)
	std::unordered_set<Connecting*> toReturn = _base->outputs();
	for (Connecting* output : _base->outputs()) {
		std::unordered_set<Connecting*> toAdd = fanouts(output, _visited);
		toReturn.insert(toAdd.begin(), toAdd.end());
	}
	return toReturn;
}

#endif