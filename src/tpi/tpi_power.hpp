#pragma once
/**
 * @file TPI_COP.h
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-02-21
 *
 * @copyright Copyright (c) 2019
 *
 */


#ifndef TPI_POWER_h
#define TPI_POWER_h
#include<cmath>
#include "tpi/TPI.hpp"
#include "tpi/COP_fault_calculator.hpp"
#include "trace/tracer.hpp"
#include "cop/COPStructures.hpp"

 /*
  * COP-based TPI.
  *
  * The TPI implemented by this TPI method is very similar to the method in the
  * following article. The given article does some implied optimization after
  * a TP is inserted by only calculating the change in fault coverage for
  * "effected" faults, but otherwise, the resulting TPs selected should be the
  * same.
  *
  * H.-C. Tsai, K.-T. Cheng, C.-J. Lin and S. Bhawmik. A hybrid algorithm for
  * test point selection for scan-based BIST. Proc. of 34th Design Automation
  * Conference, pages 478-483,1997.
  *
  * NOTE: There is no "_primitive" parameter since COP can only be performed on
  *       Boolean circuits.
  * @param _nodeType The type of nodes used in the circuit to be modified.
  * @param _lineType THe type of lines used in the circuit to be modified.
  */

template < class _nodeType, class _lineType, class _valueType >
class TPI_POWER : public TPI< _nodeType, _lineType,_valueType > {
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
	 * @param (optinal) stuck_target If true, COP calculates the prob. SAFs
	 *        are detected, else, TDFs.
	 * @param (optional) _faults Specific faults to garget. Else, target
	 *        all faults.
	 */
	TPI_POWER(
		Circuit* _circuit,
		size_t _TPLimit = std::numeric_limits<size_t>::max(),
		float _qualityLimit = 1.0,
		size_t _timeLimit = std::numeric_limits<size_t>::max(),
		bool stuck_target = true
	
		
	) :
		TPI< _nodeType, _lineType, _valueType>(
			_circuit,
			_TPLimit,
			_qualityLimit,
			_timeLimit)
		
	{
		basepower = this->power_calculator( _circuit);
	};

	/*
	 * Return the "quality" of this testpoint.
	 *
	 * For this TPI method, the "quality" is the change in fault coverage.
	 *
	 * @param _testpoint The testpoint to measure.
	 * @param _circuit The circuit the TP is added to.
	 * @return The quality of the testpoint.
	 */
	float quality(Testpoint< _nodeType, _lineType, _valueType>* _testpoint, Circuit* _circuit) {
		//printf("\n%s\t", _testpoint->location()->name().c_str());
	
		//printf("%0.8f", base);
		//COP_fault_calculator<_valueType>FaultCoverageCalculator1(_circuit,true, _faults);
		//float baseFaultCoverage_ = this->FaultCoverageCalculator_.faultCoverage();
	
		//printf("%0.7f\t", base);
			_testpoint->activate(_circuit);
		
			float TPpower = this->power_calculator( _circuit);
			//printf("%0.7f\t",TPpower);
			//float FaultCoverage = this->FaultCoverageCalculator_.faultCoverage();
			//float toReturn = (TPpower / base)*10+ (FaultCoverage/ baseFaultCoverage_);
			float toReturn = TPpower - basepower;
			_testpoint->deactivate(_circuit);
			return toReturn;
			
		
	};

	

private:

	/*
	 * Return the "quality" of this testpoint.
	 *
	 * For this TPI method, the "quality" is the change in fault coverage.
	 *
	 * @param _testpoint The testpoint to measure.
	 * @param _circuit The circuit the TP is added to.
	 * @return The quality of the testpoint.
	 */
	float power_calculator( Circuit* _circuit) {

		//std::vector<Tracable*> forwards_set;
		
		
		float TPpower=0.0;
		//forwards_set = Tracer<_lineType, _nodeType>::forwards_all(dynamic_cast<Tracable*>(_testpoint->location()));	
		//printf("name %s forward: %d \n", _testpoint->location()->name().c_str(), forwards_set.size());
		std::unordered_set<_lineType*>all_lines;
		_lineType* curline;
		for (Connecting* node : _circuit->nodes())
		{
			if (node->outputs().size() != 0) {
				for (Connecting* line : node->outputs())
				{
					curline = dynamic_cast<_lineType*>(line);
					if (curline != nullptr)
					{
						all_lines.emplace(curline);

					}
				}
			}

			if (node->inputs().size() != 0) {
				for (Connecting* line : node->inputs())
				{
					curline = dynamic_cast<_lineType*>(line);
					if (curline != nullptr)
					{
						all_lines.emplace(curline);
						
					}
				}
			}
		}
		//printf(" %d\t", all_lines.size());
		
		for (_lineType* line: all_lines)

		{
			//printf("\n line name :%s",line->name().c_str());
			COPLine<_valueType>* cast = dynamic_cast<COPLine<_valueType>*>(line);
			if (cast != nullptr)
			{
				TPpower = TPpower + abs(cast->controllability() - 0.5);
				//printf("\n%s\t  %0.8f\n", cast->name().c_str(), abs(cast->controllability() - 0.5));
			}
		}
		
		
		return TPpower;
		
	};


	/*
	 * The "base fault coverage". If not yet calculated, it will be negative.
	 */
	float basepower;

	



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
	virtual float base() {
		return 0;
	};

	/*
	 * Reset the "base" quality to compare against other testpoints against.
	 *
	 * The  input parameter manually sets the value.
	 *
	 * @param _base The manually set base value.
	 * @return The new base quality.
	 */
	virtual float base(float _base) {
		return _base;
	};



};

#endif