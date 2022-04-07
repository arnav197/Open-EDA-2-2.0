
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

#include <cstdlib> //atoi

#include "parser/Parser.hpp"
#include "sat/SATStructures.hpp"
#include "sat/SAT.hpp"

#define COMBINATION Combination<SATLine<Value<bool>>, SATNode<Value<bool>>, Value<bool>>

class C17Tests : public ::testing::Test {
public:
	void SetUp() override {
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("1", "10"), std::pair<Value<bool>, Value<bool>>(o, o)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("2", "16"), std::pair<Value<bool>, Value<bool>>(o, o)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("3", "10"), std::pair<Value<bool>, Value<bool>>(o, o)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("3", "11"), std::pair<Value<bool>, Value<bool>>(o, o)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("6", "11"), std::pair<Value<bool>, Value<bool>>(o, o)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("7", "19"), std::pair<Value<bool>, Value<bool>>(o, o)));




		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("10", "22"), std::pair<Value<bool>, Value<bool>>(o, o)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("11", "16"), std::pair<Value<bool>, Value<bool>>(o, o)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("11", "19"), std::pair<Value<bool>, Value<bool>>(o, o)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("11", "23"), std::pair<Value<bool>, Value<bool>>(o, i)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("16", "22"), std::pair<Value<bool>, Value<bool>>(o, o)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("16", "23"), std::pair<Value<bool>, Value<bool>>(o, o)));
		impossibleCombinations.push_back(std::pair<
										 std::pair<std::string, std::string>,
										 std::pair<Value<bool>, Value<bool>>
		>(std::pair<std::string, std::string>("19", "23"), std::pair<Value<bool>, Value<bool>>(o, o)));

		initializeAllCombinations();
	}

	Parser<SATLine<Value<bool>>, SATNode<Value<bool>>, Value<bool>> parser;
	Circuit* c17 = parser.Parse("c17.bench");
	SAT<Value<bool>> sat = SAT<Value<bool>>(3600);
	
	Value<bool> o = Value<bool>(false);
	Value<bool> i = Value<bool>(true);

	//This data structure contains ALL impossible line-pair combinations.
	//The first first part of the pair is the pair of line names (lower number always first).
	//The second part of the pair is the pair of values which is impossible.
	std::vector<
		std::pair<
		std::pair<std::string, std::string>,
		std::pair<Value<bool>, Value<bool>>
		>
	> impossibleCombinations;

	//This data structure contains ALL line combinations in the circuit. It is 
	//initialized using the following function, which is called in "SetUp".
	std::unordered_set<COMBINATION*> allCombinations;

	void initializeAllCombinations() {
		for (Levelized* firstNode : c17->nodes()) {
			if (firstNode->outputs().size() == 0) { continue; } //Skip POs.
			SATLine<Value<bool>>* firstCast = dynamic_cast<SATLine<Value<bool>>*>(*(firstNode->outputs().begin()));
			for (Levelized* secondNode : c17->nodes()) {
				if (firstNode == secondNode) { continue; } //Same node (and therefore, same line)
				if (secondNode->outputs().size() == 0) { continue; } //Skip POs.
				SATLine<Value<bool>>* secondCast = dynamic_cast<SATLine<Value<bool>>*>(*(secondNode->outputs().begin()));
				std::vector<Value<bool>> values = std::vector<Value<bool>>(2, Value<bool>(false));
				do {
					COMBINATION* combination;
					combination = new COMBINATION(
						std::vector<SATLine<Value<bool>>*>({ firstCast, secondCast }),
						values
						);
					allCombinations.emplace(combination);
				} while (ValueVectorFunction<Value<bool>>::increment(values) == true);
			}
			
		}
	}
	
	//Return true if the given line (names) and the given values is impossible
	bool isImpossible(COMBINATION* combination
		//std::string _line1, std::string _line2, Value<bool> _val1, Value<bool> _val2
	) {
		std::vector<std::pair<Evented<Value<bool>>*, Value<bool>>> combinationExtraction = combination->frontier();
		std::string _line1 = combinationExtraction.at(0).first->name();//combination->lines().at(0)->name();
		std::string _line2 = combinationExtraction.at(1).first->name();
		Value<bool> _val1 = combinationExtraction.at(0).second;// combination->targetValues().at(0);
		Value<bool> _val2 = combinationExtraction.at(1).second;
		if (std::atoi(_line1.c_str()) > std::atoi(_line2.c_str())) { //Numerically, _line1 should be before _line2
			std::string tmp = _line1;
			_line1 = _line2;
			_line2 = tmp;
			//Remember: flip the values too.
			Value<bool> tmpVal = _val1;
			_val1 = _val2;
			_val2 = tmpVal;
		}
		for (std::pair<
			 std::pair<std::string, std::string>,
			 std::pair<Value<bool>, Value<bool>>
		> impossibleCombination : impossibleCombinations) {
			if (_line1 != impossibleCombination.first.first || _line2 != impossibleCombination.first.second) {
				continue;
			}
			if (_val1 != impossibleCombination.second.first || _val2 != impossibleCombination.second.second) {
				continue;
			}
			return true;
		}
		return false;
	}
	
};

//Every node input combination in this circuit should be posible.
//This test will 1) find all such combinations are possible and 2) confirm that
//the returned vector will in fact create the combination.
TEST_F(C17Tests, AllNodeCombinations) {
	for (Levelized* node : c17->nodes()) {
		SATNode<Value<bool>>* nodeCast = dynamic_cast<SATNode<Value<bool>>*>(node);
		std::unordered_set<COMBINATION*> nodeCombinations = COMBINATION::allNodeCombinations(nodeCast);
		std::vector<
			Goal<Value<bool>>*
		> possible =
			sat.satisfy(
				std::unordered_set<Goal<Value<bool>>*>(nodeCombinations.begin(), nodeCombinations.end())
				);
		EXPECT_EQ(nodeCombinations.size(), possible.size());
		for(COMBINATION* combination : nodeCombinations){
			delete combination;
		}
	}
}

//Every line pair combination is NOT possible. This will check all of them. Those which are known to be impossible are set in "SetUp".
TEST_F(C17Tests, AllLinePairCombinations) {
	for (COMBINATION* combination : allCombinations) {
		bool isPossible =
			sat.satisfy(
				std::unordered_set<Goal<Value<bool>>*>({combination})
			).size();

		bool foundImpossible = (isPossible == false);
		bool expectImpossible = isImpossible(combination);
		EXPECT_EQ(foundImpossible, expectImpossible);
		if (foundImpossible != expectImpossible) {
			expectImpossible = isImpossible(combination);
			int i = 0;
		}
	}
}
