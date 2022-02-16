#pragma once
#include <utility> 
#include <limits>
#include <ctime>
#include "TPI.hpp"
#include "TPI_COP.hpp"
#include"Propagate_fault_stucture.h"
#include"Block_fault.h"

#include<cmath>
template <class _primitive, class _nodeType, class _lineType>
class TPI_propagate_fault :  public   TPI_COP<_primitive, _nodeType, _lineType> {
public:
	TPI_propagate_fault(
		Circuit* _circuit,
		size_t _TPLimit = std::numeric_limits<size_t>::max(),
		float _qualityLimit = 1.0,
		size_t _timeLimit = std::numeric_limits<size_t>::max(),
		bool stuck_target = true
		
		
	) :
		
		 TPI_COP<_primitive, _nodeType, _lineType>(
			_circuit,
			_TPLimit,
			_qualityLimit,
			_timeLimit,
			 stuck_target)
		, FaultCoverageCalculator_(
			_circuit, stuck_target
		)
		
		
	{
		
	};


	std::set<Testpoint<_primitive, _nodeType, _lineType>*> TPI_PRO(
		std::set<Testpoint<_primitive, _nodeType, _lineType>*> control_testpoints,
		std::set<Testpoint<_primitive, _nodeType, _lineType>*> observe_testpoints,
		std::vector<size_t> _limits = std::vector<size_t>()
	)
	{
		this->resetTimer();
		size_t curNumberTestpoints = 0;

		float curQuality = -1;
		std::set<Testpoint<_primitive, _nodeType, _lineType>*> inserted;



		while (
			this->timeUp() == false &&
			curNumberTestpoints < this->TPLimit_ &&
			curQuality < this->qualityLimit_
			) {
			float bestQuality = -1;
			Testpoint<_primitive, _nodeType, _lineType>* bestcontrolTestpoint;
			Testpoint<_primitive, _nodeType, _lineType>* bestobserveTestpoint;
			Testpoint<_primitive, _nodeType, _lineType>* bestTestpoint;
			




				bestcontrolTestpoint = First_best_TP(control_testpoints);

			bestobserveTestpoint = First_best_TP(observe_testpoints);

			
				
			float quality_control = this->quality(bestcontrolTestpoint);
			float quality_observe = this->quality(bestobserveTestpoint);
			if (quality_control >= quality_observe)
			{
				bestTestpoint = bestcontrolTestpoint;
				bestTestpoint = bestobserveTestpoint;
			}
			else {
				bestTestpoint = bestobserveTestpoint;
			}
			
			inserted.emplace(bestTestpoint);
			if (dynamic_cast<Testpoint_control<_primitive, _nodeType, _lineType>*>(bestTestpoint) != nullptr)//control tp
			{
				control_testpoints.erase(bestTestpoint);
			}
			else {
				observe_testpoints.erase(bestTestpoint);
			}
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
		float bestquality = -1;
		float cur = 0;
		float probability;
		float bf, pf;
		Testpoint<_primitive, _nodeType, _lineType>* besttp=nullptr;
		for (Testpoint<_primitive, _nodeType, _lineType>* curTestpoint : testpoints) {
			if (dynamic_cast<Testpoint_control<_primitive, _nodeType, _lineType>*>(curTestpoint) != nullptr)// is a control tp
			{
				probability = curTestpoint->location()->controllability();
				if (curTestpoint->value().magnitude() == true)// tp control 1
				{
					bf = curTestpoint->location()->Block_faults(true).size();
					//printf("location: %s  bf:%f \n", curTestpoint->location()->name().c_str(), bf);
					

				}
				else
				{
					std::vector<Fault<bool>*>aa = curTestpoint->location()->Block_faults(false);
					bf = aa.size();
					//printf("location: %s  bf: %f \n", curTestpoint->location()->name().c_str(), bf);
					probability = 1 - probability;
					//printf("location: %s  bf: %d   pro:%f \n ", curTestpoint->location()->name().c_str(), aa.size(), probability);
					float dur = -bf * log(probability);
					//printf("location: %s ", curTestpoint->location()->name().c_str());
					//printf("bf: %f ", bf);
					//printf("dur :%f", dur);
					//printf("pro:%f \n", probability);
					//printf("location: %s  bf: %d   pro:%f  cur %f\n", curTestpoint->location()->name().c_str(), bf, probability, dur);
				}
				cur = -bf * log(probability);
				
			}

			else {
				probability = curTestpoint->location()->observability();

				pf = curTestpoint->location()->propagate_faults().size();
				
				cur = -pf * log(probability);
			/*	printf("location: %s ", curTestpoint->location()->name().c_str());
					printf("pf: %f ", pf);
					printf("dur :%f ", cur);
					printf("pro:%f \n", probability);*/
					
			}

			if (cur > bestquality)
			{
				bestquality = cur;
				besttp = curTestpoint;
			}
		}
		return besttp;

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

private:
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

	float baseFaultCoverage_;

	COP_fault_calculator<_primitive> FaultCoverageCalculator_;
	

};