/**
 * @file FaultStructure_Test.cpp
 * @author Joshua Immanuel (jzi0005@tigermail.auburn.edu)
 * @version 0.1
 * @date 2019-01-28
 *
 * @copyright Copyright (c) 2018
 *
 */

#include"gtest/gtest.h"
#include"FaultStructures.hpp"
#include "FaultyValue.hpp"

class FaultyLineTest : public ::testing::Test {
public:
	void SetUp() override {

	}
	FaultyLine<FaultyValue<bool>> fl;
	std::string name = fl.name();
	FaultyLine<FaultyValue<bool>>* flnamed = new FaultyLine<FaultyValue<bool>>("Test_Line");
};

 //FaultyLine();
//TEST_F(FaultyLineTest, ConstructorTEST01) {
//	EXPECT_NO_THROW(FaultyLine<bool, FaultyValue<bool>> flt;);
//}

//FaultyLine();
TEST_F(FaultyLineTest, ConstructorTEST02) {
	EXPECT_EQ("UNDEF", name);
}

//FaultyLine();
TEST_F(FaultyLineTest, ConstructorTEST03) {
	std::string name = flnamed->name();
	EXPECT_EQ("Test_Line", name);
}