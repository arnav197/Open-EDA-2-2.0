/**
 * @file SimulationStructure_Test.cpp
 * @author Joshua Immanuel (jzi0005@tigermail.auburn.edu)
 * @version 0.1
 * @date 2019-01-28
 *
 * @copyright Copyright (c) 2018
 *
 */

#include"gtest/gtest.h"
#include"SimulationStructures.hpp"
#include "EventQueue.hpp"


//SimulationLine()
//TEST(SimLine_Constructor_Test, TEST01) {
//	EXPECT_NO_THROW(SimulationLine<bool, Value<bool>> sl;);
//}

//SimulationLine()
TEST(SimLine_Constructor_Test, TEST02) {
	SimulationLine<Value<bool>> sl;
	EXPECT_EQ("UNDEF",sl.name());
}

//SimulationLine()
TEST(SimLine_Constructor_Test, TEST03) {
	SimulationLine<Value<bool>> sl("Test_Line");
	EXPECT_EQ("Test_Line", sl.name());
}

//SimulationNode()
//TEST(SimNode_Constructor_Test, TEST01) {
//	EXPECT_NO_THROW(SimulationNode<bool, Value<bool>> sn()); //Simulation Node has a default constructor.
//}

class SimNode_Test : public ::testing::Test {
public:
	void SetUp() override {
		l1->inputLevelZero();
	}
	std::unordered_set<SimulationLine<Value<bool>>*> empty = {};
	SimulationLine<Value<bool>>* l1 = new SimulationLine<Value<bool>>("l1");
	SimulationLine<Value<bool>>* l2 = new SimulationLine<Value<bool>>("l2");
	std::unordered_set<SimulationLine<Value<bool>>*> in_l_n1 = { l1 };
	std::unordered_set<SimulationLine<Value<bool>>*> o_l_n1 = { l2 };
	Function<Value<bool>>* func = new BooleanFunction<Value<bool>>("nand");
	SimulationNode<Value<bool>>* sn = new SimulationNode<Value<bool>>(func, "nand", in_l_n1, o_l_n1);
	Value<bool> val1 =  Value<bool>(1);
	Value<bool> val2 =  Value<bool>(0);
	Value<bool> val3 =  Value<bool>(1);
	Value<bool> val4 =  Value<bool>(1);
	Value<bool> val;
	std::vector<Value<bool>>vec_val = { val1,val2 };
};
//SimulationNode()
TEST_F(SimNode_Test, TEST01) {
	EXPECT_EQ(val1.magnitude() , sn->function()->evaluate(vec_val).magnitude()); // Check if  the NAND gate evaluated with a vector of 1,0 returns an output of 1.
}

//SimulationNode()
//TEST_F(SimNode_Test, TEST02) {
//
//	std::set<std::pair<size_t, Evented<bool>*>> event_queue = sn->go();
//	EXPECT_TRUE(event_queue.empty()); //The input values are the same before and after: X
//}

//SimulationNode()
TEST_F(SimNode_Test, TEST03) {

	EXPECT_NO_THROW(  sn->go(); ); //Should return the event stack for the simulation node.
}