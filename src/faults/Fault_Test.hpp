/**
 * @file Fault_Test.cpp
 * @author Joshua Immanuel (jzi0005@tigermail.auburn.edu)
 * @version 0.1
 * @date 2019-01-28
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "gtest/gtest.h"
#include "faults/FaultStructures.hpp"
#include "faults/FaultGenerator.hpp"
#include "parser/Parser.hpp"
#include "faults/Fault.hpp"
#include "faults/FaultyValue.hpp"

class FaultTest : public ::testing::Test {
protected:
	void SetUp() override {
		Parser<FaultyLine<FaultyValue<bool>>, FaultyNode<FaultyValue<bool>>, FaultyValue<bool>> parser;
		Circuit* c = parser.Parse("c17.bench");
		FaultGenerator<FaultyValue<bool>> fgen;
		std::unordered_set<Fault<FaultyValue<bool>>*> faults = fgen.allFaults(c);
		faultUnderTest = *(faults.begin());

	}
	
	FaultyLine<FaultyValue<bool>>* fl1 = new FaultyLine<FaultyValue<bool>>("Faulty Line 1");
	FaultyLine<FaultyValue<bool>>* fl2 = new FaultyLine<FaultyValue<bool>>("Faulty Line 2");
	FaultyValue<bool> val0 = FaultyValue<bool>(0);;
	FaultyValue<bool> val1 = FaultyValue<bool>(1);;
	Fault<FaultyValue<bool>>* faultUnderTest;
	Fault<FaultyValue<bool>> f1 = Fault<FaultyValue<bool>>(fl1, val0);
	int flag = 0;
};

//TEST_F(FaultTest, Constructors) {
//	//FaultyLine<bool> fl;
//	//FaultyValue<bool> val(0);
//	EXPECT_NO_THROW(Fault<bool, FaultyValue<bool>> f());
//	EXPECT_NO_THROW(Fault<bool, FaultyValue<bool>> f(fl1, val0));
//	Fault<bool, FaultyValue<bool>> f2(f1);
//	EXPECT_EQ(f1.value(), f2.value()); //Copy constructor
//
//}

TEST_F(FaultTest, Overwritten_Operators) {
	Fault<FaultyValue<bool>>f2 = f1;
	EXPECT_EQ(f1.value(), f2.value()); //Assign (copy) constructor
	EXPECT_EQ(f1,f2); //Equal constructor
	f2 = Fault<FaultyValue<bool>>(fl1, val1);
	EXPECT_NE(f1, f2);  //Not equal constructor
}

//value();
TEST_F(FaultTest, value) {
	EXPECT_EQ(f1.value(), val0);
}

//go() const;
TEST_F(FaultTest, go) {
	try {
		EXPECT_NO_THROW(faultUnderTest->go());
	} catch (const std::exception& exc) {
		printf("EXCEPTION!\n");
		std::cout << exc.what();
	}
	EXPECT_NO_THROW(faultUnderTest->go());
}

class FaultyTest : public ::testing::Test {
protected:
	void SetUp() override {
	}
	FaultyLine<FaultyValue<bool>>* fl = new FaultyLine<FaultyValue<bool>>("Faulty Line 1");
	Fault<FaultyValue<bool>>* f = new Fault<FaultyValue<bool>>(fl, FaultyValue<bool>(0));
};


//Faulty();
//TEST_F(FaultyTest, ConstructorsTests) {
//	EXPECT_NO_THROW(Faulty<bool, FaultyValue<bool>> fauly);
//}

//Private Functions cannot be tested
//bool isFaultActive(Fault<_primitive> _fault);
//TEST_F(FaultyTest, activate_deactivate) {
//	fl->activate(f);
//	EXPECT_TRUE(fl->isFaultActive(f));
//	fl->deactivate(f);
//	EXPECT_FALSE(fl->isFaultActive(f));
//}
