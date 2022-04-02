/**
 * @file COP_fault_calculator.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-02-21
 *
 * @copyright Copyright (c) 2019
 *
 */


#ifndef COP_fault_calculator_h
#define COP_fault_calculator_h

#include "structures/Circuit.h"
#include "faults/Fault.hpp"
#include "faults/FaultGenerator.hpp"
#include "cop/COPStructures.hpp"

/*
 * Calculates the fault coverage of a circuit using COP values.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class COP_fault_calculator {
public:
	/*
	 * Create the calculator for a given Circuit
	 *
	 * @param _circuit The circuit to calculate for.
	 */
	COP_fault_calculator(Circuit* _circuit, bool stuck_target,int v, std::unordered_set<Fault<_valueType>*> _faults = std::unordered_set<Fault<_valueType>*>());

	/*
	 * Upon deletion, delete all faults.
	 */
	~COP_fault_calculator();


	/*
	 * The predicted fault coverage
	 *
	 * @return The predicted fault coverage.
	 */
	float faultCoverage();

	/*
	 * Clear the calculator's list of faults (will NOT delete).
	 *
	 * @return Faults which are not forgotten (which should be deleted elsewhere.
	 */
	std::unordered_set<Fault<_valueType>*> clearFaults();

private:
	/*
	 * The faults for this calculator.
	 */
	std::unordered_set<Fault<_valueType>*> faults_;

	/*
	 * The probability a given fault is detected.
	 *
	 * @param _fault The fault to detect.
	 * @return The probability of detection.
	 */
	float detect(Fault<_valueType>* _fault);

	/*
	*if it is true use stuck target cop. if it is false use delay target cop
	*/

	bool stuck_target_;
	int v_;
};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////

template <class _valueType>
COP_fault_calculator<_valueType>::COP_fault_calculator(Circuit * _circuit, bool stuck_target,int v, std::unordered_set<Fault<_valueType>*> _faults) {
	if (_faults.size() == 0) {
		this->faults_ = FaultGenerator<_valueType>::allFaults(_circuit);
	}
	else {
		this->faults_ = _faults;
	}

	this->stuck_target_ = stuck_target;
	this->v_ = v;
}

template <class _valueType>
COP_fault_calculator<_valueType>::~COP_fault_calculator() {
	//WE DO NOT DELETE FAULTS, but we should check if they weren't.
	if (this->faults_.size() != 0) {
		//printf("WARNING: a fault calculator's faults were not deleted ???");
	}
}

template <class _valueType>
float COP_fault_calculator<_valueType>::faultCoverage() {
	float toReturn = 0.0;
	for (Fault<_valueType>* fault : this->faults_) {
		toReturn += this->detect(fault);
	}
	toReturn /= (float)this->faults_.size();
	return toReturn;
}

template <class _valueType>
std::unordered_set<Fault<_valueType>*> COP_fault_calculator<_valueType>::clearFaults() {
	std::unordered_set<Fault<_valueType>*> toReturn = this->faults_;
	this->faults_.clear();
	return toReturn;
}

template <class _valueType>
float COP_fault_calculator<_valueType>::detect(Fault<_valueType>* _fault) {
	Faulty<_valueType>* location = _fault->location();
	COP* cast = dynamic_cast<COP*>(location);
	if (cast == nullptr) {
		throw "Cannot calculate fault-based fault coverage: COP not enabled on the fault location.";
	}
	auto SAValue = _fault->value().magnitude();
	float CC = cast->controllability();
	float CO = cast->observability();
	if (SAValue) {
		CC = 1 - CC;
	}
	//DEBUG printf("DBG Fault: %s (%d outputs) sa%d -> %f\n", _fault->location()->name().c_str(), _fault->location()->outputs().size(), SAValue, CC*CO); //DEBUG
	if (stuck_target_ == true)
	{
		return 1-pow((1-CC * CO),v_);
		//return CC * CO;
	}
	else
	{
		return 1 - pow((1 - CC * CO*(1 - CC)), v_);
	}
}



#endif
