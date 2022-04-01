#pragma once
#include "tpi/TPI.hpp"
#include "tpi/TPI_COP.hpp"
#include"Propagate_fault_stucture.h"
#include"Block_fault.h"

#include<cmath>
template <class _primitive, class _nodeType, class _lineType>
class TPI_propagate_fault : public virtual TPI<_primitive, _nodeType, _lineType>, public TPI_COP<_primitive, _nodeType, _lineType> {
public:
	TPI_propagate_fault(
		Circuit* _circuit,
		size_t _TPLimit = std::numeric_limits<size_t>::max(),
		float _qualityLimit = 1.0,
		size_t _timeLimit = std::numeric_limits<size_t>::max()
		
	) :
		TPI<_primitive, _nodeType, _lineType>(
			_circuit,
			_TPLimit,
			_qualityLimit,
			_timeLimit)
		, TPI_COP<_primitive, _nodeType, _lineType>(
			_circuit,
			_TPLimit,
			_qualityLimit,
			_timeLimit,
			stuck_target)
	{
		this->baseFaultCoverage_ = -1;
	};


	virtual std::set<Testpoint<_primitive, _nodeType, _lineType>*> testpoints(
		std::set<Testpoint<_primitive, _nodeType, _lineType>*> control_testpoints,		
		std::set<Testpoint<_primitive, _nodeType, _lineType>*> observe_testpoints,
		std::vector<size_t> _limits = std::vector<size_t>()
	) {
		this->resetTimer();
		size_t curNumberTestpoints = 0;
		std::vector<size_t> numInserted = std::vector<size_t>(_testpoints.size(), 0);
		float curQuality = -1;
		std::set<Testpoint<_primitive, _nodeType, _lineType>*> inserted;

		if (_limits.size() == 0) {
			_limits = std::vector<size_t>(_testpoints.size(), std::numeric_limits<size_t>::max());
		}
		if (_limits.size() != _testpoints.size()) {
			throw "Testpoint insertion limits to not match the number of testpoint sets.";
		}


		while (
			this->timeUp() == false &&
			curNumberTestpoints < this->TPLimit_ &&
			curQuality < this->qualityLimit_
			) {
			float bestQuality = -1;
			Testpoint<_primitive, _nodeType, _lineType>* bestcontrolTestpoint;			
			Testpoint<_primitive, _nodeType, _lineType>* bestobserveTestpoint;
			Testpoint<_primitive, _nodeType, _lineType>* bestTestpoint;
			std::vector<std::set<Testpoint<_primitive, _nodeType, _lineType>*>> _testpoints
			size_t bestSet = 0;
			
			

				bestcontrolTestpoint = First_best_TP(bestcontrolTestpoint)
					
					bestobserveTestpoint = First_best_TP(bestobserveTestpoint)

					_testpoints.push_back(bestcontrolTestpoint);
				
				_testpoints.push_back(bestcontrol0Testpoint);
				for (int i = 0; i < _testpoints.size(); i++)
				{
					std::pair<Testpoint<_primitive, _nodeType, _lineType>*, float> best = this->bestTestpoint(_testpoints.at(i));
					if (best.second > bestQuality) {
						bestQuality = best.second;
						bestTestpoint = best.first;
						bestSet = i;
					}
				}
			if (bestQuality < 0) {//No more TPs to add: no positive quality.
				break;
			}
			inserted.emplace(bestTestpoint);
			_testpoints.erase(bestTestpoint);
			bestTestpoint->activate();
			printf("TP Chosen - %d of %d\n", curNumberTestpoints, this->TPLimit_);
			curNumberTestpoints = curNumberTestpoints + 1;

		}
		for (Testpoint<_primitive, _nodeType, _lineType>* testpoint : inserted) {
			testpoint->deactivate();
		}

		return inserted;
	};



	/*
	*return best TP through calculating propoagte fault number(observe TPS) and blocked fault number(control TPs)

	*@return best TP 

	*/

	Testpoint<_primitive, _nodeType, _lineType>* First_best_TP(std::set<Testpoint<_primitive, _nodeType, _lineType>*> testpoints) {
		float bestquality=-1;
		float cur = 0;
		float probability;
		float bf,pf;
		Testpoint<_primitive, _nodeType, _lineType>* besttp;
		for (size_t i = 0; i < testpoints.size(); i++) {
			if (if (dynamic_cast<Testpoint_control<bool, COPFaultyNode<bool>, COPFaultyLine<bool>>*>(testpoints.at(i)) != nullptr))// is a control tp
			{
				if (testpoints.at(i)->value().magnitude() == true)// tp control 1
				{
					bf = testpoints.at(i)->location()->Block_faults(true).size();
					probability = testpoints.at(i)->controllability();

				}
				else
				{
					bf = testpoints.at(i)->location()->Block_faults(false).size();

					probability = 1 - probability;
				}
				cur = -bf * log(probability);
			}

			else {
				probability = testpoints.at(i)->observability();
			}
			pf = testpoints.at(i)->location()->propagate_faults();
			cur = -pf * log(probability);


			if (cur > bestquality)
			{
				bestquality = cur;
				besttp = testpoints.at(i)
			}
		}
		return besttp

	}

private:
	float baseFaultCoverage_;

	COP_fault_calculator<_primitive> FaultCoverageCalculator_;

};