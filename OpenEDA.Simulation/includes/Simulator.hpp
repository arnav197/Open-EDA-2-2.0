/**
 * @file Simulator.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2018-12-27
 *
 * @copyright Copyright (c) 2018
 *
 */

#ifndef Simulator_h
#define Simulator_h

#include <vector>

#include "Value.hpp"
#include "Circuit.h"
#include "Function.hpp"
#include "SimulationStructures.hpp"
#include "EventQueue.hpp"

/*
 * A simulator which applies stimulus to a circuit and observes outputs.
 *
 * A Simulator is any object which has the ability to apply stimulus to an
 * object.
 *
 * Objects to be simulated must be of the "Circuit" type, and must be able to
 * return a set of input Nodes which can have stimulus applied to them.
 *
 * The default (uninherited) simulator uses a priority queue to evaluate
 * the Circuit.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class Simulator {
public:
	/*
	 * Apply a given vector of input values to the Circuit.
	 *
	 * The number of values provided must be equal the number of inputs
	 * provided, else an exception will be thrown.
	 *
	 * The Circuit will be evalauted based on changed-Value events in a
	 * levelized order. Runtime is between O(1) and O(# Nodes).
	 *
	 * If no input Nodes are applied, a vectorised version of Circuit input
	 * Nodes will be generated. If this is done, there is no set order of input
	 * values to be applied.
	 *
	 * @param _circuit The circuit to perform simulation on.
	 * @param _stimulus (optional) The input vector of Values to apply to the
	 *        circuit. If not given, the input stimulus will copy the circuit's
	 *        current inputs.
	 * @param (optional) _queue The simulation queue to use. If none is given
	 *        (which is typical), a blank quueue will be used.
	 * @param (optional) _inputs Which circuit inputs to apply stimulus to. If not
	 *        given, it will be presumed each input value in "_stimulus"
	 *        represents a Circuit input.
	 * @param (optional) _outputs The outputs to measure. If not given, all
	 *        circuit outputs will be measured.
	 * @param (optional) _observe Which circuit outputs to measure (true =
	 *        measure, false = do not measure). If not given, all outputs will
	 *        be measured.
	 * @return The Circuit output Values created by this stimulus.
	 */
	std::vector<_valueType> applyStimulus(
		Circuit * _circuit,
		std::vector<_valueType> _stimulus = std::vector<_valueType>(),
		EventQueue<_valueType> _simulationQueue = EventQueue<_valueType>(),
		std::vector<SimulationNode<_valueType>*> _inputs = std::vector<SimulationNode<_valueType>*>(),
		std::vector<SimulationNode<_valueType>*> _outputs = std::vector<SimulationNode<_valueType>*>(),
		std::vector<bool> _observe = std::vector<bool>()
	);

protected:
	/*
	 * The current outputs of the Circuit.
	 *
	 * @param _circuit The Circuit to probe.
	 * @param _outputs The outputs to measure. If not given, all
	 *        circuit outputs will be measured.
	 * @param _observe Which circuit outputs to measure (true =
	 *        measure, false = do not measure). If not given, all outputs will
	 *        be measured.
	 * @return The current outputs of the Circuit.
	 */
	std::vector<_valueType> outputs(
		Circuit* _circuit,
		std::vector<SimulationNode<_valueType>*> & _outputs,
		std::vector<bool> & _observe  
		);

};

template <class _valueType>
inline std::vector<_valueType> Simulator<_valueType>::applyStimulus(
	Circuit * _circuit,
	std::vector<_valueType> _stimulus,
	EventQueue<_valueType> _simulationQueue,
	std::vector<SimulationNode<_valueType>*> _inputs,
	std::vector<SimulationNode<_valueType>*> _outputs,
	std::vector<bool> _observe
) {
	if (_circuit == nullptr) { throw "No circuit given to apply stimulus to."; }
	if (_inputs.size() == 0) {//DEL Cannot convert directly from Node* to SimulationNode<T>* _inputs = std::vector<SimulationNode<T>*>(_circuit->pis().begin(), _circuit->pis().end());
		for (Levelized* input : _circuit->pis()) {
			SimulationNode<_valueType>* cast = dynamic_cast<SimulationNode<_valueType>*>(input); //NOTE: Can this be done wtihout a dynamic_cast? It may require changing the pointer type to be more "strict" or removing virtual inheritence, which in turn may require re-organizing code.
			_inputs.push_back(cast);
		}
	}
	if (_stimulus.size() == 0) {
		_stimulus = std::vector<_valueType>(_inputs.size(), _valueType());
	}
	if (_stimulus.size() != _inputs.size()) {
		throw "When applying stimulus, stimulus and inputs do not match in size.";
	}

	//Initialize the queue.
	for (size_t i = 0; i < _stimulus.size(); ++i) {
		_valueType value = _stimulus.at(i);
		std::vector<Event<_valueType>> events = _inputs.at(i)->go(
			std::vector<_valueType>(1, value)
		);
		_simulationQueue.add(events);
	}

	_simulationQueue.process();

	return this->outputs(_circuit, _outputs, _observe);
}


template <class _valueType>
std::vector<_valueType> Simulator<_valueType>::outputs(
	Circuit * _circuit,
	std::vector<SimulationNode<_valueType>*> & _outputs,
	std::vector<bool> & _observe
) {
	if (_outputs.empty() == true) {
		for (Levelized* output : _circuit->pos()) {
			_outputs.push_back(dynamic_cast<SimulationNode<_valueType>*>(output));
		}
		//DELETE: doesn't work, won't compile. _outputs = std::vector<SimulationNode<T>*>(_circuit->pos().begin(), _circuit->pos().end());
	}
	if (_observe.empty() == true) {
		_observe = std::vector<bool>(_outputs.size(), true);
	}
	else if (_observe.size() != _outputs.size()) {
		throw "_outputs/_observe size mismach while measuring simulation outputs.";
	}

	std::vector<_valueType> toReturn;
	for (size_t i = 0; i < _outputs.size(); i++) {
		if (_observe.at(i) == false) {
			continue;
		}
		SimulationNode<_valueType>* output = _outputs.at(i);
		std::unordered_set<Connecting*> lines = output->inputs();
		SimulationLine<_valueType>* outputLine = dynamic_cast<SimulationLine<_valueType>*>(*(lines.begin()));
		toReturn.push_back(outputLine->value());
	}
	return toReturn;
}

#endif
