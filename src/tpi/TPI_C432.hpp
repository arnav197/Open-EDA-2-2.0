
#pragma once
/**
 * @file Testpoint_Test.cpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-06-23
 *
 * @copyright Copyright (c) 2019
 *
 */

#include"gtest/gtest.h"
#include"TPI_COP.hpp"

#include "Parser.hpp"
#include "COP_TPI_Structures.hpp"
#include "TPGenerator.hpp"
#include "TPI_COP.hpp"
#include "Testpoint_control.hpp"
#include "Testpoint_invert.hpp"
#include "Testpoint_observe.hpp"
#include "FaultSimulator.hpp"
#include "PRPG.hpp"
#include "FaultyValue.hpp"

#define VALUE FaultyValue<bool>
#define NODETYPE COP_TPI_Node<VALUE>
#define LINETYPE COP_TPI_Line<VALUE>
#define TPCONTROL Testpoint_control<NODETYPE, LINETYPE, VALUE>
#define TPOBSERVE Testpoint_observe<NODETYPE, LINETYPE, VALUE>
#define TPINVERT Testpoint_invert<NODETYPE, LINETYPE, VALUE>
#define TP Testpoint<NODETYPE, LINETYPE, VALUE>

bool connectingSortFunction (SimulationNode<VALUE>* i, SimulationNode<VALUE>* j) {
	Connecting* iOutput = *(i->outputs().begin());
	std::string iName = iOutput->name();
	Connecting* jOutput = *(j->outputs().begin());
	std::string jName = jOutput->name();
	return iName.compare(jName) < 0;
}

std::vector<SimulationNode<VALUE>*> orderedPis(Circuit* _circuit) {
	std::vector<SimulationNode<VALUE>*> toReturn;
	for (Levelized* pi : _circuit->pis()) {
		SimulationNode<VALUE>* cast = dynamic_cast<SimulationNode<VALUE>*>(pi);
		if (cast == nullptr) {
			throw "Cannot order PIs if a pi is not of the correct type";
		}
		toReturn.push_back(cast);
	}
	std::sort(toReturn.begin(), toReturn.end(), connectingSortFunction);
	return toReturn;
}

class C432Tests : public ::testing::Test {
public:
	void SetUp() override {
		observeTPs = observeGen.allTPs(modifiedCircuit);
		all_tps = { observeTPs };
		originalPisOrdered = orderedPis(originalCircuit);
		modifiedPisOrdered = orderedPis(modifiedCircuit);
	}



	Parser<COP_TPI_Line<VALUE>, COP_TPI_Node<VALUE>, VALUE> parser;

	Circuit* originalCircuit = parser.Parse("c432.bench");
	Circuit* modifiedCircuit = parser.Parse("c432.bench");
	TPI_COP<NODETYPE, LINETYPE, VALUE> tpi = TPI_COP<NODETYPE, LINETYPE, VALUE>(modifiedCircuit);
	TPGenerator<TPOBSERVE, NODETYPE, LINETYPE, VALUE> observeGen;
	std::set<TP*> observeTPs;
	std::vector<std::set<TP*>> all_tps;

	std::vector<SimulationNode<VALUE>*> originalPisOrdered;
	std::vector<SimulationNode<VALUE>*> modifiedPisOrdered;

};

//It was noticed that TDF fault coverage DECREASED after TPI was performed: 
//this is an attempt to re-careate this.
TEST_F(C432Tests, ObservePointTests) {
	//PERFORM TPI
	tpi.timeLimit(3600);
	tpi.testpointLimit(0.01*(originalCircuit->nodes().size()));
	std::set<TP*> testpoints = tpi.testpoints(all_tps);
	EXPECT_EQ(testpoints.size(), 2);

	//Fault Simulation Setup
	std::unordered_set<Fault<VALUE>*>originalFaults = FaultGenerator<VALUE>::allFaults(originalCircuit, false);
	FaultSimulator<VALUE>originalSimulator(true);
	originalSimulator.setFaults(originalFaults);
	std::unordered_set<Fault<VALUE>*>modifiedFaults = FaultGenerator<VALUE>::allFaults(modifiedCircuit, false);
	FaultSimulator<VALUE>modifiedSimulator(true);
	modifiedSimulator.setFaults(modifiedFaults);
	ASSERT_EQ(originalFaults.size(), modifiedFaults.size());

	

	//SIMULTE
	PRPG<VALUE> prpg(originalCircuit->pis().size());
	size_t numVec = 10000;
	for (size_t vecNum = 0; vecNum < numVec / 2 ; vecNum++) {
		std::vector<VALUE> vector = prpg.increment();
		originalSimulator.applyStimulus(originalCircuit, vector, EventQueue<VALUE>(), originalPisOrdered);
		modifiedSimulator.applyStimulus(modifiedCircuit, vector, EventQueue<VALUE>(), modifiedPisOrdered);
		ASSERT_EQ(originalSimulator.detectedFaults().size(), modifiedSimulator.detectedFaults().size());
	}

	//Activate TPs
	for (TP* testpoint : testpoints) {
		testpoint->activate(modifiedCircuit);
	}

	for (size_t vecNum = 0; vecNum < numVec / 2; vecNum++) {
		std::vector<VALUE> vector = prpg.increment();
		originalSimulator.applyStimulus(originalCircuit, vector, EventQueue<VALUE>(), originalPisOrdered);
		modifiedSimulator.applyStimulus(modifiedCircuit, vector, EventQueue<VALUE>(), modifiedPisOrdered);
		ASSERT_LE(originalSimulator.detectedFaults().size(), modifiedSimulator.detectedFaults().size());
	}
}

