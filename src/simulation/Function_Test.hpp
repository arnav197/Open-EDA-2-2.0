/**
 * @file Function_Test.cpp
 * @author Joshua Immanuel (jzi0005@tigermail.auburn.edu)
 * @version 0.1
 * @date 2019-01-10
 *
 * @copyright Copyright (c) 2018
 *
 */

#include"gtest/gtest.h"
#include"Function.hpp"

class BFuncTest : public ::testing::Test {
public:
	void SetUp() override {

	}
	Value<bool> val0 =  Value<bool>(0);
	Value<bool> val1 =  Value<bool>(1);
	Value<bool> val;
	std::vector<Value<bool>> val00 = { val0,val0 };
	std::vector<Value<bool>> val01 = { val0,val1 };
	std::vector<Value<bool>> val10 = { val1,val0 };
	std::vector<Value<bool>> val11 = { val1,val1 };
	std::vector<Value<bool>> vals1 = { val1 };
	std::vector<Value<bool>> vals0 = { val0 };
	std::vector<Value<bool>> val_empty = { };
	//BooleanFunction bfunc;
	BooleanFunction<Value<bool>>* and = new BooleanFunction<Value<bool>>("and");
	BooleanFunction<Value<bool>>* nand = new BooleanFunction<Value<bool>>("nand");
	BooleanFunction<Value<bool>>* or = new BooleanFunction<Value<bool>>("or");
	BooleanFunction<Value<bool>>* nor = new BooleanFunction<Value<bool>>("nor");
	BooleanFunction<Value<bool>>* buf = new BooleanFunction<Value<bool>>("bUf");
	BooleanFunction<Value<bool>>* xor = new BooleanFunction<Value<bool>>("XOR");
	BooleanFunction<Value<bool>>* xnor = new BooleanFunction<Value<bool>>("xnor");
	BooleanFunction<Value<bool>> * not = new BooleanFunction<Value<bool>>("noT");

};


//BooleanFunction(std::string _functionString);
TEST(BFuncConstructorTest, TEST01) {
	EXPECT_ANY_THROW(BooleanFunction<Value<bool>> bfunc);
}

//BooleanFunction(std::string _functionString);
TEST(BFuncConstructorTest, TEST02) {
	EXPECT_NO_THROW(BooleanFunction<Value<bool>> bfunc("and"););
	EXPECT_NO_THROW(BooleanFunction<Value<bool>> bfunc("nand"););
	EXPECT_NO_THROW(BooleanFunction<Value<bool>> bfunc("or"););
	EXPECT_NO_THROW(BooleanFunction<Value<bool>> bfunc("nor"););
	EXPECT_NO_THROW(BooleanFunction<Value<bool>> bfunc("noT"););
	EXPECT_NO_THROW(BooleanFunction<Value<bool>> bfunc("bUf"););
	EXPECT_NO_THROW(BooleanFunction<Value<bool>> bfunc("XOR"););
	EXPECT_NO_THROW(BooleanFunction<Value<bool>> bfunc("xnor"););
}

//BooleanFunction(std::string _functionString);
TEST(BFuncConstructorTest, TEST03) {
	EXPECT_ANY_THROW(BooleanFunction<Value<bool>> bfunc("for"););
	EXPECT_ANY_THROW(BooleanFunction<Value<bool>> bfunc("absor"););
}

//std::string string() const;
TEST_F(BFuncTest, ANDTEST01) {
	EXPECT_EQ(and->string(), "and");
}

//std::string string() const;
TEST_F(BFuncTest, STRINGTEST02) {
	EXPECT_EQ(not->string(), "not");	// The string should be completely converted to lowercase.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTAND01) {
	val = and->evaluate(val11);		// AND Gate is evaluated with an input vector of 1,1. 
	EXPECT_EQ(1,val.magnitude() );	// The output should be 1.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTAND02) {
	val = and->evaluate(val00);		// AND Gate is evaluated with an input vector of 0,0.
	EXPECT_EQ(0, val.magnitude());	// The output should be 0.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTOR01) {
	val = or->evaluate(val10);		// Test OR Gate with an input vector of 1,0.
	EXPECT_EQ(1, val.magnitude());	// The output should be 1.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTOR02) {
	val = or ->evaluate(val00);		// Test OR Gate with an input vector of 0,0.
	EXPECT_EQ(0, val.magnitude());	// The output should be 0.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTOR03) {
	val = or ->evaluate(val01);		// Test OR Gate with an input vector of 0,1.
	EXPECT_EQ(1, val.magnitude());	// The output should be 1.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTNAND01) {
	val = nand->evaluate(val01);	// Evaluate the NAND Gate with an input vector of 0,1.
	EXPECT_EQ(1, val.magnitude());	// The expected output should be 1.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTNAND02) {
	val = nand->evaluate(val11);	// Evaluate the NAND Gate with an input vector of 1,1.
	EXPECT_EQ(0, val.magnitude());	// The expected output should be 0.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTNOR01) {
	val = nor->evaluate(val11);		// Evaluate the input to the NOR Gate with an input vector of 1,1.
	EXPECT_EQ(0, val.magnitude());	// The output should be 0.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTNOR02) {
	val = nor->evaluate(val10);		// Evaluate the input to the NOR Gate with an input vector of 1,0.
	EXPECT_EQ(0, val.magnitude());	// The output should be 0.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTNOR03) {
	val = nor->evaluate(val00);		// Evaluate the input to the NOR Gate with a vector of 0,0.
	EXPECT_EQ(1, val.magnitude());	// The output should be 1.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTNOT01) {		
	val = not->evaluate(vals1);		// Test NOT Gate with an input vector of 1.
	EXPECT_EQ(0, val.magnitude());	// The output should be 0.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTNOT02) {
	val = not->evaluate(vals0);		// Test NOT Gate with an input vector 0f 0.
	EXPECT_EQ(1, val.magnitude());	// Expected output should be 1.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTBUF01) {
	val = buf->evaluate(vals1);		// Evaluate the Buffer input with a value of 1.
	EXPECT_EQ(1, val.magnitude());	// Expected output should be 1.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTBUF02) {
	val = buf->evaluate(vals0);		// Evaluate the Buffer input with a value of zero.
	EXPECT_EQ(0, val.magnitude());	// The output should be zero.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, VALIDITYTEST) {
	EXPECT_ANY_THROW(val = buf->evaluate(val_empty););
	
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTXOR01) {
	val = xor->evaluate(val11);		 // Evaluate the XOR gate with an input vector of 1,1.
	EXPECT_EQ(0, val.magnitude());	 // Expect an output of 0.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTXOR02) {
	val = xor->evaluate(val10);		// Evaluate the XOR gate with an input vector of 1,0.
	EXPECT_EQ(1, val.magnitude());	// Expect an output of 1.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTXOR03) {
	val = xor->evaluate(val00);		// Evaluate the XOR gate with an input vector of 0,0.
	EXPECT_EQ(0, val.magnitude());	// Expect an output of 0.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTXNOR01) {
	val = xnor->evaluate(val11);	// Evaluate the XNOR gate with an input vector of 1,1.
	EXPECT_EQ(1, val.magnitude());	// Expect an ouput of 1.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTXNOR02) {
	val = xnor->evaluate(val10);	// Evaluate the XNOR gate with an input vector of 1,0.
	EXPECT_EQ(0, val.magnitude());	// Expect an output of 0.
}

//virtual Value<bool> evaluate(std::vector<Value<bool>> _vector) const;
TEST_F(BFuncTest, TESTXNOR03) {
	val = xnor->evaluate(val00);	// Evaluate the XNOR gate with an input vector of 0,0.
	EXPECT_EQ(1, val.magnitude());	// Expect an output of 1.
}

