
#pragma once
/**
 * @file Testpoint_Test.cpp
 * @author Spencer Millican (millican@auburn.edu)
 * @version 0.1
 * @date 2019-05-13
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
#include "Moghaddam_Structures.hpp"

#define PRIMITIVE FaultyValue<bool>
#define LINETYPE Moghaddam_Line<PRIMITIVE>
#define NODETYPE Moghaddam_Node<PRIMITIVE>

class C17_Moghaddam_Tests : public ::testing::Test {
public:
	void SetUp() override {
	}

	void TearDown() override {
		for (Fault<PRIMITIVE>* fault : faults) {
			delete fault;
		}
		delete c17;
	}

	Parser<LINETYPE, NODETYPE, PRIMITIVE> parser;
	Circuit* c17 = parser.Parse("c17.bench");

	FaultGenerator<PRIMITIVE> faultGenerator;
	std::unordered_set<Fault<PRIMITIVE>*> faults = faultGenerator.allFaults(c17);


	//Expected number of faults detected when the given TP is present (observes only).
	std::map<std::string, float> ExpD = {
{"1",2},
{"2",2},
{"3",2},
{"3_10",3.00078186082877},
{"3_11",2.99921813917123},
{"6",2},
{"7",2},
{"10",5.00078186082877},
{"11",4.99921813917123},
{"11_16",4.9587209395095},
{"11_19",4.04049719966173},
{"16",6.9587209395095},
{"16_22",5.79566596700518},
{"16_23",5.16305497250432},
{"19",6.04049719966173},
{"22",10.796447827834},
{"23",11.203552172166}
	};

	std::map<std::string, float> Expb = {
		{"1",0},
{"2",0},
{"3",0},
{"3_10",0},
{"3_11",0},
{"6",0},
{"7",0},
{"10",0},
{"11",0},
{"11_16",0},
{"11_19",0},
{"16",0},
{"16_22",0},
{"16_23",0},
{"19",0},
{"22",0},
{"23",0}
	};

	std::map<std::string, float> ExpB = {
	{"1",3.00078186082877},
{"2",4.9587209395095},
{"3",4},
{"3_10",2},
{"3_11",2},
{"6",2.99921813917123},
{"7",4.04049719966173},
{"10",5.79566596700518},
{"11",4},
{"11_16",2},
{"11_19",2},
{"16",11.0412790604905},
{"16_22",5.00078186082877},
{"16_23",6.04049719966173},
{"19",5.16305497250432},
{"22",0},
{"23",0}
	};

	std::map<std::string, float> ExpOmega = {
	{"1",1.01029995663981},
{"2",0.3353814340585},
{"3",0.55636549091145},
{"3_10",1.51584489194042},
{"3_11",1.51709179448141},
{"6",1.01165818829079},
{"7",0.65811743852845},
{"10",1.02075950669843},
{"11",1.02383536801419},
{"11_16",1.7047188834383},
{"11_19",2.04105707281076},
{"16",0.297499101360764},
{"16_22",0.724103183721349},
{"16_23",1.05388269143917},
{"19",1.23298618362811},
{"22",0},
{"23",0}
	};

	std::map<std::string, float> Expc = {
	{"1",0},
{"2",0},
{"3",0},
{"3_10",0},
{"3_11",0},
{"6",0},
{"7",0},
{"10",0},
{"11",0},
{"11_16",0},
{"11_19",0},
{"16",0},
{"16_22",0},
{"16_23",0},
{"19",0},
{"22",0},
{"23",0}
	};

	std::map<std::string, float> ExpC = {
	{"1",1.51584489194042},
{"2",0.831531469894304},
{"3",1.1127309818229},
{"3_10",1.01029995663981},
{"3_11",1.01165818829079},
{"6",1.51709179448141},
{"7",1.32956083371137},
{"10",1.18301123666463},
{"11",0.819196393925647},
{"11_16",0.687563952169862},
{"11_19",1.01029995663981},
{"16",0.472036546216334},
{"16_22",0.62479136774565},
{"16_23",1.23298618362811},
{"19",1.05388269143917},
{"22",0},
{"23",0}
	};

	void EvalLine(Connecting* line) {
		std::string lineName = line->name();
		std::string fanout = "";
		if (line->inputs().size() != 0) {
			Connecting* input = *(line->inputs().begin());
			if (dynamic_cast<NODETYPE*>(input) == nullptr) {
				Connecting* fanOutNode = *(line->outputs().begin());
				Connecting* fanOutLine = *(fanOutNode->outputs().begin());
				lineName = lineName + "_" + fanOutLine->name();
			}
		}
		LINETYPE* cast = dynamic_cast<LINETYPE*>(line);
		float D = cast->D();
		float B = cast->B(false);
		float b = cast->B(true);
		EXPECT_NEAR(D, ExpD.at(lineName), 0.01);
		EXPECT_NEAR(B, ExpB.at(lineName), 0.01);
		EXPECT_NEAR(b, Expb.at(lineName), 0.01);
	}
};



TEST_F(C17_Moghaddam_Tests, C17_Moghaddam_Values) {
	for (Levelized* node : c17->nodes()) {
		for (Connecting* line : node->inputs()) {
			EvalLine(line);
		}		
	}
}