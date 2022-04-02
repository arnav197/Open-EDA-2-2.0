#pragma once

#include "gtest/gtest.h"
#include "expand/Expand.hpp"
#include "simulation/SimulationStructures.hpp"
#include "parser/Parser.hpp"
#include "Circuit.h"

class ExpandTest : public::testing::Test {
public:
	void SetUp() override {

	}
	Parser<SimulationLine<Value<bool>>, SimulationNode<Value<bool>>, Value<bool>> parser;
	Circuit* circuit = parser.Parse("C://lab2//EDA2.0//Open EDA 2.0//c880.bench");
};


 //Circuit* Expand_in(Circuit* _c)
TEST_F(ExpandTest, TEST01)
{
	Expand  <Value<bool>, SimulationNode < Value<bool>>, SimulationLine<Value<bool>>> expand_1;
	
	Circuit* c_change_1 = expand_1.Expand_in(circuit);
	for (Levelized* n : c_change_1->nodes())
	{
		EXPECT_LE(n->inputs().size(), 2);
	}


}



// Circuit* Expand_out(Circuit* _c)
TEST_F(ExpandTest, TEST02)
{
	Expand  <Value<bool>, SimulationNode < Value<bool>>, SimulationLine<Value<bool>>> expand_2;
	Circuit* c_change_2 = expand_2.Expand_in(circuit);
	for (Levelized* n : c_change_2->nodes())
	{
		EXPECT_LE(n->outputs().size(), 2);
	}

}



