/**
 * @file Combination.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-01-14
 *
 * @copyright Copyright (c) 2019
 *
 */

#ifndef Combination_h
#define Combination_h

#include <unordered_set>
#include <vector>

#include "simulation/Value.hpp"
#include "simulation/ValueVectorFunctions.hpp"
#include "simulation/Function.hpp"
#include "sat/Goal.hpp"

/*
 * A combination of circuit Lines at a set of Values in a circuit.
 *
 * A Combination is a special type of 'Node' which represents a combination of
 * particular objects in a circuit at particular Values.
 *
 * During simulation, if the combination is updated to the designated Values,
 * then the input connections to the combination will be removed.
 *
 * @param _lineType The type of lines that ndoes are connected to.
 * @param _nodeType The type of node implemented as the "polling node" for this
 *        Combination. It should ideally be the same kind of node as
 *        implemented in the rest of the Circuit.
 * @param _valueType The type of value being simulated, e.g., Value/FaultyValue
 */
template <class _lineType, class _nodeType, class _valueType>
class Combination : public _nodeType, public Goal<_valueType> {
public:
	/*
	 * By default, a Combination has no Lines and no Values, which will be
	 * interpreted as not-seen.
	 */
	Combination() {
	}

	/*
	 * Create a Combination of a given set of lines at a given vector of Values.
	 *
	 * It is not valid to have a combination with no Lines or Values, or to have
	 * the number of Lines and Values not match.
	 *
	 * @param _lines The Lines to measure values on.
	 * @param _values The Values to be measured.
	 * @param _seen (optional) If set, this parameter is considered "already seen".
	 */
	Combination(
		std::vector<_lineType*> _lines,
		std::vector<_valueType> _values,
		bool _seen = false
	) : _nodeType(
		nullptr, "COMBINATION"
		) {
		lines_ = _lines;
		values_ = _values;
		if (lines_.size() != values_.size()) {
			throw "Lines and Values must match in size.";
		}
		if (lines_.size() == 0) {
			throw "At least one Line must be given";
		}
		if (_seen == true) {
			return;
		}
		for (_lineType* line : _lines) {
			//The combination will only be "connected" to the circuit if it is 
			//not-yet-seen (by default, this will run).
			line->addOutput(this);
		}
	};

	/*
	 * Create a seen Combination of a given set of lines, and get the values from the lines.
	 *
	 * It is presumed that this combination has been seen, and therefore no circuit
	 * connections will be made.
	 *
	 * EXCEPTION: At least one line must be given.
	 *
	 * @param _lines The lines to measure values on which currently hold their needed values.
	 */
	Combination(
		std::vector<_lineType*> _lines
	) : _nodeType(
	) {
		this->lines_ = _lines;
		this->values_ = std::vector<_valueType>();
		for (_lineType* line : _lines) {
			this->values_.push_back(line.value());
		}
		if (lines_.size() == 0) {
			throw "At least one line must be given";
		}
	};

	/*
	 * Create a seen Combination of a given set of nodes, and get the values from the nodes.
	 *
	 * It is presumed that this combination has been seen, and therefore no circuit
	 * connections will be made.
	 *
	 * EXCEPTION: At least one node must be given.
	 *
	 * @param _nodes The nodes to measure values on which currently hold their needed values.
	 */
	Combination(
		std::vector<_nodeType*> _nodes
	) : _nodeType(
	) {
		std::vector<_lineType*> lines;
		for (_nodeType* node : _nodes) {
			if (node->outputs().size() != 1) {
				throw "Cannot make a combination from nodes which do not have 1 output.";
			}
			_lineType* outputLine = dynamic_cast<_lineType*>(*(node->outputs().begin()));
			if (outputLine == nullptr) {
				throw "Combination line is not of the right type.";
			}
			lines.push_back(outputLine);
		}

		this->lines_ = lines;
		this->values_ = std::vector<_valueType>();
		for (_lineType* line : this->lines_) {
			this->values_.push_back(line->value());
		}
		if (lines_.size() == 0) {
			throw "At least one node must be given";
		}
	};

	/*
	 * Check if this combination has been seen. If so, remove its connections.
	 *
	 * An exception will be thrown if the size of the given Values is different
	 * than the Combination's Values.
	 *
	 * If the value has already been seen, a default value will be returned.
	 *
	 * NOTE: The returned value is in-effect useless. This is because this
	 *       function overwrites the inherited value to allow "seen" values to
	 *       be easily implemented.
	 *
	 * NOTE: Since the values given (_values) cannot be gauranteed to be in the
	 *       same order, the given values will be ignored. Instead, values will be
	 *       fetched manaully by this function.
	 *
	 * @param _values The Values to evaluate using this object's function.
	 * @return The evaluated value of this object (will always be the default Value).
	 */
	virtual _valueType value(std::vector<_valueType> _values) {
		if (this->inputs().size() == 0) {
			return _valueType();
			//DELETE: flawed. throw "This combination has already been seen.";
		}
		if (_values.size() != values_.size()) {
			throw "The size of values must match";
		}
		for (size_t i = 0; i < this->values_.size(); i++) {
			if (values_.at(i) == this->lines_.at(i)->value()) {
				continue;
			}
			else {
				return _valueType();
			}
		}
		//Values match, delete all input connections.
		//NOTE: the "critical" part is needed because if multiple processes try to "disconect"
		//      this combination at the same time, it can cause a memory race condition. The
		//      project which compiles this needs to have openMP enabled.
		#pragma omp critical 
		this->inputs(std::unordered_set<Connecting*>());
		return _valueType();
	};

	///////////////////////////////////////////////////////////////////////////
	// GOAL FUNCTIONS
	///////////////////////////////////////////////////////////////////////////
	
	/*
	 * Has this combination been seen / Is the Goal satisfied (i.e., 
	 * successful)?
	 *
	 * @return True if the combination has been seen.
	 */
	virtual bool success() {
		return this->inputs().size() == 0;
	};

	/*
	 * Is the Goal impossible (i.e., a backtrace is needed)?
	 *
	 * @return True if the goal is not possible
	 */
	virtual bool impossible() {
		for (size_t lineIndex = 0; lineIndex < this->lines_.size(); lineIndex++) {
			_lineType* line = this->lines_.at(lineIndex);
			_valueType value = this->values_.at(lineIndex);
			if (line->value().valid() & line->value().magnitude() != value.magnitude()) {
				return true;
			}
		}
		return false;
	}

	/*
	 * Return a "frontier" of possible values to set.
	 *
	 * For a combination, this is all lines which do not have a valid value set.
	 *
	 * @return A list of location - Value pairs corresponding to choices which
	 *         may satisfy this Goal.
	 */
	virtual std::vector<std::pair<Evented<_valueType>*, _valueType>> frontier() {
		std::vector<std::pair<Evented<_valueType>*, _valueType>> toReturn;
		for (size_t lineIndex = 0; lineIndex < this->lines_.size(); lineIndex++) {
			_lineType* line = this->lines_.at(lineIndex);
			_valueType value = this->values_.at(lineIndex);
			std::pair<Evented<_valueType>*, _valueType> lineValuePair = std::pair<Evented<_valueType>*, _valueType>(line, value);
			toReturn.push_back(lineValuePair);
		}
		return toReturn;
	}
	

	/*
	 * The current input Values seen by this Combination.
	 *
	 * @return The current input Values seen by this Combination.
	 */
	//DELETE: obsolete after "success" and "impossible
	//std::vector<_valueType> currentValues() {
	//	std::vector<_valueType> toReturn;
	//	for (_lineType* line : this->lines_) {
	//		toReturn.push_back(line->value());
	//	}
	//	return toReturn;
	//};

	/*
	 * The target values of this Combination.
	 *
	 * @return The target values of this Combination.
	 */
	 //DELETE: obsolete after "success" and "impossible
	//std::vector<_valueType> targetValues() const {
	//	return this->values_;
	//};

	/*
	 * Return the Lines polled by this Combination.
	 *
	 * @return The Lines polled by this Combination.
	 */
	 //DELETE: obsolete after "success" and "impossible
	//std::vector<_lineType*> lines() const {
	//	return this->lines_;
	//};

	/*
	 * For a given node, create and return all possible input combinations.
	 *
	 * @param _node The node to generate all (not yet seen) Combinations for.
	 * @return All (not yet seen) input Combinations for this node.
	 */
	static std::unordered_set<Combination<_lineType, _nodeType, _valueType>*> allNodeCombinations(_nodeType* _node) {
		std::unordered_set<Combination<_lineType, _nodeType, _valueType>*> toReturn;
		std::vector<_valueType> inputValues(_node->inputs().size(), _valueType(0));
		if (inputValues.size() == 0) {
			return toReturn;
		}
		std::vector<_lineType*> inputLines;
		for (Connecting* input : _node->inputs()) {
			_lineType* inputCast = dynamic_cast<_lineType*>(input);
			inputLines.push_back(inputCast);
		}
		do {
			Combination<_lineType, _nodeType, _valueType>* newCombination = new Combination<_lineType, _nodeType, _valueType>(inputLines, inputValues);
			toReturn.emplace(newCombination);
		} while (ValueVectorFunction<_valueType>::increment(inputValues) == true);
		return toReturn;
	}

private:
	/*
	 * The Lines of this Combination.
	 */
	std::vector<_lineType*> lines_;

	/*
	 * The Values of this Combination.
	 */
	std::vector<_valueType> values_;

};


#endif