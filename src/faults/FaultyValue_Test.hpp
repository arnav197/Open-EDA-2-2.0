/**
 * @file COPStructures_Tests.cpp
 * @author Joshua Immanuel (jzi0005@tigermail.auburn.edu)
 * @version 0.1
 * @date 2019-03-01
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "gtest/gtest.h"
#include "FaultyValue.hpp"

class FaultyValueTest : public ::testing::Test {
public:
	void SetUp() override {
		
	}
	//                                        m     v     
	FaultyValue<bool> o = FaultyValue<bool>(false, true);
	FaultyValue<bool> i = FaultyValue<bool>(true,  true);
	FaultyValue<bool> x = FaultyValue<bool>(false, false);

	FaultyValue<bool> d = FaultyValue<bool>(i, o);
	FaultyValue<bool> u = FaultyValue<bool>(o, i);

	BooleanFunction<FaultyValue<bool>> AND =  BooleanFunction<FaultyValue<bool>>("and");
	BooleanFunction<FaultyValue<bool>> OR = BooleanFunction<FaultyValue<bool>>("or");
	BooleanFunction<FaultyValue<bool>> XOR = BooleanFunction<FaultyValue<bool>>("xor");


	void TearDown() {

	}
};

/*
 * The order test: confirm the "sorting" order is....
 *
 * XX, X0, X1, 0X, 00 (0), 01 (U), 1X, 10 (D), 11 (1)
 *
 * We're not going to check all of these, so the orders we will test is...
 * X->0->U->D->1
 */
TEST_F(FaultyValueTest, SortOrderTest) {
	EXPECT_FALSE(x < x);
	EXPECT_TRUE(x < o);
	EXPECT_TRUE(x < u);
	EXPECT_TRUE(x < d);
	EXPECT_TRUE(x < i);

	EXPECT_FALSE(o < x);
	EXPECT_FALSE(o < o);
	EXPECT_TRUE(o < u);
	EXPECT_TRUE(o < d);
	EXPECT_TRUE(o < i);

	EXPECT_FALSE(u < x);
	EXPECT_FALSE(u < o);
	EXPECT_FALSE(u < u);
	EXPECT_TRUE(u < d);
	EXPECT_TRUE(u < i);

	EXPECT_FALSE(d < x);
	EXPECT_FALSE(d < o);
	EXPECT_FALSE(d < u);
	EXPECT_FALSE(d < d);
	EXPECT_TRUE(d < i);

	EXPECT_FALSE(i < x);
	EXPECT_FALSE(i < o);
	EXPECT_FALSE(i < u);
	EXPECT_FALSE(i < d);
	EXPECT_FALSE(i < i);

	//The "magnitude" on X shouldn't matter, so we'll test it.
	EXPECT_FALSE(FaultyValue<bool>(false, false) < FaultyValue<bool>(false, false));
	EXPECT_FALSE(FaultyValue<bool>(false, false) < FaultyValue<bool>(true, false));
	EXPECT_FALSE(FaultyValue<bool>(true, false) < FaultyValue<bool>(false, false));
	EXPECT_FALSE(FaultyValue<bool>(true, false) < FaultyValue<bool>(true, false));
}

#define O(var) EXPECT_EQ(var.magnitude(), false); EXPECT_EQ(var.valid(), true); EXPECT_EQ(var.faulty(), false);
#define I(var) EXPECT_EQ(var.magnitude(), true); EXPECT_EQ(var.valid(), true); EXPECT_EQ(var.faulty(), false);
#define D(var) EXPECT_EQ(var.magnitude(), false); EXPECT_EQ(var.valid(), true); EXPECT_EQ(var.faulty(), true);
#define U(var) EXPECT_EQ(var.magnitude(), true); EXPECT_EQ(var.valid(), true); EXPECT_EQ(var.faulty(), true);
#define X(var) EXPECT_EQ(var.valid(), false); EXPECT_EQ(var.faulty(), false);
/* INPUTS
 *   0 1 D U X
 * 0 0 0 0 0 0
 * 1 0 1 D U X
 * D 0 D D 0 X <-- NOTE: This was originally "0", but it shouldn't be
 * U 0 U 0 U X
 * X 0 X X X X
 *
 *   0  1  D  U  X
 * 0 0  1  2  3  4
 * 1    5  6  7  8
 * D       9 10 11
 * U         12 13
 * X            14
 */
TEST_F(FaultyValueTest, ANDTest) {
	FaultyValue<bool> out0 =  AND.evaluate({ o, o });
	FaultyValue<bool> out1 =  AND.evaluate({ o, i });
	FaultyValue<bool> out2 =  AND.evaluate({ o, d });
	FaultyValue<bool> out3 =  AND.evaluate({ o, u });
	FaultyValue<bool> out4 =  AND.evaluate({ o, x });
	FaultyValue<bool> out5 =  AND.evaluate({ i, i });
	FaultyValue<bool> out6 =  AND.evaluate({ i, d });
	FaultyValue<bool> out7 =  AND.evaluate({ i, u });
	FaultyValue<bool> out8 =  AND.evaluate({ i, x });
	FaultyValue<bool> out9 =  AND.evaluate({ d, d });
	FaultyValue<bool> out10 = AND.evaluate({ d, u });
	FaultyValue<bool> out11 = AND.evaluate({ d, x });
	FaultyValue<bool> out12 = AND.evaluate({ u, u });
	FaultyValue<bool> out13 = AND.evaluate({ u, x });
	FaultyValue<bool> out14 = AND.evaluate({ x, x });

	O(out0);
	O(out1);
	O(out2);
	O(out3);
	O(out4);
	I(out5);
	D(out6);
	U(out7);
	X(out8);
	D(out9);
	O(out10);
	X(out11);
	U(out12);
	X(out13);
	X(out14);
}

TEST_F(FaultyValueTest, ORTest) {
	FaultyValue<bool> out0 = OR.evaluate({ o, o });
	FaultyValue<bool> out1 = OR.evaluate({ o, i });
	FaultyValue<bool> out2 = OR.evaluate({ o, d });
	FaultyValue<bool> out3 = OR.evaluate({ o, u });
	FaultyValue<bool> out4 = OR.evaluate({ o, x });
	FaultyValue<bool> out5 = OR.evaluate({ i, i });
	FaultyValue<bool> out6 = OR.evaluate({ i, d });
	FaultyValue<bool> out7 = OR.evaluate({ i, u });
	FaultyValue<bool> out8 = OR.evaluate({ i, x });
	FaultyValue<bool> out9 = OR.evaluate({ d, d });
	FaultyValue<bool> out10 = OR.evaluate({ d, u });
	FaultyValue<bool> out11 = OR.evaluate({ d, x });
	FaultyValue<bool> out12 = OR.evaluate({ u, u });
	FaultyValue<bool> out13 = OR.evaluate({ u, x });
	FaultyValue<bool> out14 = OR.evaluate({ x, x });

	O(out0);
	I(out1);
	D(out2);
	U(out3);
	X(out4);
	I(out5);
	I(out6);
	I(out7);
	I(out8);
	D(out9);
	I(out10);
	X(out11);
	U(out12);
	X(out13);
	X(out14);
}

TEST_F(FaultyValueTest, XORTest) {
	FaultyValue<bool> out0 = XOR.evaluate({ o, o });
	FaultyValue<bool> out1 = XOR.evaluate({ o, i });
	FaultyValue<bool> out2 = XOR.evaluate({ o, d });
	FaultyValue<bool> out3 = XOR.evaluate({ o, u });
	FaultyValue<bool> out4 = XOR.evaluate({ o, x });
	FaultyValue<bool> out5 = XOR.evaluate({ i, i });
	FaultyValue<bool> out6 = XOR.evaluate({ i, d });
	FaultyValue<bool> out7 = XOR.evaluate({ i, u });
	FaultyValue<bool> out8 = XOR.evaluate({ i, x });
	FaultyValue<bool> out9 = XOR.evaluate({ d, d });
	FaultyValue<bool> out10 = XOR.evaluate({ d, u });
	FaultyValue<bool> out11 = XOR.evaluate({ d, x });
	FaultyValue<bool> out12 = XOR.evaluate({ u, u });
	FaultyValue<bool> out13 = XOR.evaluate({ u, x });
	FaultyValue<bool> out14 = XOR.evaluate({ x, x });

	O(out0);
	I(out1);
	D(out2);
	U(out3);
	X(out4);
	O(out5);
	U(out6);
	D(out7);
	X(out8);
	O(out9);
	I(out10);
	X(out11);
	O(out12);
	X(out13);
	X(out14);
}

class FaultyValueTest_Wide : public ::testing::Test {
public:
	void SetUp() override {

	}

	BooleanFunction<FaultyValue<unsigned long long int>> AND = BooleanFunction<FaultyValue<unsigned long long int>>("and");
	BooleanFunction<FaultyValue<unsigned long long int>> OR = BooleanFunction<FaultyValue<unsigned long long int>>("or");
	BooleanFunction<FaultyValue<unsigned long long int>> XOR = BooleanFunction<FaultyValue<unsigned long long int>>("xor");


	void TearDown() {

	}
};

#define WIDTH unsigned long long int
TEST_F(FaultyValueTest_Wide, WideTest) {
	//The inputs of this test are in the same order as the boolean tests:
	//         Value              MAGG VAL  FAUL (invert the mag)
	//Input 1: 000_0011_11DD_DUUX 03C6 7FFE 003E
	//Input 2: 01D_UX1D_UXDU_XUXX 2A94 7BB4 19B4
	//AND out: 000_001D_UXD0_XUXX 0284 7FB4 01A4
	//OR  out: 01D_UX11_11D1_XUXX 2BD6 7BF4 1824
	//XOR out: 01D_UX0U_DX01_X0XX 2910 7BB4 1980
	FaultyValue<WIDTH> input1 = FaultyValue<WIDTH>(
		Value<WIDTH>(0x03F8,       0x7FFE),
		Value<WIDTH>(0x03F8^0x003E, 0x7FFE)
		);
	FaultyValue<WIDTH> input2 = FaultyValue<WIDTH>(
		Value<WIDTH>(0x3320         , 0x7BB4), 
		Value<WIDTH>(0x3320 ^ 0x19B4, 0x7BB4)
		);
	WIDTH f1check = input1.faulty();
	WIDTH f2check = input2.faulty();
	if (f1check == 0x01 | f2check == 0x01)
	{
		printf("Hi\n");
	}
	FaultyValue<WIDTH> ANDout = AND.evaluate({ input1, input2 });
	FaultyValue<WIDTH> ORout = OR.evaluate({ input1, input2 });
	FaultyValue<WIDTH> XORout = XOR.evaluate({ input1, input2 });

	EXPECT_EQ(ANDout.magnitude() & ANDout.valid(), 0x0284 & 0x7FB4); EXPECT_EQ(ANDout.valid(), 0x7FB4); EXPECT_EQ(ANDout.faulty() & ANDout.valid(), 0x01A4);
	EXPECT_EQ(ORout.magnitude() & ORout.valid(), 0x2BD6 & 0x7BF4); EXPECT_EQ(ORout.valid(), 0x7BF4); EXPECT_EQ(ORout.faulty() & ORout.valid(), 0x1824);
	EXPECT_EQ(XORout.magnitude() & XORout.valid(), 0x2910 & 0x7BB4); EXPECT_EQ(XORout.valid(), 0x7BB4); EXPECT_EQ(XORout.faulty() & XORout.valid(), 0x1980);

}
