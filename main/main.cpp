#include "Parser.hpp"
#include "FaultGenerator.hpp"
#include "TPGenerator.hpp"
#include "FaultSimulator.hpp"
#include "TestPoint.hpp"
#include "Testpoint_control.hpp"
#include "Testpoint_observe.hpp"
#include "Testpoint_invert.hpp"
#include "TPI.hpp"
#include "TPI_COP.hpp"
#include "Expand.hpp"
#include "Value.hpp"
#include "SimulationStructures.hpp"
#include "ValueVectorFunctions.hpp"
#include "COP_TPI_Structures.hpp"
#include "PRPG.hpp"
#include "tpi_power.hpp"

#include "Window.hpp"
#include <vector>
#include <set>
#include <iostream>
#include <ctime>


//General settings.
#define PRIMITIVE unsigned long long int   //The underlying data primitive used for simulation.
#define SIZEOFPRIMITIVE 64   //I.e., 64 for unsigned long long int, 1 for bool

//TPI Settings
#define TPITIMELIMIT 108000   //The time limit for TPI, in seconds, i.e., 3600 -> 1 hour
#define TPLIMIT 0.01   //The TP limit in terms of a percentage of nodes,s, i.e., 0.01 -> 1%
#define PRETPIVEC 10   //Number of vectors to apply before TPI (to eleminate easy-to-detect faults from the TPI algorithm).

//For fault simulation

#define MAXVEC 16384   //The maximum total number of vectors to simulate across ALL iterations. I.e., 64*128=16384
//-> NOTE: the total number of vectors to simulate per benchmark will always be less than this number.
//-> NOTE: the simulation primitive size ("SIZEOFPRIMITIVE") is accounted for, so do not adjust this number based on the primitive size.
//--> E.g., if "unsigned long long int" is used (and thus "SIZEOFPRIMITIVE" is 64), and "MAXVEC" is 128, two 64-bit vectors will be applied automatically.

#define MAXITER 128 //The maximum number of fault simulations to perform (the average fault coverage is taken).
//-> E.g., if "1" is given, a single fault simulation will be performed.
//-> E.g., if "2" is given, two fault simulations will be performed and the average of the two fault coverages will be printed.

#define MAXFAULTCOVERAGE 99.0 //If no fault coverage limit is given, fault simulation will be performed until this fault coverage is obtained by at least one circuit.
//-> I.e., 99.0 = 99% 
//-> NOTE: in practice, this will be overwritten to 100% (no limit) or a specific limit.

#define SIMTIMELIMIT 1000 //The time, in seconds, to limit fault simulation to.
//-> I.e., 1000 -> one thousand seconds
//NOTE: this is more powerful than setting other limits (i.e., "MAXVEC" and "MAXITER"). This allows you to gather as much data as you can, time allowed.

#define TPRATIO 2 //If testpoints are given for fault simulation, what fraction of vectors will TPs be enabled for.
//-> I.e., 2 -> 1/2 of all vectors.


//Convenience: this are for easier code later on.
#define VALUETYPE FaultyValue<PRIMITIVE>
#define LINETYPE COP_TPI_Line<VALUETYPE>
#define NODETYPE COP_TPI_Node<VALUETYPE>
#define OBSERVETP Testpoint_observe<NODETYPE, LINETYPE, VALUETYPE>
#define CONTROLTP Testpoint_control<NODETYPE, LINETYPE, VALUETYPE>
#define INVERSIONTP Testpoint_invert<NODETYPE, LINETYPE, VALUETYPE>
#define GENERIC_TESTPOINT Testpoint<NODETYPE, LINETYPE, VALUETYPE>
#define FAULTTYPE Fault<VALUETYPE>

//Input benchmarks
std::vector<std::string> Circuitfiles = {
		"c17.bench",
		"c432.bench",
		"c499.bench",
		"c880.bench",
		"c1355.bench",
		"c1908.bench",
		"c2670.bench",
		"c3540.bench",
		"c5315.p.bench",
		"c6288.p.bench",
		"c7552.p.bench",
		"b01.p.bench",
		"b02.p.bench",
		"b03.p.bench",
		"b04.p.bench",
		"b05.p.bench",
		"b06.p.bench",
		"b07.p.bench",
		"b08.p.bench",
		"b09.p.bench",
		"b10.p.bench",
		"b11.p.bench",
		"b12.p.bench",
		"b13.p.bench",
		"b14.p.bench",
		"b15.p.bench" //If this one is skipped, the trailing comma from the last benchmark needs to be removed.
};

template <class _container>
void garbage(_container _trashCan) {
	for (auto item : _trashCan) {
		delete item;
	}
}


// compare two output name
template <class type>
bool connectingSortFunction(type* i, type* j) {
	Connecting* iOutput = *(i->outputs().begin());
	std::string iName = iOutput->name();
	Connecting* jOutput = *(j->outputs().begin());
	std::string jName = jOutput->name();
	return iName.compare(jName) < 0;
}


//order Pi
template <class type>
std::vector<type*> orderedPis(Circuit* _circuit) {
	std::vector<type*> toReturn;
	for (Levelized* pi : _circuit->pis()) {
		type* cast = dynamic_cast<type*>(pi);
		if (cast == nullptr) {
			throw "Cannot order PIs if a pi is not of the correct type";
		}
		toReturn.push_back(cast);
	}
	std::sort(toReturn.begin(), toReturn.end(), connectingSortFunction<type>);
	return toReturn;
}


std::set<GENERIC_TESTPOINT*> chooseTPs(Circuit* _circuit, size_t _pre_sim,  bool _stuck_at) {

	//FIRST, set limits after simulating easy-to-detect faults.
	FaultSimulator<VALUETYPE> fs(!_stuck_at); 
	fs.setFaults(FaultGenerator<VALUETYPE>::allFaults(_circuit, _stuck_at));
	
	for (size_t i = 0; i < _pre_sim; i++) {
		std::vector<VALUETYPE> inputVec = ValueVectorFunction<VALUETYPE>::random(_circuit->pis().size());
		fs.applyStimulus(_circuit, inputVec);
	}

	
	//SECOND, set TPI constraints
	TPI_COP<NODETYPE, LINETYPE, VALUETYPE>tpi(_circuit, MAXVEC,{}, {}, {}, _stuck_at, fs.undetectedFaults());
	tpi.timeLimit(TPITIMELIMIT);
	tpi.testpointLimit(_circuit->nodes().size()*TPLIMIT);

	
	// THIRD, generate all TPs
	TPGenerator<CONTROLTP, NODETYPE, LINETYPE, VALUETYPE> TPgenerator_control;
	std::set<GENERIC_TESTPOINT*> all_control_tps = TPgenerator_control.allTPs(_circuit);
	TPGenerator<OBSERVETP, NODETYPE, LINETYPE, VALUETYPE> TPgenerator_observe;
	std::set<GENERIC_TESTPOINT*> all_observe_tps = TPgenerator_observe.allTPs(_circuit);
	std::vector<std::set<GENERIC_TESTPOINT*>> all_tps;
	all_tps.push_back(all_control_tps);
	all_tps.push_back(all_observe_tps);
	

	//FOURTH, select TPs (the long part)
	clock_t start2, finish2;
	start2 = clock();
	
	std::set<GENERIC_TESTPOINT*> chosen_tps = tpi.testpoints(all_tps);
	
	finish2 = clock() - start2;
	double totaltime2 = finish2 / (float)CLOCKS_PER_SEC;
	printf("%f\t", totaltime2);


	//FOURTH, clean up (create a copy of chosen TPs and delete the originals)
	for (std::set<GENERIC_TESTPOINT*> tp_set : all_tps) {
		for (GENERIC_TESTPOINT* tp : tp_set) {
			if (chosen_tps.count(tp) == 0) {
				delete tp;
			}
		}
	}

	int numC = 0;
	int numO = 0;
	int numI = 0;

	for (GENERIC_TESTPOINT* tp : chosen_tps) {
		if (dynamic_cast<CONTROLTP*>(tp) != nullptr) {
			numC++;
		}
		else if (dynamic_cast<OBSERVETP*>(tp) != nullptr) {
			numO++;
		}
		else if (dynamic_cast<INVERSIONTP*>(tp) != nullptr) {
			numI++;
		}

		else {
			throw "Something is wrong here.";
		}
	}
	printf("%d\t", numC);

	//CLEAN UP (the faults in TPI and fault simulator)
	//Since the faults in TPI are also in the fault simulator, clean the fault simulator only.
	tpi.clearFaults();
	/*std::unordered_set<Fault<VALUETYPE>*> toDelete = fs.clearFaults();
	for (Fault<VALUETYPE>* fault : toDelete) {
		delete fault;
	}
	*/

	return chosen_tps;
}

#define MAXVECPERITER 10000000000  //The maximnum number of vectors allowed for any single fault simulation iteration.
/*-> Do not overwrite this value: it is used only if no limit is given, and therefore should be a very large value.
Simulate until...
1) The maximum number of vectors is reached.
2) The time limit is reached.
3) One benchmark reaches the max fault coverage limit.
3a) If this is reached, another iteration will be strated. If the other two limits reached in the middle of a non-first iteration, the results of the last iteration will be discarded.
@return The number of vectors simulated per iteration.
*/

size_t faultSimulate(
	
	std::vector<Circuit*> _circuits,
	std::vector<std::unordered_set<FAULTTYPE*>> _faults,
	
	float _FCLimit = MAXFAULTCOVERAGE,
	bool _tdf = false,
	size_t _vecLimit = MAXVECPERITER,
	size_t _itrLimit = MAXITER,
	
	std::vector<std::set<GENERIC_TESTPOINT*>> _testpoints = std::vector< std::set<GENERIC_TESTPOINT*>>()
) {
	PRPG<VALUETYPE> prpg(_circuits.at(0)->pis().size());

	//PREPARE fault simulators and ordered PIs for each circuit.
	
	std::vector<FaultSimulator<VALUETYPE>*> faultSimulators;
	
	std::vector<std::vector<SimulationNode<VALUETYPE>*>> pis;
	
	for (size_t i = 0; i < _circuits.size(); i++) {
		Circuit* circuit = _circuits.at(i);
		FaultSimulator<VALUETYPE> * faultsimulator = new FaultSimulator<VALUETYPE>(_tdf); faultsimulator->setFaults(_faults.at(i));
		faultSimulators.push_back(faultsimulator);
		pis.push_back(orderedPis<SimulationNode<VALUETYPE>>(circuit));
	}


	//PREPARE Accumulated fault coverages (across all iterations).
	std::vector<float> faultCoverages = std::vector<float>(_circuits.size(), 0.0);

	size_t iteration_number = 0;
	size_t num_vec_applied = 0;
	size_t vec_per_iter = _vecLimit;
	clock_t start, finish;
	start = clock();


	//FAULT SIMULATION
	while (iteration_number < _itrLimit) {
		float cur_best_fault_coverage = 0.0;
		size_t iter_vec_applied = 0;
		bool tpActivated = false;
		while (
			((cur_best_fault_coverage < _FCLimit) || iteration_number != 0) && //Max fault coverage reached (which we only care if we do not have a vec_per_iter set).
			(((clock() - start) / CLOCKS_PER_SEC) < SIMTIMELIMIT) &&     //Time limit reached
			(num_vec_applied < MAXVEC) &&       //Total (across all iterations) vec limit reached.
			(iter_vec_applied < vec_per_iter)   //Vec limit (for this iteration) reached
			) {  //Apply a vector if all conditions are met 
			//debug printf("iter %d\t vec %d\n", iteration_number, num_vec_applied);
			std::vector<VALUETYPE> inputVector = prpg.load();

			//TP activation check
			//DEBUG printf("%d + %d (%d): %d %d %d\n", iter_vec_applied, vec_per_iter, TPRATIO, (_testpoints.size() != 0) , (iter_vec_applied >= vec_per_iter / TPRATIO) , (iter_vec_applied < (vec_per_iter / TPRATIO) + SIZEOFPRIMITIVE));
			if ((_testpoints.size() != 0) && (iter_vec_applied >= vec_per_iter / TPRATIO) && (iter_vec_applied < (vec_per_iter / TPRATIO) + SIZEOFPRIMITIVE)) {
				//debug printf("Activating tps...\n");
				tpActivated = true;
				Simulator<VALUETYPE> clearingSimulator; EventQueue<VALUETYPE> clearingQueue;
				for (size_t i = 0; i < _testpoints.size(); i++) {
					std::set<GENERIC_TESTPOINT*> tpset = _testpoints.at(i);

					for (GENERIC_TESTPOINT* tp : tpset) {
						//debug printf("Adding to queue...\n");
						clearingQueue.add(tp->activate(_circuits.at(i)));
					}
					//debug printf("Clearing circuit state...\n");
					clearingSimulator.applyStimulus(_circuits.at(i), std::vector<VALUETYPE>(), clearingQueue);
				}
				//debug printf("Done activating TPs\n");
			}

			for (size_t i = 0; i < _circuits.size(); i++) {
				faultSimulators.at(i)->applyStimulus(
					_circuits.at(i), inputVector, EventQueue<VALUETYPE>(), pis.at(i)
				);
				cur_best_fault_coverage =
					cur_best_fault_coverage > faultSimulators.at(i)->faultcoverage() ?
					cur_best_fault_coverage : faultSimulators.at(i)->faultcoverage();
			}

			num_vec_applied += SIZEOFPRIMITIVE;
			iter_vec_applied += SIZEOFPRIMITIVE;
		}

		vec_per_iter = (iteration_number == 0) ? iter_vec_applied : vec_per_iter;

		//Deactivate TPs
		if (tpActivated) {
			//debug printf("Activating tps...\n");
			Simulator<VALUETYPE> clearingSimulator; EventQueue<VALUETYPE> clearingQueue;
			for (size_t i = 0; i < _testpoints.size(); i++) {
				std::set<GENERIC_TESTPOINT*> tpset = _testpoints.at(i);
				for (GENERIC_TESTPOINT* tp : tpset) {
					//debug printf("Adding to queue...\n");
					clearingQueue.add(tp->deactivate(_circuits.at(i)));
				}
				//debug printf("Clearing circuit state...\n");
				clearingSimulator.applyStimulus(_circuits.at(i), std::vector<VALUETYPE>(), clearingQueue);
			}
			//debug  printf("Done activating TPs\n");
		}

		//Save iteration result if 1st iter or not half-way through.
		if (
			(iteration_number == 0) ||
			(
			(((clock() - start) / CLOCKS_PER_SEC) < SIMTIMELIMIT) &&
				(num_vec_applied < _vecLimit)
				)
			) {
			for (size_t i = 0; i < _circuits.size(); i++) {
				faultCoverages[i] = faultCoverages.at(i) + faultSimulators.at(i)->faultcoverage();
				//DEBUG printf("%d -> %f (%f):\n", i, faultCoverages.at(i) / ((float) iteration_number + 1), faultCoverages.at(i));
			}
		}
		else {
			break;
		}

		for (size_t i = 0; i < _circuits.size(); i++) {
			faultSimulators.at(i)->resetFaults();
		}

		iteration_number++;
	}
	
	//for (size_t i = 0; i < _circuits.size(); i++) {
	//	faultCoverages[i] = (faultCoverages.at(i) / (float) iteration_number); //Average the fault coverages.
	//}

	//debug printf("                                             \r");//Clear the line from previous carage return.
	printf("%d\t", vec_per_iter); //Vectors
	printf("%d\t", iteration_number); //Iterations
	//DEBUG printf("FaultCoverages:\n"); //Fault simulation...
	for (size_t i = 0; i < _circuits.size(); i++) {
		printf("%f\t", faultCoverages.at(i) / (float)iteration_number);
		//std::vector<FAULTTYPE*> faults = faultSimulators.at(i)->clearFaults();
		//NO: calling function is responsible for deleting.
		//for (FAULTTYPE* fault : faults) {
		//	delete fault;
		//}
		delete faultSimulators.at(i);
	}

	return vec_per_iter;
}


void evalCircuit(std::string _circuitFile) {
	//FIRST, parse all circuits and get generate circuit information.
	Parser<LINETYPE, NODETYPE, VALUETYPE> parser;
	Circuit* circuit_no_tpi = parser.Parse(_circuitFile.c_str());		// cop circuit
	Circuit* circuit_cop_tpi_saf = parser.Parse(_circuitFile.c_str());	// orginal circuit
	Circuit* circuit_cop_tpi_tdf = parser.Parse(_circuitFile.c_str());	// orginal circuit
	printf("%s\t", _circuitFile.c_str()); //BENCH
	printf("%d\t", circuit_no_tpi->pis().size()); //PIs
	printf("%d\t", circuit_no_tpi->pos().size()); //POs
	printf("%d\t", circuit_no_tpi->nodes().size()); //Nodes


	//Generate faults before TPI: otherwise fault coverage comparisons are unfare.
	//These faults will be deleted at the end of fault simulation.
	std::vector < std::unordered_set<FAULTTYPE*>> safs = {
		FaultGenerator<VALUETYPE>::allFaults(circuit_no_tpi,true),
		FaultGenerator<VALUETYPE>::allFaults(circuit_cop_tpi_saf,true),
		FaultGenerator<VALUETYPE>::allFaults(circuit_cop_tpi_tdf,true)
	};
	std::vector < std::unordered_set<FAULTTYPE*>> tdfs = {
		FaultGenerator<VALUETYPE>::allFaults(circuit_no_tpi,false),
		FaultGenerator<VALUETYPE>::allFaults(circuit_cop_tpi_saf,false),
		FaultGenerator<VALUETYPE>::allFaults(circuit_cop_tpi_tdf,false)
	};


	//Perform fault simulation on the original circuit to (try to) reach 95% fault coverage.
	size_t numVec = faultSimulate(std::vector<Circuit*>(
		{ circuit_no_tpi }),  //No-TPI circuit.
		std::vector<std::unordered_set<FAULTTYPE*>>({ safs.at(0) }), //The no-TPI circuit's faults.
		95.0,	//The goal fault coverage
		false,	//stuck-at fault simulation.
		MAXVEC,	//Do not limit the number of vectors
		1 		//Limit to a single iteration.
	);

	//Perform TPI (TPs are actiated during fault simulation.
	std::set<GENERIC_TESTPOINT*> cop_tpi_saf_tps = chooseTPs(circuit_cop_tpi_saf, PRETPIVEC, false);
	for (GENERIC_TESTPOINT* tp : cop_tpi_saf_tps) {
		tp->deactivate(circuit_cop_tpi_saf);
	}
	std::set<GENERIC_TESTPOINT*> cop_tpi_tdf_tps = chooseTPs(circuit_cop_tpi_tdf, PRETPIVEC, true);
	for (GENERIC_TESTPOINT* tp : cop_tpi_tdf_tps) {
		tp->deactivate(circuit_cop_tpi_tdf);
	}

	std::vector<Circuit*> allCircuits = { circuit_no_tpi, circuit_cop_tpi_saf, circuit_cop_tpi_tdf };
	std::vector<std::set<GENERIC_TESTPOINT*>> allTestpoints = {
		std::set<GENERIC_TESTPOINT*>(),
		cop_tpi_saf_tps, cop_tpi_tdf_tps };
	faultSimulate(allCircuits, safs, 100.0, false, numVec, MAXITER, allTestpoints); //Do all safs, use no FC limit, set a vector limit based on the previous 95% fault coverage, repeat iterations and use average FC, time permitting.
	faultSimulate(allCircuits, tdfs, 100.0, true, numVec, MAXITER, allTestpoints); //Do all tdfs, ...


	//CLEANUP (faults are already deleted)
	garbage<std::set< GENERIC_TESTPOINT*>>(cop_tpi_saf_tps);
	garbage<std::set< GENERIC_TESTPOINT*>>(cop_tpi_tdf_tps);
	for (auto tdfList : tdfs) {
		garbage<std::unordered_set<FAULTTYPE*>>(tdfList);
	}
	for (auto safList : safs) {
		garbage<std::unordered_set<FAULTTYPE*>>(safList);
	}
	garbage<std::vector<Circuit*>>(allCircuits);  //Circuits must be deleted after faults.
}


//expand circuit, for every gate, no more than 2 input, no more than 2 fanout
Circuit * expand(Circuit *_c)
{
	Expand <VALUETYPE,NODETYPE, LINETYPE >expand;
	expand.Expand_in(_c);
	expand.Expand_out(_c);
	return _c;
}


//generte a sub-circuit based on seleted line, and subcircut size is _limit, _c must be expanded
Circuit * Subcircuit(Circuit* _c, Levelized* _line, size_t _limit) {
	Window<VALUETYPE, NODETYPE, LINETYPE> window;
	Circuit* subcircuit = window.Getwindow(_c, _line, _limit);
	return subcircuit;
}


//generage input feature, _c is entire circuit not subcircuit
std::vector< float>Features(Circuit* _c, Levelized* _line, int _limit) {
	Window<VALUETYPE, NODETYPE, LINETYPE> window;
	std::vector< float> features = window.getdata(_c, _line, _limit);
	return features;
}


int main(int argc, const char* argv[]) {
	printf("Printing table header...\n");
	printf("BENCH\tPIs\tPOs\tNodes\t");
	//First fault simulation: no TPs, reach 95%
	printf("95\%_Vec\t95\%_Iter\t");
	printf("95\%_FC_No_TPs\t");
	//TIME time and results (saf-targeting): control and observe pts
	printf("TPI_time_saf\tTPI_results_saf\t");
	//TIME time and results (tdf-targeting): control and observe pts
	printf("TPI_time_tdf\tTPI_results_tdf\t");
	//Second fault simulation: safs 
	printf("saf_Vec\tsaf_Iter\t");
	printf("noTPs_FC\t");
	printf("saf_targeting_FC\t");
	printf("tdf_targeting_FC\t");
	//Second fault simulation: tdf 
	printf("tdf_Vec\ttdf_Iter\t");
	printf("noTPs_FC\t");
	printf("saf_targeting_FC\t");
	printf("tdf_targeting_FC\t");
	printf("\n");
	for (std::string circuitFile : Circuitfiles) {
		evalCircuit("C://lab2//EDA2.0//Open EDA 2.0//"+circuitFile);
		printf("\n");
	}

	


	
}