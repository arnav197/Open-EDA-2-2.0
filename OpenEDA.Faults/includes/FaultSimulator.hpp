/**
 * @file FaultSimulator.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-17
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef FaultSimulator_h
#define FaultSimulator_h

#include <unordered_set>

#include "Fault.hpp"
#include "Simulator.hpp"
#include "FaultyValue.hpp"

 /*
  * A Simulator capible of performing fault simulation.
  *
  * FaultSimulator builds upon existing event-based Simulators to apply
  * stimulus and measure the respons of circuits, except the same stimulus will
  * be applied multiple times: one for each Fault (plus once for the original
  * circuit).
  *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
  */
template <class _valueType>
class FaultSimulator : public Simulator<_valueType> {
public:
	/*
	 * Create the fautl simulator.
	 *
	 * The parameter is set to true if the faults given should be interpreted
	 * as TDFs. By default, faults given are presumed to be SAFs.
	 *
	 * @param (optional) _tdfEnable Make this simulator a TDF simulator.
	 */
	FaultSimulator(bool _tdfEnable = false);

	/*
	 * On destruction, clear (delete) the fault lists.
	 */
	virtual ~FaultSimulator();

	/*
	 * Apply a given vector of input values to the Circuit.
	 *
	 * The number of values provided must be equal the number of inputs
	 * provided, else an exception will be thrown.
	 *
	 * If no input Nodes are applied, a vectorised version of Circuit input
	 * Nodes will be generated. If this is done, there is no set order of input
	 * values to be applied.
	 *
	 * Detected faults will be stored internally.
	 *
	 * @param _circuit The circuit to perform simulation on.
	 * @param _stimulus The input vector of Values to apply to the Circuit.
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
		std::vector<_valueType> _stimulus,
		EventQueue<_valueType> _simulationQueue = EventQueue<_valueType>(),
		std::vector<SimulationNode<_valueType>*> _inputs = std::vector<SimulationNode<_valueType>*>(),
		std::vector<SimulationNode<_valueType>*> _outputs = std::vector<SimulationNode<_valueType>*>(),
		std::vector<bool> _observe = std::vector<bool>()
	);

	/*
	 * Set the fault list to simulate.
	 *
	 * @param _faults The set of faults.
	 */
	void setFaults(std::unordered_set<Fault<_valueType>*> _faults);
	/*
	 * reSet the fault list to simulate.
	 *
	 * @param _faults The set of faults.
	 */
	void resetFaults();

	/*
	 * Return the list of detected faults.
	 *
	 * @return The detected faults.
	 */
	std::unordered_set<Fault<_valueType>*> detectedFaults();

	/*
	 * Return the list of undetected faults.
	 *
	 * @return The undetected faults.
	 */
	std::unordered_set<Fault<_valueType>*> undetectedFaults();

	/*
	 * Return the fault coverage.
	 *
	 * @return the fault coverage.
	 */

	float faultcoverage();

	/*
	 * Clear the list of faults associated with this fault simulator.
	 * NOTE: They will NOT be deleted. That should be done elsewhere.
	 *
	 * @returns The list of faults in this simulator (do be deleted).
	 */
	std::unordered_set<Fault<_valueType>*> clearFaults();



protected:
	/*
	 * All undetected faults.
	 */
	std::unordered_set<Fault<_valueType>*> undetectedFaults_;

	/*
	 * All detected faults.
	 */
	std::unordered_set<Fault<_valueType>*> detectedFaults_;

	/*
	 * Is the Fault worth simulating, i.e., can it have an impact?
	 *
	 * @param The Fault which may (not) have an impact when activated.
	 * @return True if the fault will have an impact when activated.
	 */
	virtual bool hasImpact(Fault<_valueType>* _fault);

	/*
	 * Will a Fault NOT be excited.
	 *
	 * This is not the same as !hasImpact, since the line which the Fault is on
	 * must have a valid value.
	 *
	 * NOTE: This is used for TDF simulation (i.e., "will the first vector NOT
	 *       excite the fault, and therefore the second might).
	 *
	 * @param _fault The fault which may (not) have an impact when activated.
	 * @return True if the fault will not be effected with its current value.
	 */
	virtual bool notExcited(Fault<_valueType>* _fault);

private:



	/*
	 * Is this a TDF simulator?
	 */
	bool tdfEnable_;
};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////

template<class _valueType>
inline FaultSimulator<_valueType>::FaultSimulator(bool _tdfEnable) {
	this->tdfEnable_ = _tdfEnable;
}

template<class _valueType>
inline FaultSimulator<_valueType>::~FaultSimulator() {
	if (this->clearFaults().size() != 0) {
		//printf("WARNING: a fault simulator had undeleted faults?");
	}
}

/*
 * Should a given Fault be considered for simulation BEFORE the vector is
 * applied (only relevant for TDF simulation).
 *
 * For TDF simulation, a fault is relevant if...
 * 1) The normal (i.e., non-faulty) value of the location of the fault is valid.
 * 2) After the fault is enabled, the value on the line is NOT faulty. This
 *    implies that a transition on the line may be faulty.
 *
 * @param _fault The Fault to check if it is relevant BEFORE the next vector
 *               is applied.
 * @return Is the Fault relevant?
 */
template<class _valueType>
inline bool preVectorFaultRelevant(Fault<_valueType>* _fault) {
	_valueType preEnableValue = _fault->location()->value();
	if (!(preEnableValue.valid())) { //Are no bits valid?
		return false;
	}
	_fault->go(); //Activate the fault.
	_valueType postEnableValue = _fault->location()->value();
	_fault->go(); //Deactivate the fault.
	if (~postEnableValue.faulty()) {//Are any bits not faulty?
		return true; //yes
	}
	return false; //No
}

//Because Bool does not have predictable "~" behavior, the previous
//needs to be explecitly defined for "bool".
template<>
inline bool preVectorFaultRelevant(Fault<FaultyValue<bool>>* _fault) {
	FaultyValue<bool> preEnableValue = _fault->location()->value();
	if (!preEnableValue.valid()) { //Are no bits valid?
		return false;
	}
	_fault->go(); //Activate the fault.
	FaultyValue<bool> postEnableValue = _fault->location()->value();
	_fault->go(); //Deactivate the fault.
	if (!postEnableValue.faulty()) {//Are any bits not faulty?
		return true; //yes
	}
	return false; //No
}

template<class _valueType>
inline std::vector<_valueType> FaultSimulator<_valueType>::applyStimulus(
	Circuit * _circuit,
	std::vector<_valueType> _stimulus,
	EventQueue<_valueType> _simulationQueue,
	std::vector<SimulationNode<_valueType>*> _inputs,
	std::vector<SimulationNode<_valueType>*> _outputs,
	std::vector<bool> _observe
)
{
	//DELETE: redundant.
	////Populated (observable) output list (if not given)
	//if (_outputs.empty() == true) {
	//	for (Levelized* output : _circuit->pos()) {
	//		_outputs.push_back(dynamic_cast<SimulationNode<_valueType>*>(output));
	//	}
	//}
	//if (_observe.empty() == true) {
	//	_observe = std::vector<bool>(_outputs.size(), true);
	//}
	//else if (_observe.size() != _outputs.size()) {
	//	throw "_outputs/_observe size mismach during fault simulation.";
	//}

	//Chose faults to simulate based on previous vector.
	std::vector<Fault<_valueType>*>  faultsToSimulate;
	if (this->tdfEnable_ == false) { //S-A-F mode: previous vector doesn't matter.
		faultsToSimulate = std::vector<Fault<_valueType>*>(this->undetectedFaults_.begin(), this->undetectedFaults_.end());
	} else {//T-D-F mode: previous vector matters.
		for (Fault<_valueType>* fault : this->undetectedFaults_) {
			if (preVectorFaultRelevant(fault)) {
				faultsToSimulate.push_back(fault);
			}
		}
	}

	//Obatin the "non-faulty" state.
	std::vector<_valueType> goodOutputs = this->Simulator<_valueType>::applyStimulus(_circuit, _stimulus, _simulationQueue, _inputs, _outputs, _observe);

	//Simulate every fault.
	for (Fault<_valueType>* fault : faultsToSimulate) {
		std::vector<Event<_valueType>> faultActivationEvents = fault->go(); //Activate the fault
		if (fault->location()->value().faulty()) { //There is at least one faulty bit at the location.
			_simulationQueue.add(faultActivationEvents); 
			_simulationQueue.process();
		} else {
			fault->go(); //Imediately deactivate and continue to the next fault: there's no point in simulating.
			continue;
		}
				
		std::vector<_valueType> faultyOutputs = this->outputs(_circuit, _outputs, _observe);

		for (_valueType &val : faultyOutputs) {
			if (val.faulty() && val.faulty()) { //Detected
				this->undetectedFaults_.erase(fault);
				this->detectedFaults_.emplace(fault);
				break;
			}
		}

		_simulationQueue.add(fault->go()); //NOTE: "Go" will toggle the activation of the fault and return all new events from this (de)activation.
		//DEBUG: I want to know ALL stimulus generated by a fault alone.
		_simulationQueue.process();
	}

	//Reset the state of the circuit. //NOTE: is this step needed?
	this->Simulator<_valueType>::applyStimulus(_circuit, _stimulus, _simulationQueue, _inputs, _outputs, _observe);
	return goodOutputs;
}

template<class _valueType>
inline void FaultSimulator<_valueType>::setFaults(std::unordered_set<Fault<_valueType>*> _faults) {
	this->clearFaults();
	this->undetectedFaults_ = _faults;
}

template<class _valueType>
inline void FaultSimulator<_valueType>::resetFaults() {
	this->undetectedFaults_.insert(this->detectedFaults_.begin(), this->detectedFaults_.end());
	this->detectedFaults_.clear();
}



template<class _valueType>
inline std::unordered_set<Fault<_valueType>*> FaultSimulator<_valueType>::detectedFaults() {
	return this->detectedFaults_;
}

template<class _valueType>
inline std::unordered_set<Fault<_valueType>*> FaultSimulator<_valueType>::undetectedFaults() {
	return this->undetectedFaults_;
}

template<class _valueType>
inline float FaultSimulator<_valueType>::faultcoverage() {

	float fc = (float)detectedFaults_.size() / ((float)detectedFaults_.size() + (float)undetectedFaults_.size());
	return fc * 100;
}

template<class _valueType>
inline bool FaultSimulator<_valueType>::hasImpact(Fault<_valueType>* _fault) {
	_valueType curLineValue = _fault->location()->value();
	_valueType faultValue = _fault->value();
	return curLineValue != faultValue;
}

template<class _valueType>
inline bool FaultSimulator<_valueType>::notExcited(Fault<_valueType>* _fault) {
	_valueType curLineValue = _fault->location()->value();
	_valueType faultValue = _fault->value();
	if (curLineValue.valid() == false || faultValue.valid() == false) {
		return false;
	}
	return curLineValue.magnitude() == faultValue.magnitude();
}

template<class _valueType>
inline std::unordered_set<Fault<_valueType>*> FaultSimulator<_valueType>::clearFaults() {
	std::unordered_set<Fault<_valueType>*> toReturn = this->detectedFaults_;
	toReturn.insert(this->undetectedFaults_.begin(), this->undetectedFaults_.end());
	this->detectedFaults_.clear();
	this->undetectedFaults_.clear();
	return toReturn;

}


////////////////////////////////////////////////////////////////////////////////
// GARBAGE
////////////////////////////////////////////////////////////////////////////////
//DELETE:
// Every since "faulty values" are supported, these functions are no longer needed.
/*
 * Given a "faulty" value and the current line value, set what the faulty value
 * will be when active (i.e., modifiy it) as well as a mask of what values may
 * change in future fault simulation simulations (i.e., for TDFs).
 *
 * NOTE: inputs are modified through reference.
 *
 * @param _faultyValue The value to appear in the circuit during simulation.
 * @param _lineValue The value of the line in the circuit (where the fault is).
 * @param _mask The mask of values which may be affected for future iterations.
 * @param _same When calculating the mask, do we want unknown circuit values to
 *              be the same as or different than the fault? (FOR TDF: first vec
 *              is true, second vec is false.
 * @return Will the fault matter, i.e., will it have an effect on the circuit.
 */
 //template<class _valueType>
 //bool maskAndModify(
 //	_valueType & _faultyValue,
 //	_valueType & _lineValue,
 //	_primitive & _mask,
 //	bool _same
 //) {
 //	//Any non-valid values on the line need to be made the opposite magnitude as the fault.
 //	_primitive toForce = ~_lineValue.valid(); //The non-valid bits.
 //	_primitive toFlip;
 //	if (_same) {
 //		toFlip = _lineValue.magnitude() ^ _faultyValue.magnitude();
 //	}
 //	else {
 //		toFlip = _lineValue.magnitude() ^ ~(_faultyValue.magnitude());
 //	}
 //	toFlip = toFlip & toForce;
 //	_lineValue.magnitude(toFlip ^ _lineValue.magnitude());
 //
 //	if (_same) {
 //		_mask = ~(_faultyValue.magnitude() ^ _lineValue.magnitude()) & _mask;
 //	}
 //	else {
 //		_mask = (_faultyValue.magnitude() ^ _lineValue.magnitude()) & _mask;
 //	}
 //
 //	_primitive changedMagnitude = _mask; //It just so happened that the mask is the same as magnitudes the flip. Not necessarly the same as valids to flip.
 //	_primitive changedValids = ~_lineValue.valid();
 //	if (_same == false) {
 //		_faultyValue.magnitude(_lineValue.magnitude() ^ changedMagnitude);
 //		changedValids = (~changedValids ^ _faultyValue.valid()) & _mask;
 //		_faultyValue.valid(_lineValue.valid() ^ changedValids);
 //	}
 //	else {
 //		changedValids = changedValids & _mask;
 //		_faultyValue.valid(_faultyValue.valid() ^ changedValids);
 //	}
 //
 //
 //	return changedMagnitude | changedValids;
 //}
 //
 //template<>
 //bool maskAndModify(
 //	FaultyValue<bool> & _faultyValue,
 //	FaultyValue<bool> & _lineValue,
 //	bool & _mask,
 //	bool _same
 //) {
 //	//Any non-valid values on the line need to be made the opposite magnitude as the fault.
 //	bool toForce = !_lineValue.valid(); //The non-valid bits.
 //	bool toFlip;
 //	if (_same) {
 //		toFlip = _lineValue.magnitude() ^ _faultyValue.magnitude();
 //	}
 //	else {
 //		toFlip = _lineValue.magnitude() ^ !(_faultyValue.magnitude());
 //	}
 //	toFlip = toFlip & toForce;
 //	_lineValue.magnitude(toFlip ^ _lineValue.magnitude());
 //
 //	if (_same) {
 //		_mask = !(_faultyValue.magnitude() ^ _lineValue.magnitude()) & _mask;
 //	}
 //	else {
 //		_mask = (_faultyValue.magnitude() ^ _lineValue.magnitude()) & _mask;
 //	}
 //
 //	bool changedMagnitude = _mask; //It just so happened that the mask is the same as magnitudes the flip. Not necessarly the same as valids to flip.
 //	bool changedValids = !_lineValue.valid();
 //	if (_same == false) {
 //		_faultyValue.magnitude(_lineValue.magnitude() ^ changedMagnitude);
 //		changedValids = (!changedValids ^ _faultyValue.valid()) & _mask;
 //		_faultyValue.valid(_lineValue.valid() ^ changedValids);
 //	}
 //	else {
 //		changedValids = changedValids & _mask;
 //		_faultyValue.valid(_faultyValue.valid() ^ changedValids);
 //	}
 //
 //
 //	return changedMagnitude | changedValids;
 //}


#endif