/**
 * @file COPStructures.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-02-04
 *
 * @copyright Copyright (c) 2019
 *
 */


#ifndef COPSTructures_h
#define COPSTructures_h

#include "simulation/SimulationStructures.hpp"
#include "simulation/ValueVectorFunctions.hpp"
#include <cmath>

 /*
  * Controllability and observability information.
  *
  * It is presumed that all input and output connections are of the COP type.
  *
  * This is a pure virtual class. Inheriting objects must designate how
  * controllability and obserability are calculated.
  */
class COP : virtual public Levelized {
public:
	/*
	 * Create a COP object.
	 *
	 * @bool (optional) Does this object always calculate on call? (default no)
	 */
	COP();



	/*
	 * Return the controllablity of the object (and calculate it if necessary).
	 *
	 * Will throw an exception if the value cannot be calculated (no objects to
	 * calculate from, failure to calculate).
	 *
	 * @return The controllability of the object.
	 */
	float controllability();

	/*
	 * Manually set the controllability of the object.
	 *
	 * @param The controllability value to set to.
	 * @return The controllability of the object (after setting).
	 */
	float controllability(float _controllability);

	/*
	 * Return the observability of the object (and calculate it if necessary).
	 *
	 * Since the observability calculation for an object can depend on the object
	 * being calcualted for (the "fan out problem"), the calling object may be
	 * required. This is because an object may not have a "single observability".
	 * A nullptr may be acceptable if a deterministic observability value is
	 * possible.
	 *
	 * Will throw an execption if the value cannot be calculated.
	 *
	 * If the COP object was created as "always calculate", then the observability
	 * will always be recalculated unless a valid observability was manually set.
	 *
	 * @param (optional) The object calling to calculate observability.
	 * @return The observability of the object.
	 */
	float observability(COP* _calling = nullptr);

	/*
	 * Manually set the observability of the object.
	 *
	 * @param The observability value to set to.
	 * @return The observability of the object (after setting).
	 */
	float observability(float _observability);


	/*
	 * Clear this object's observability and any observabilities that rely on this
	 * object.
	 */
	virtual void clearObservability();

	/*
	 * Clear this object's controllability and any controllabilities that rely on
	 * this object.
	 */
	virtual void clearControllability();



protected:

	/*
	 * Call this object to calculate its controllability.
	 *
	 * This function is implicitly meant to be overwritten by the inherited
	 * object.
	 *
	 * @return The controllability calculated.
	 */
	virtual float calculateControllability() = 0;

	/*
	 * Call this object to calculate the calling's observability.
	 *
	 * This function is implicitly meant to be overwritten by the inherited
	 * object.
	 *
	 * @param The calling object to calculate the observability for.
	 * @return The controllability calculated.
	 */
	virtual float calculateObservability(COP* _calling = nullptr) = 0;

	/*
	 * Because adding/removing inputs/outputs can change CC&CO values, Connecting
	 * functions need to be amended. The original Connecting functions will be
	 * forwarded after CC&CO values are cleared.
	 */

	 /*
	  * Delete a given input connection
	  *
	  * The given input connection will be removed. If it does not exist, an
	  * exception will be thrown.
	  *
	  * @param The input connection to remove.
	 */
	virtual void removeInputConnection(Connection* _rmv, bool _deleteConnection = true);

	/*
	 * Delete a given output connection
	 *
	 * The given output connection will be removed. If it does not exist, an
	 * exception will be thrown.
	 *
	 * @param The output connection to remove.
	 */
	virtual void removeOutputConnection(Connection* _rmv, bool _deleteConnection = true);

	/*
	 * Add a given input connection
	 *
	 * @param Input connection to add
	 */
	virtual void addInputConnection(Connection* _add);

	/*
	 * Add a given output connection
	 *
	 * @param Output connection to add
	 */
	virtual void addOutputConnection(Connection* _add);

	/*
	 * Does this COP object always calculate when called?
	 */
	bool calculateAlways_;

private:
	/*
	 * The current controllability value of this object (negative = undefined).
	 */
	float controllability_;

	/*
	 * The current observability value of this object (negative = undefined).
	 */
	float observability_;

};

/*
 * A Line which holds information necessary to perform COP.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class COPLine : public virtual SimulationLine<_valueType>, virtual public COP {
public:

	/*
	 * Create a COPLine with a "UNDEF" name.
	 */
	COPLine();

	/*
	 * Create a Line with a given name
	 *
	 * @param _name The name of the Line
	 */
	COPLine(std::string _name);


	/*
 * Return Create an copy of this object.
 *
 *
 *
 * @return new COPline.
 */
	virtual Connecting* clone();
protected:

	/*
	 * Call this Line to calculate its controllability.
	 *
	 * For a Line, the controllability is equal to the controllability of the
	 * driving object (whether it's a Node or a Line).
	 *
	 * @return The controllability calculated.
	 */
	virtual float calculateControllability();

	/*
	 * Call this object to calculate the calling's observability.
	 *
	 * For a Line, the observability is equal to the maximum of all output
	 * observabilities, regardless of what they're from (Nodes or Lines).
	 *
	 * The parameter (calling object) will be ignored.
	 *
	 * @param The calling object (Line) to calculate the observability for.
	 * @return The controllability calculated.
	 */
	virtual float calculateObservability(COP* _calling = nullptr);

};

/*
 * A Node which holds information necessary to perform COP.
 *
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _valueType>
class COPNode : public virtual SimulationNode<_valueType>, virtual public COP {
public:
	/*
	 * Create a node with no inputs, no outputs, and a "copy" function.
	 */
	COPNode() : SimulationNode<_valueType>(), COP(), Connecting() {

	}

	/*
	 * Create a Boolean node.
	 *
	 * @param _function The function of this node (nullptr = "copy node")
	 * @param (optional) _name The name of the node.
	 * @param (optional) _inputs Input lines to the given node.
	 * @param (optional) _outputs Output lines to the given node.
	 */
	COPNode(
		Function<_valueType>* _function,
		std::string _name = "UNDEF",
		std::unordered_set<COPLine<_valueType>*> _inputs = std::unordered_set<COPLine<_valueType>*>(),
		std::unordered_set<COPLine<_valueType>*> _outputs = std::unordered_set<COPLine<_valueType>*>()
	) :
		SimulationNode<_valueType>(
			_function,
			_name,
			std::unordered_set<SimulationLine<_valueType>*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<SimulationLine<_valueType>*>(_outputs.begin(), _outputs.end())
			),
		COP(),
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
	 * Create a copy of this node.
	 *
	 * @return A new COPNode.
	 */
	virtual Connecting* clone();

protected:

	/*
	 * Call this Node to calculate its controllability.
	 *
	 * For a Node, the controllability is calculated using the input Lines and
	 * the Node's function.
	 *
	 * @return The controllability calculated.
	 */
	virtual float calculateControllability();

	/*
	 * Call this object to calculate the calling's observability.
	 *
	 * For a Node, the observability is calculated
	 *
	 * The parameter (calling object) will be ignored.
	 *
	 * @param The calling object (Line) to calculate the observability for.
	 * @return The controllability calculated.
	 */
	virtual float calculateObservability(COP* _calling = nullptr);


};

////////////////////////////////////////////////////////////////////////////////
// Inline function declarations.
////////////////////////////////////////////////////////////////////////////////

inline COP::COP() {
	this->controllability_ = -1;
	this->observability_ = -1;
}


inline float COP::controllability() {
	if (this->controllability_ >= 0) {
		//DEBUG printf("DBG CC %s (%d outputs) = %f (no calc)\n", this->name().c_str(), this->outputs().size(), this->controllability_);//DEBUG
		return this->controllability_;
	}

	this->controllability_ = this->calculateControllability();
	if (this->controllability_ < 0) {
		throw "Cannot calculate COP CC: failure to calculate.";
	}
	//DEBUG printf("DBG CC %s (%d outputs) = %f (calc)\n", this->name().c_str(), this->outputs().size(), this->controllability_);//DEBUG
	return this->controllability_;
}

inline float COP::controllability(float _controllability) {
	this->controllability_ = _controllability;
	return this->controllability_;
}

inline float COP::observability(COP* _calling) {
	if (this->observability_ >= 0) {
		//DEBUG printf("DBG CO %s (%d outputs) = %f (no calc)\n", this->name().c_str(), this->outputs().size(), this->observability_);//DEBUG
		return this->observability_;
	}

	float toReturn = this->calculateObservability(_calling);
	if (toReturn < 0) {
		throw "Cannot calculate COP CO: failure to calculate.";
	}

	if (this->inputs().size() <= 1) {//If an object has more than one input, it's returned observability may change depending on who's calling.
		//Otherwise, save the observability for later.
		this->observability_ = toReturn;
	}
	return toReturn;
}

inline float COP::observability(float _observability) {
	this->observability_ = _observability;
	return this->observability_;
}

inline void COP::clearObservability() {
	if (this->name() == "po" || (this->name().find("observe") != std::string::npos)) {
		return;
	}
	//DEBUG printf("DBG CO CLEAR %s (%d outputs) ... ", this->name().c_str(), this->outputs().size());//DEBUG
	if (this->observability_ < 0 && this->inputs().size() <= 1) { //Already cleared (and should calculate).
		//DEBUG printf("STOP: %f %d\n", this->observability_, this->calculateAlways_);//DEBUG
		return;
	}
	this->observability_ = -1;
	std::unordered_set<Connecting*> inputs = this->inputs();
	//DEBUG printf("DONE, forward to %d\n", inputs.size());
	for (Connecting * input : inputs) {
		COP* cast = dynamic_cast<COP*>(input);
		if (cast != nullptr) {
			/*A SPECIAL NOTE:
			This is one of the few places where casting to a nullptr is allowed. When a node is deleted, it will
			call "clear observability/controllability" on itself. The controllability call will lead to clear
			observability calls coming back to it, but by this time, the node will no longer be considered "COP"
			(since it is in the process of being deconstructed). To avoid this, nullptrs are skipped. 
			*/
			cast->clearObservability();
		}
	}
}

inline void COP::clearControllability() {
	if (this->name() == "pi" || (this->name().find("const") != std::string::npos)) {
		return;
	}
	if (this->controllability_ < 0) { //Already cleared
		return;
	}
	//DEBUG printf("DBG CC CLEAR %s (%d outputs)\n", this->name().c_str(), this->outputs().size());		//DEBUG
	this->controllability_ = -1;
	std::unordered_set<Connecting*> outputs = this->outputs();
	for (Connecting * output : outputs) {
		COP* cast = dynamic_cast<COP*>(output);
		cast->clearControllability();

		//Change CC can change CO on "parallel" lines (e.g., the lines feeding the
		//same gate).	 NOTE: see deleted segment below.
		for (Connecting * possiblyParralInput : output->inputs()) {
			if (possiblyParralInput != this) {
				COP* parallelInput = dynamic_cast<COP*>(possiblyParralInput);
				parallelInput->clearObservability();
			}
		}
	}

	//DELETE, bad performance: Because the clearControllability call will always go forward,
	//the clearObservability call be be done directly here instead of finding
	//what objects are "in parallel" with this one.
	//this->clearObservability();
}

inline void COP::removeInputConnection(Connection * _rmv, bool _deleteConnection) {
	this->clearControllability();
	this->Levelized::removeInputConnection(_rmv, _deleteConnection);
}

inline void COP::removeOutputConnection(Connection * _rmv, bool _deleteConnection) {
	this->clearObservability();
	this->Levelized::removeOutputConnection(_rmv, _deleteConnection);
}

inline void COP::addInputConnection(Connection * _add) {
	this->clearControllability();
	this->Levelized::addInputConnection(_add);
}

inline void COP::addOutputConnection(Connection * _add) {
	this->clearObservability();
	this->Levelized::addOutputConnection(_add);
}

template<class _valueType>
inline COPLine<_valueType>::COPLine() :
	COP(),
	SimulationLine<_valueType>() {
}

template<class _valueType>
inline COPLine<_valueType>::COPLine(std::string _name) :
	COP(),
	SimulationLine<_valueType>(_name),
	Connecting(_name) {
}
template<class _valueType>
inline float COPLine<_valueType>::calculateControllability() {
	if (this->inputs().size() != 1) {
		throw "Cannot calculate COPLine controllability: need exactly 1 input.";
	}
	COP* cast = dynamic_cast<COP*>(*(this->inputs().begin()));
	float toReturn = cast->controllability();
	if (toReturn > 1 || toReturn < 0) {
		throw "CO fail.";
	}
	return toReturn;
}

template<class _valueType>
inline float COPLine<_valueType>::calculateObservability(COP * _calling) {
	if (this->outputs().size() == 0) {
		return 0.0;
		//DELETE: incorrect. throw "A line with no outputs cannot calculate its observability.";
	}
	float toReturn = 1;
	for (Connecting* output : this->outputs()) {
		COP* cast = dynamic_cast<COP*>(output);
		float other = cast->observability(this);
		toReturn *= (1 - other);
		//DELETE: flawed. toReturn = other > toReturn ? other : toReturn;
	}
	toReturn = 1 - toReturn;
	if (toReturn > 1 || toReturn < 0) {
		throw "CO fail.";
	}
	return toReturn;
}

template<class _valueType>
inline Connecting* COPLine<_valueType>::clone() {
	return new COPLine<_valueType>(this->name());
}

/*
 * Count the number of 1s in a number
 *
 * https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive
 */
 //NOTE: this function is not actually used for anything, but it's kept here for reference.
template<class _width>
inline unsigned long int numOnes(_width v) {
	unsigned long int c; // c accumulates the total bits set in v
	for (c = 0; v; c++) {
		v &= v - 1; // clear the least significant bit set
	}
	return c;
}

/*
 * Manually calculate COP for a given node.
 */
template<class _valueType>
inline float manualCOP(COPNode<_valueType>* node, std::vector<float> inputControllabilities) {
	Function<_valueType>* func = dynamic_cast<Function<_valueType>*>(node->function());
	//TODO: what's the "initial" value?
	std::vector<_valueType> nodeInputVals = std::vector<_valueType>(node->inputs().size(), _valueType(0xFFFFFFFFFFFFFFFF));
	ValueVectorFunction<_valueType>::increment(nodeInputVals);
	float ret = 0.0;
	do {
		_valueType outVal = func->evaluate(nodeInputVals);
		auto valid = outVal.valid();		
		auto magnitude = outVal.magnitude();
		auto inputMask = valid; inputMask = 0x1; //Which input values are we currently looking at? //NOTE: is there a more eligant way to make this a "1 of a right size"?
		while (valid) {//There's at least one valid output to measure.
			if (valid & magnitude & 0x1) { //We have a valid 1, so calculate the prob. of it occuring.
				float prob = 1;
				for (size_t a = 0; a < nodeInputVals.size(); a++) {
					if (nodeInputVals.at(a).magnitude() & inputMask) { //Input is a 1
						prob = prob * (inputControllabilities.at(a));
					}
					else { //Input is a 0
						prob = prob * (1 - inputControllabilities.at(a));
					}
				}
				ret = ret + prob;
			}
			valid = valid >> 1;
			magnitude = magnitude >> 1;
			inputMask = inputMask << 1;
		}

	} while (ValueVectorFunction<_valueType>::increment(nodeInputVals) == true);

	return ret;
}

template<class _valueType>
inline float COPNode<_valueType>::calculateControllability() {
	//First, get the input controllabilities.
	std::vector<float> inputControllabilities;
	for (Connecting* input : this->inputs()) {
		COP* cast = dynamic_cast<COP*>(input);
		inputControllabilities.push_back(cast->controllability());
	}

	//Second, calculate depending on the gate type.
	float toReturn = 1.0;
	std::string functionName = this->name();
	if (functionName == "pi") {
		return 0.5;
	}
	else if (this->name().find("const") != std::string::npos) {
		_valueType value = this->value();
		if (value.valid() == false) {
			throw "Cannot calculate COP CC: constant value is not valid.";
		}
		if (value.magnitude() == false) {
			return 0.0;
		}
		return 1.0;
	}
	else if (//The controllability depends on the probability of all 1's?
		functionName == "and" ||
		functionName == "nand" ||
		functionName == "buf" ||
		functionName == "not" ||
		functionName == "po"
		) {
		for (float value : inputControllabilities) {
			toReturn *= value;
		}
	}
	else if (//The controllability depends on the probability of all 0's?
		functionName == "or" ||
		functionName == "nor"
		) {
		for (float value : inputControllabilities) {
			toReturn *= (1 - value);
		}
	}
	else { //Manual evaluation is needed.
		 toReturn=manualCOP(this, inputControllabilities);
		 if (toReturn > 1)
		 {
			 throw "error >1";
		 }
	}

	if (
		functionName == "nand" ||
		functionName == "or" ||
		functionName == "not"
		) {
		toReturn = 1 - toReturn;
	}
	if (toReturn > 1)
	{
		throw "error >1";
	}
	return toReturn;
}

template<class _valueType>
inline float COPNode<_valueType>::calculateObservability(COP * _calling) {
	if (this->name() == "po" || (this->name().find("observe") != std::string::npos)) {
		return 1.0;
	}
	if ((this->name().find("const") != std::string::npos)) {
		return 0.0;
	}
	float pPass = 1.0;  //The probability that a signal will pass through the given gate.
	if (this->name() == "and" || this->name() == "nand") {
		for (Connecting* input : this->inputs()) {
			if (input == _calling) {
				continue;
			}
			COP* cast = dynamic_cast<COP*>(input);
			pPass *= cast->controllability();
		}
	}
	else if (this->name() == "or" || this->name() == "nor") {
		for (Connecting* input : this->inputs()) {
			if (input == _calling) {
				continue;
			}
			COP* cast = dynamic_cast<COP*>(input);
			pPass *= 1 - cast->controllability();
		}
	}



	COP* outputLine = dynamic_cast<COP*>(*(this->outputs().begin()));
	float outputObservability = outputLine->observability(this);
	float toReturn = pPass * outputObservability;
	if (toReturn < 0 || toReturn > 1) {
		throw "Impossible pPass calculation.";
	}
	return toReturn;
}



template<class _valueType>
inline Connecting* COPNode<_valueType>::clone() {
	return new COPNode<_valueType>(this->function());
}




#endif