#pragma once
/**
 * @file ATPGTests.hpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-12-20
 *
 * @copyright Copyright (c) 2019
 *
 */

#include"gtest/gtest.h"


#include "Parser.hpp"
#include "ATPGStructures.hpp"
#include "DFrontier.hpp"
#include "SAT.hpp"
#include "Fault.hpp"
#include "FaultyValue.hpp"
#include "FaultGenerator.hpp"

#define PRIMITIVE bool
#define VALUE_TYPE FaultyValue<PRIMITIVE>
#define LINE_TYPE ATPGLine<VALUE_TYPE>
#define NODE_TYPE ATPGNode<VALUE_TYPE>
#define FAULT_TYPE Fault<VALUE_TYPE>
#define SAT SAT<VALUE_TYPE>



class C17ATPGTests : public ::testing::Test {
public:
	void SetUp() override {

	}
	
	Parser<LINE_TYPE, NODE_TYPE, VALUE_TYPE> parser;
	Circuit* c17 = parser.Parse("c17.bench");
	SAT sat = SAT(3600);

	FaultGenerator<VALUE_TYPE> faultGenerator;
	EventQueue<VALUE_TYPE> simulationQueue;
	std::unordered_set<FAULT_TYPE*> allfaults = faultGenerator.allFaults(c17);
};

TEST_F(C17ATPGTests, c17ATPG) {
	for (FAULT_TYPE* fault : allfaults) {
		printf("TEST: Now satisfying fault: %s sa %s...\n",
			fault->location()->name().c_str(),
			fault->value().name().c_str()
		);
		DFrontier<VALUE_TYPE>* goal = new DFrontier<VALUE_TYPE>(fault); //Create a SAT objective.
		simulationQueue.add(fault->go()); simulationQueue.process(); //Activate the fault (and update the circuit)
		ASSERT_TRUE(sat.satisfy(goal));
		simulationQueue.add(fault->go()); simulationQueue.process(); //Deactivate the fault (and update the circuit)
		delete goal;
	}
}



class C432ATPGTests : public ::testing::Test {
public:
	void SetUp() override {

	}

	Parser<LINE_TYPE, NODE_TYPE, VALUE_TYPE> parser;
	Circuit* c432 = parser.Parse("c432.bench");
	SAT sat = SAT(3600);

	FaultGenerator<VALUE_TYPE> faultGenerator;
	EventQueue<VALUE_TYPE> simulationQueue;
	std::unordered_set<FAULT_TYPE*> allfaults = faultGenerator.allFaults(c432);

	size_t expectedNumRedundant = 7; //The specific faults which are redundant are...
	//213->259 sa0
	//102->259 sa0
	//319->347 sa0
	//112->347 sa0
	//360->379 sa0
	//115->379 sa0
	//393->429 sa1
	//These have all been confirmed using the TESTCAD toolset.
};

TEST_F(C432ATPGTests, c432ATPG) {
	size_t numDetected = 0;
	size_t numRedundant = 0;
	for (FAULT_TYPE* fault : allfaults) {
		printf("TEST: Now satisfying fault: %s sa %s...",
			fault->location()->name().c_str(),
			fault->value().name().c_str()
		);
		DFrontier<VALUE_TYPE>* goal = new DFrontier<VALUE_TYPE>(fault); //Create a SAT objective.
		simulationQueue.add(fault->go()); simulationQueue.process(); //Activate the fault (and update the circuit)
		if (sat.satisfy(goal))
		{
			printf("detected.\n");
			numDetected++;
		}
		else
		{
			printf("redundant.\n");
		}
		simulationQueue.add(fault->go()); simulationQueue.process(); //Deactivate the fault (and update the circuit)
		delete goal;
	}
	EXPECT_EQ(numRedundant, expectedNumRedundant);
}