/**
 * @file FaultGenerator_Test.cpp
 * @author Joshua Immanuel (jzi0005@tigermail.auburn.edu)
 * @version 0.1
 * @date 2019-02-18
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "gtest/gtest.h"
#include "faults/FaultGenerator.hpp"
#include "faults/FaultStructures.hpp"
#include "parser/Parser.hpp"
#include "faults/FaultyValue.hpp"

#include <iostream>

class FaultGeneratorTest : public ::testing::Test {
public:
	void SetUp() override {

	}
	Parser<FaultyLine<FaultyValue<bool>>, FaultyNode<FaultyValue<bool>>, FaultyValue<bool>> parser;
	Circuit* ckt = parser.Parse("c17.bench");
	FaultGenerator<FaultyValue<bool>>* fgen;
	std::unordered_set<Fault<FaultyValue<bool>>*> c17faults = fgen->allFaults(ckt);
};

//std::set<Fault<_primitive>> allFaults(Circuit* _circuit);
TEST_F(FaultGeneratorTest, c17) {
	for (Fault<FaultyValue<bool>>* fault : c17faults) {
		std::string locationName = fault->location()->name();
		bool value = fault->value().magnitude();
		std::cout << locationName ;
		std::cout << "\t";
		std::cout << value;
		std::cout << "\n";
	}
	EXPECT_EQ(22,c17faults.size());
}