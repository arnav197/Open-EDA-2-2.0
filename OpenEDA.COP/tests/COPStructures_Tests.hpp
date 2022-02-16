/**
 * @file COPStructures_Tests.cpp
 * @author Joshua Immanuel (jzi0005@tigermail.auburn.edu)
 * @version 0.1
 * @date 2019-03-01
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <map>

#include "gtest/gtest.h"
#include "COPStructures.hpp"
#include "Parser.hpp"

class B19Test : public ::testing::Test {
public:
	void SetUp() override {

	}

	Parser<COPLine<Value<bool>>, COPNode<Value<bool>>, Value<bool>> parser;
	Circuit* b19 = parser.Parse("b19.p.bench");
};

class C17Test : public ::testing::Test {
public:
	void SetUp() override {

	}

	Parser<COPLine<Value<bool>>, COPNode<Value<bool>>, Value<bool>> parser;
	Circuit* c17 = parser.Parse("c17.bench");

	std::map<std::string, float> CC =
	{
		{"1", 0.5},
		{"2", 0.5},
		{"3", 0.5}, //has fan-outs
		{"6", 0.5},
		{"7", 0.5},
		{"10", 0.75},
		{"11", 0.75}, //has fan-outs
		{"16", 0.625}, //has fan-outs
		{"19", 0.625},
		{"22", 0.53125},
		{"23", 0.609375}
	};

	std::map<std::string, float> baseCO =
	{
		{"1", 0.3125},
		{"2", 0.6796875},
		{"3", 0.527008056640625}, //has fan-outs
		{"6", 0.31201171875},
		{"7", 0.46875},
		{"10", 0.625},
		{"11", 0.6240234375}, //has fan-outs
		{"16", 0.90625}, //has fan-outs
		{"19", 0.625},
		{"22", 1.0},
		{"23", 1.0}
	};

	//Because no two fan-outs feed the same gate, I identify fan-outs only by the gate name.
	std::map<std::string, float> fanoutCO =
	{
		{"10", 0.3125},
		{"11", .31201171875},
		{"16", 0.453125},
		{"19", 0.3125},
		{"22", 0.75},
		{"23", 0.625}
	};

	void ForwardCOPTraceAndCheck(COP* obj) {
		//1 Is this is a line? If so, check its value (don't care about checking nodes).
		COPLine<Value<bool>>* lineCast = dynamic_cast<COPLine<Value<bool>>*>(obj);
		if (lineCast != nullptr) {
			//CHOICE: Is this a fan-out?
			std::string lineName = obj->name();
			float expCC = CC.at(lineName);
			bool isFanOut = false;
			if (obj->inputs().size() != 0) {
				Connecting* input = *(obj->inputs().begin());
				if (dynamic_cast<COPNode<Value<bool>>*>(input) == nullptr) {
					isFanOut = true;
					Connecting* fanOutNode = *(obj->outputs().begin());
					Connecting* fanOutLine = *(fanOutNode->outputs().begin());
					lineName = fanOutLine->name();
				}
			}

			
			float expCO = -404;
			//YES: this is a fanout
			if (isFanOut == true) {
				expCO = fanoutCO.at(lineName);
			} else {//NO: this is NOT a fan-out
				expCO = baseCO.at(lineName);
			}
			float foundCC = obj->controllability();
			float foundCO = obj->observability();
			if (foundCC != expCC) {
				throw "CC MISMATCH";
			}
			if (foundCO != expCO) {
				throw "CO MISMATCH";
			}
		}

		

		//2 Forward the call to all outputs.
		for (Connecting* output : obj->outputs()) {
			COP* outputCast = dynamic_cast<COP*>(output);
			if (outputCast == nullptr) {
				throw "We have a serious problem here.";
			}
			ForwardCOPTraceAndCheck(outputCast);
		}
	}

};


//This is an exaustive test for c17
TEST_F(C17Test, ExhaustiveTest) {
	for (Levelized* pi : c17->pis()) {
		COPNode<Value<bool>>* cast = dynamic_cast<COPNode<Value<bool>>*>(pi);
		if (cast == nullptr) {
			throw "You have a serious problem here.";
		}
		EXPECT_NO_THROW(ForwardCOPTraceAndCheck(cast));
	}
}

//This tests confirms observabilty can be calcualted for all b19 inputs.
//This was created for a latent bug.
//This test can take some time to run, but this is expected: b19 is a large
// circuit.
TEST_F(B19Test, InputObservabilityTest) {
	for (Levelized* pi : b19->pis()) {
		COPNode<Value<bool>>* cast = dynamic_cast<COPNode<Value<bool>>*>(pi);
		if (cast == nullptr) {
			throw "You have a serious problem here.";
		}
		EXPECT_NO_THROW(cast->observability());
	}
}

