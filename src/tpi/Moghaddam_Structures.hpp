/**
 * @file Moghaddam_Structures.hpp
 * @author Yang Sun (yzs0057@auburn.edu)
 * @version 0.1
 * @date 2019-09-13
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef Moghaddam_Structures_h
#define Moghaddam_Structures_h

#include <vector>

#include "faults/Fault.hpp"
#include "faults/FaultStructures.hpp"
#include "cop/COPStructures.hpp"
#include "COP_TPI_Structures.hpp"
#include "simulation/Function.hpp"
#include "simulation/SimulationStructures.hpp"



 /*
  * FEATURE: This object has the ability to "propigate" faults by way of the 
  * Moghaddam algorithm. For the specifics of the algorithm, see the article:
  *
  * https://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=8355918
  *
  * Objects of this class can create calculations which determine...
  * 1) The number of faults which can be propigated to a given location, and 
  * 2) The number of faults which will be blocked if a given line is 0/1.
  *
  * (NOTE: It is arguable if the calculated values are sound, but this feature
  *  is intended to reproduce the cited article's methods faithfully.)
  *
  * @param _primitive The width of simulation (bool or unsigned long long it).
  */
template <class _primitive>
class Moghaddam : virtual public COP, virtual public Faulty<_primitive>, virtual public Levelized {
public:
	/*
	 *Create a Propagate_fault object.
	 *
	 *@param (optional) Does this object always calculate on call? (default no)
	 */

	Moghaddam() : COP(), Faulty<_primitive>() {
		this->D_ = -1;
		this->B_ = std::vector<float>(2, -1);
	}

	/*
	 * Return the number of faults which can be propigated to this location.
	 *
	 * @return The number of faults propigated to the location.
	*/
	float D () {
		if (this->D_ >= 0) {
			return this->D_;
		}
		this->D_ = this->calculateD();
		if (this->D_ < 0) {
			throw "Failed to calculated the number of faults propigated.";
		}
		return this->D_;
	}

	/*
	 * Return the number of faults blocked if this line is 0/1.
	 *
	 * @param _blockingValue The value which will block faults.
	 * @return The number of faults blocked.
	 *
	 */
	float B(bool _blockingValue) {
		size_t index = _blockingValue ? 0 : 1;
		if (this->B_.at(index) >= 0) {
			return this->B_.at(index);
		}
		this->B_[index] = this->calculateB(_blockingValue);
		if (this->B_.at(index) < 0) {
			throw "Failed to calculated the number of faults propigated.";
		}
		return this->B_.at(index);
	}

protected:
	/*
	 * Call this object to calculate its propagated faults.
	 *
	 * @return The number of propagated faults.
	 */
	float calculateD() {
		float toReturn = (float) this->getfaults().size();

		//No inputs? Easy calculation.
		if (this->inputs().size() == 0) {
			return toReturn;
		}

		for (Connecting* input : this->inputs()) {
			Moghaddam<_primitive>* cast = dynamic_cast<Moghaddam<_primitive>*>(input);
			if (cast == nullptr) {
				throw "Input cannot propigate faults.";
			}

			float faultsToDistibute = cast->D();
			float sumCO = 0.0; //The sum of observabilities.
			for (Connecting* inputOutput : cast->outputs()) {
				cast = dynamic_cast<Moghaddam<_primitive>*>(inputOutput);
				if (cast == nullptr) {
					throw "Input's outputs cannot propigate faults.";
				}
				sumCO += cast->observability();
			}
			float thisObservability = this->observability();
			float distributedFaults = faultsToDistibute * (thisObservability / sumCO);
			toReturn += distributedFaults;
		}

		return toReturn;
	}

	/*
	 * Call this object to calculate its blocked faults.
	 *
	 * @param _blockingValue The value which blocks faults.
	 * @return The number of blocked faults.
	 */
	float calculateB(bool _blockingValue) {
		float toReturn = 0.0;
		for (Connecting* output : this->outputs()) {
			Moghaddam<_primitive>* cast = dynamic_cast<Moghaddam<_primitive>*>(output);
			if (cast == nullptr) {
				throw "Input cannot propigate faults.";
			}

			float forward = cast->forwardBlocking(_blockingValue);
			float lateral = cast->lateralBlocking(_blockingValue, this);
			toReturn += forward + lateral;
		}
		return toReturn;
	}

	/*
	 * Clear fault propgation values (on this and reliant objects).
	 */
	void clearD() {
		if (this->D_ < 0) { //Already cleared
			return;
		}
		this->D_ = -1;
		std::unordered_set<Connecting*> outputs = this->outputs();
		for (Connecting * output : outputs) {
			Moghaddam<_primitive>* cast = dynamic_cast<Moghaddam<_primitive>*>(output);
			cast->clearD();

			//Changing D can change b/B on "parallel" lines (e.g., the lines feeding the
			//same gate).
			for (Connecting * possiblyParralInput : output->inputs()) {
				if (possiblyParralInput != this) {
					Moghaddam<_primitive>* parallelInput = dynamic_cast<Moghaddam<_primitive>*>(possiblyParralInput);
					parallelInput->clearB();
				}
			}
		}
	}

	/*
	 * Clear fault blocking values (on this and reliant objects).
	 */
	void clearB() {
		//DEBUG printf("DBG CO CLEAR %s (%d outputs) ... ", this->name().c_str(), this->outputs().size());//DEBUG
		if (this->B_.at(0) < 0 && this->B_.at(1) < 0 ) { //Already cleared
			//DEBUG printf("STOP: %f %d\n", this->observability_, this->calculateAlways_);//DEBUG
			return;
		}
		this->B_ = std::vector<float>(2, -1);
		std::unordered_set<Connecting*> inputs = this->inputs();
		//DEBUG printf("DONE, forward to %d\n", inputs.size());
		for (Connecting * input : inputs) {
			Moghaddam<_primitive>* cast = dynamic_cast<Moghaddam<_primitive>*>(input);
			if (cast != nullptr) {
				//A SPECIAL NOTE:
				//This is one of the few places where casting to a nullptr is allowed. When a node is deleted, it will
				//call "clear D/B" on itself. The controllability call will lead to clear
				//B calls coming back to it, but by this time, the node will no longer be considered "Moghaddam<_primitive>"
				//(since it is in the process of being deconstructed). To avoid this, nullptrs are skipped.
				cast->clearB();
			}
		}
	}

	/*
	 * Return the number of blocked faluts which will be forwarded to a calling
	 * object.
	 *
	 * This function is implicitly defined for individual objects (i.e., Lines
	 * and nodes).
	 *
	 * @param _blockingValue The value which blocks faults.
	 * @return The number of blocked faults.
	 */
	virtual float forwardBlocking(bool _blockingValue) = 0;

	/*
	 * Return the number of blocked faluts which will be forwarded to a calling
	 * object through latteral blocking.
	 *
	 * This function is implicitly defined for individual objects (i.e., Lines
	 * and nodes).
	 *
	 * @param _blockingValue The value which blocks faults.
	 * @param _calling The calling object.
	 * @return The number of blocked faults.
	 */
	virtual float lateralBlocking(bool _blockingValue, Moghaddam<_primitive>* _calling) = 0;

private:
	/*
	 * The number of faults propigated to this location
	 *
	 * Negative -> not yet known.
	 */
	float D_;

	/*
	 * The number of faults blocked with this object is 0/1.
	 *
	 * Negative -> not yet known.
	 */
	std::vector<float> B_;

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
	virtual void removeInputConnection(Connection* _rmv, bool _deleteConnection = true) {
		this->clearD();
		this->COP::removeInputConnection(_rmv, _deleteConnection);
	}

	/*
	 * Delete a given output connection
	 *
	 * The given output connection will be removed. If it does not exist, an
	 * exception will be thrown.
	 *
	 * @param The output connection to remove.
	 */
	virtual void removeOutputConnection(Connection* _rmv, bool _deleteConnection = true) {
		this->clearB();
		this->COP::removeOutputConnection(_rmv, _deleteConnection);
	}

	/*
	 * Add a given input connection
	 *
	 * @param Input connection to add
	 */
	virtual void addInputConnection(Connection* _add) {
		this->clearD();
		this->COP::addInputConnection(_add);
	}

	/*
	 * Add a given output connection
	 *
	 * @param Output connection to add
	 */
	virtual void addOutputConnection(Connection* _add) {
		this->clearB();
		this->COP::addOutputConnection(_add);
	}
	
};


/*
 * This Line implements all functions necessary to perform Moghaddam's TPI
 * algorithm.
 *
 * @param _primitive The width of simulation (bool or unsigned long long it).
 */
template <class _primitive>
class Moghaddam_Line: public COP_TPI_Line<_primitive>, virtual public Moghaddam<_primitive> {
public:
	/*
	 * Create a Moghaddam_Line with a "UNDEF" name.
	 */
	Moghaddam_Line() : COP_TPI_Line<_primitive>(), Moghaddam<_primitive>() {
	}

	/*
	 * Create a Line with a given name
	 *
	 * @param _name The name of the Line
	 */
	Moghaddam_Line(std::string _name) : COP_TPI_Line<_primitive>(_name), Moghaddam<_primitive>(), Connecting(_name) {

	}

protected:
	/*
	 * Call this Line to calculate its controllability.
	 *
	 * For a Line, the controllability is equal to the controllability of the
	 * driving object (whether it's a Node or a Line).
	 *
	 * @return The controllability calculated.
	 */
	virtual float calculateControllability() {
		return this->COP_TPI_Line<_primitive>::calculateControllability();
	}

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
	virtual float calculateObservability(COP* _calling = nullptr) {
		return this->COP_TPI_Line<_primitive>::calculateObservability(_calling);
	}

	/*
	 * Return the number of blocked faluts which will be forwarded to a calling
	 * object.
	 *
	 * This function is implicitly defined for individual objects (i.e., Lines
	 * and nodes).
	 *
	 * @param _blockingValue The value which blocks faults.
	 * @return The number of blocked faults.
	 */
	virtual float forwardBlocking(bool _blockingValue) {
		return this->B(_blockingValue);
	}

	/*
	 * Return the number of blocked faluts which will be forwarded to a calling
	 * object through latteral blocking.
	 *
	 * This function is implicitly defined for individual objects (i.e., Lines
	 * and nodes).
	 *
	 * @param _blockingValue The value which blocks faults.
	 * @param _calling The calling object.
	 * @return The number of blocked faults.
	 */
	virtual float lateralBlocking(bool _blockingValue, Moghaddam<_primitive>* _calling) {
		if (this->inputs().size() > 1) {
			throw "Something is seriously wrong here: a line cannot have more than one input.";
		}
		return 0.0;
	}

};


/*
 * This Node implements all functions necessary to perform Moghaddam's TPI
 * algorithm.
 *
 * @param _primitive The width of simulation (bool or unsigned long long it).
 */
template <class _primitive>
class Moghaddam_Node : public COP_TPI_Node<_primitive>, virtual public Moghaddam<_primitive> {
public:

	/*
	 * Create a node with no inputs, no outputs, and a "copy" function.
	 */

	Moghaddam_Node() :
		COP_TPI_Node<_primitive>(true),
		Moghaddam<_primitive>(),
		Connecting()
    {

	}

	/*
	 * Create a Node which supports Moghaddam TPI.
	 *
	 * @param _function The function of this node (nullptr = "copy node")
	 * @param (optional) _name The name of the node.
	 * @param (optional) _inputs Input lines to the given node.
	 * @param (optional) _outputs Output lines to the given node.
	 */
	Moghaddam_Node(
		Function<_primitive>* _function,
		std::string _name,
		std::unordered_set<Moghaddam_Line<_primitive>*> _inputs = std::unordered_set<Moghaddam_Line<_primitive>*>(),
		std::unordered_set<Moghaddam_Line<_primitive>*> _outputs = std::unordered_set<Moghaddam_Line<_primitive>*>()
	):
		COP_TPI_Node<_primitive>(
			_function,
			_name,
			std::unordered_set<COP_TPI_Line<_primitive>*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<COP_TPI_Line<_primitive>*>(_outputs.begin(), _outputs.end())
			),
		Valued<_primitive>(
			_function
			),
		Moghaddam<_primitive>(),
		Connecting(
			std::unordered_set<Connecting*>(_inputs.begin(), _inputs.end()),
			std::unordered_set<Connecting*>(_outputs.begin(), _outputs.end()),
			_name
		)
	{
	}

protected:

	/*
	 * Call this Node to calculate its controllability.
	 *
	 * For a Node, the controllability is calculated using the input Lines and
	 * the Node's function.
	 *
	 * @return The controllability calculated.
	 */
	virtual float calculateControllability() {
		return this->COP_TPI_Node<_primitive>::calculateControllability();
	}

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
	virtual float calculateObservability(COP* _calling = nullptr) {
		return this->COP_TPI_Node<_primitive>::calculateObservability(_calling);
	}

	/*
	* Return the number of blocked faluts which will be forwarded to a calling
	* object.
	*
	* This function is implicitly defined for individual objects (i.e., Lines
	* and nodes).
	*
	* @param _blockingValue The value which blocks faults.
	 * @return The number of blocked faults.
	*/
	virtual float forwardBlocking(bool _blockingValue) {
		Function<_primitive>* function = this->function();
		if (function == nullptr) { //This is a pi, po, or const
			return 0.0;
		}

		std::string functionName = function->string();
		if (functionName == "const" || functionName == "po" || functionName == "xor" || functionName == "xnor") {
			return 0.0;
		}
		if (functionName == "buf") {
			return this->B(_blockingValue);
		}
		if (functionName == "not") {
			return this->B(!_blockingValue);
		}
		if (functionName == "or" ) {
			return _blockingValue == true ? this->B(_blockingValue) : 0.0;
		}
		if (functionName == "nor") {
			return _blockingValue == true ? this->B(!_blockingValue) : 0.0;
		}
		if (functionName == "and") {
			return _blockingValue == false ? this->B(_blockingValue) : 0.0;
		}
		if (functionName == "nand") {
			return _blockingValue == false ? this->B(!_blockingValue) : 0.0;
		}
		throw "No valid gate type for blocking faults found.";
		return 0.0;
	}

	/*
	 * Return the number of blocked faluts which will be forwarded to a calling
	 * object through latteral blocking.
	 *
	 * This function is implicitly defined for individual objects (i.e., Lines
	 * and nodes).
	 *
	 * @param _blockingValue The value which blocks faults.
	 * @param _calling The calling object.
	 * @return The number of blocked faults.
	 */
	virtual float lateralBlocking(bool _blockingValue, Moghaddam<_primitive>* _calling) {
			Function<_primitive>* function = this->function();
			
			if (function == nullptr) {//This is a po, pi, or const.
				return 0.0;
			}

			std::string functionName = function->string();
			if (functionName == "const" || functionName == "po" || functionName == "buf" || functionName == "not" || functionName == "xor" || functionName == "xnor") {
				return 0.0;
			}
			if ((functionName == "or" || functionName == "nor") && (!_blockingValue)) {
				return 0.0;
			}
			if ((functionName == "and" || functionName == "nand") && (_blockingValue)) {
				return 0.0;
			}

			float toReturn = 0.0;
			for (Connecting * input : this->inputs()) {
				if (input == _calling) {
					continue;
				}
				Moghaddam<_primitive>* cast = dynamic_cast<Moghaddam<_primitive>*>(input);
				if (cast == nullptr) {
					throw "Input cannot propigate faults.";
				}
				toReturn += cast->D();
			}

			return toReturn;
	}

};

#endif FaultPropigation_Structures_h
