/**
 * @file TPI.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-02-19
 *
 * @copyright Copyright (c) 2019
 *
 */


#ifndef TPI_h
#define TPI_h

#include <utility> //pair
#include <limits>
#include <ctime>

#include "Testpoint.hpp"
#include "Circuit.h"

 /*
  * An algroithm which implements iterative test point insertion (TPI).
  *
  * This is a pure-virtual class describing the features of any TPI algorithm.
  *
  * @param _nodeType The type of nodes used in the circuit to be modified.
  * @param _lineType THe type of lines used in the circuit to be modified.
  * @param _valueType The type of value to be simulated. Example: Value/FaultyValue
  */
template <class _nodeType, class _lineType, class _valueType>
class TPI {
public:
	/*
	 * Create a TPI algorithm with a given TP, quality, and time limit (in
	 * seconds).
	 *
	 * @param _circuit The circuit to insert TPs into.
	 * @param (optional) _TPLimit The maximumum allowed TPs to insert. By
	 *        default, this will be the maximum possible.
	 * @param (optional) _qualityLimit The maximum quality to achieve.
	 *        By default, this is will 100% (1.0f).
	 * @param (optional) _timeLimit The maximum time (in seconds) to spend
	 *        on TPI. By default, this will be the "maximum possible time".
	 */
	TPI(
		Circuit* _circuit,
		size_t _TPLimit = std::numeric_limits<size_t>::max(),
		float _qualityLimit = 1.0,
		size_t _timeLimit = std::numeric_limits<size_t>::max()
	) {
		this->circuit_ = _circuit;
		this->TPLimit_ = _TPLimit;
		this->qualityLimit_ = _qualityLimit;
		this->timeLimit_ = _timeLimit;
	}


	/*
	 * Evaluate the given set(s) of testpoints and return those selected to be
	 * inserted.
	 *
	 * After selection, the testpoints will NOT be active.
	 *
	 * Provding multiple sets of testpoints allows limits to be set on each
	 * set. This is given as a "limit-set" pair.
	 *
	 * An exception will be thrown if the testpoint limits do not match the
	 * number of testpoint sets.
	 *
	 * @param _testpoints The testpoints to evaluate. This is given as a "set
	 *        of sets", where each set as a limit assigned to it. This limit
	 *        designates the maximum number of testpoints to choose from the
	 *        attached set.
	 * @param (optional) _limits The limit to the number of testpoints for each
	 *        set. If no limits is given, no limit will be given to any set.
	 */
	std::set<Testpoint<_nodeType, _lineType, _valueType>*> testpoints(
		std::vector<std::set<Testpoint<_nodeType, _lineType, _valueType>*>> _testpoints,
		std::vector<size_t> _limits = std::vector<size_t>()
	) {
		this->resetTimer();
		size_t curNumberTestpoints = 0;
		std::vector<size_t> numInserted = std::vector<size_t>(_testpoints.size(), 0);
		float curQuality = -1;
		std::set<Testpoint<_nodeType, _lineType, _valueType>*> inserted;

		if (_limits.size() == 0) {
			_limits = std::vector<size_t>(_testpoints.size(), std::numeric_limits<size_t>::max());
		}
		if (_limits.size() != _testpoints.size()) {
			throw "Testpoint insertion limits to not match the number of testpoint sets.";
		}


		while (
			this->timeUp() == false &&
			curNumberTestpoints < this->TPLimit_ &&
			curQuality < this->qualityLimit_&&
			_testpoints.at(0).size()>0

			) {
			float bestQuality = -10000;
			Testpoint<_nodeType, _lineType, _valueType>* bestTestpoint;
			size_t bestSet = 0;
			for (size_t i = 0; i < _testpoints.size(); i++) {
				if (numInserted.at(i) >= _limits.at(i)) { //Skip: set limit reached.
					continue;
				}
				std::pair<Testpoint<_nodeType, _lineType, _valueType>*, float> best = this->bestTestpoint(_testpoints.at(i), this->circuit_);
				if (best.second > bestQuality) {
					bestQuality = best.second;
					bestTestpoint = best.first;
					bestSet = i;
				}
			}
			//if (bestQuality < 0) {//No more TPs to add: no positive quality.
			//	break;
			//}
			inserted.emplace(bestTestpoint);
			_testpoints.at(bestSet).erase(bestTestpoint);

			////modify remove the same location TP
			/*for (size_t a = 0; a < _testpoints.size(); a++) {
				for (Testpoint<_nodeType, _lineType, _valueType>* tp : _testpoints.at(a))
				{
					if (strcmp(bestTestpoint->location()->name().c_str(),tp->location()->name().c_str())==0)
					{
						Testpoint<_nodeType, _lineType, _valueType>* toremovetp = tp;
						_testpoints.at(a).erase(toremovetp);
					}
				}
			}*/
			bestTestpoint->activate(this->circuit_);
			 //printf("TP Chosen - %d of %d\n", curNumberTestpoints, this->TPLimit_);
			curNumberTestpoints = curNumberTestpoints + 1;

		}
		/*for (Testpoint<_nodeType, _lineType, _valueType>* testpoint : inserted) {
			testpoint->deactivate(this->circuit_);
		}*/

		return inserted;
	};

	/*
	 * Set the time limit (and reset the timer).
	 *
	 * @param The new time limit (in seconds).
	 */
	void timeLimit(size_t _timeLimit) {
		this->timeLimit_ = _timeLimit;
		this->resetTimer();
	};

	/*
	 * Set the testpoint limit.
	 *
	 * @param The new testpoint limit.
	 */
	void testpointLimit(size_t _testpointLimit) {
		this->TPLimit_ = _testpointLimit;
	};

	/*
	 * Return the "quality" of this testpoint.
	 *
	 * The relative measure of this testpoing is irrelevant as long as
	 * "no quality" (i.e., this testpoint should never be chosen) is a negative
	 * number.
	 *
	 * @param _testpoint The testpoint to measure.
	 * @param _circuit The circuit the TP is added to.
	 * @return The quality of the testpoint.
	 */
	virtual float quality(Testpoint<_nodeType, _lineType, _valueType>* _testpoint, Circuit* _circuit) = 0;

	/*
	 * The circuit this TPI is attached to
	 */
	Circuit* circuit_;

private:
	/*
	 * Return the "base" quality (no extra TP fault coverage) to compare other
	 * testpoints against.
	 *
	 * This function will be used to "bestTestpoint" to return final quality.
	 *
	 * If no base is needed, have it return "0" always.
	 *
	 * @return The base quality"
	 */
	virtual float base() = 0;

	/*
	 * Reset the "base" quality to compare against other testpoints against.
	 *
	 * The  input parameter manually sets the value.
	 *
	 * @param _base The manually set base value.
	 * @return The new base quality.
	 */
	virtual float base(float _base) = 0;

	/*
	 * For a given set of testpoints, return the best testpoint and its quality.
	 *
	 * @param _testpoints The set of testpoints to evaluate.
	 * @param _circuit The Circuit to insert testpoints into.
	 * @return A pair: the best testpoint and its quality.
	 */
	std::pair<Testpoint<_nodeType, _lineType, _valueType>*, float> bestTestpoint(std::set<Testpoint<_nodeType, _lineType, _valueType>*> _testpoints, Circuit* _circuit) {
		float bestQuality = -10000;
		Testpoint<_nodeType, _lineType, _valueType>* bestTestpoint;
		size_t curTPNumber = 1;
		for (Testpoint<_nodeType, _lineType, _valueType>* curTestpoint : _testpoints) {
			if (curTestpoint->location() == nullptr)
			{
				throw"error";
		}
			


			float quality = this->quality(curTestpoint, _circuit);
			if (quality > bestQuality) {
				bestQuality = quality;
				bestTestpoint = curTestpoint;
			}
			//printf("Done analyzing TP %d of %d\n", curTPNumber, _testpoints.size());//DEBUG
			curTPNumber++;
		}
		this->base(bestQuality);
		return std::pair<Testpoint<_nodeType, _lineType, _valueType>*, float>(bestTestpoint, bestQuality);
	};

	/*
	 * Reset the timer.
	 */
	void resetTimer() {
		this->endTime_ = std::clock() + (this->timeLimit_ * CLOCKS_PER_SEC);
	};

	/*
	 * Is time up?
	 *
	 * @return True if time is up.
	 */
	bool timeUp() {
		return clock() > this->endTime_;
	};


	/*
	 * The set TP limit
	 */
	size_t TPLimit_;

	/*
	 * The set quality limit
	 */
	float qualityLimit_;

	/*
	 * The time limit (in seconds).
	 */
	size_t timeLimit_;

	/*
	 * The end of the timer (in "tick time")
	 */
	std::clock_t endTime_;

	/*std::string tpName(Testpoint<_primitive, _nodeType, _lineType>* _tp) {
		std::string type;
		if (dynamic_cast<Testpoint_observe<_primitive, _nodeType, _lineType>*>(_tp) != nullptr)
		{
			type = "OB";
		}
		else if (dynamic_cast<Testpoint_invert<_primitive, _nodeType, _lineType>*>(_tp) != nullptr)
		{
			type = "XOR";
		}
		else if (dynamic_cast<Testpoint_control<_primitive, _nodeType, _lineType>*>(_tp) != nullptr)
		{
			Testpoint_control<_primitive, _nodeType, _lineType>* cast = dynamic_cast<Testpoint_control<_primitive, _nodeType, _lineType>*>(_tp);
			type = cast->getTPValue().getValue() == true ? "C1" : "C0";
		}
		else {
			::err("I don't know what kind of TP this is.");
		}
		std::string net = _tp->getNet()->getName();
		std::string fo = _tp->getFo() == nullptr ? "NULL" : _tp->getFo()->getOutput()->getName();
		return net + "->" + fo + "_" + type;
	}*/

};

#endif