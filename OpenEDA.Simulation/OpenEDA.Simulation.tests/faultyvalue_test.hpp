///**
// * @file Value_Test.cpp
// * @author Joshua Immanuel (jzi0005@tigermail.auburn.edu)
// * @version 0.1
// * @date 2019-01-09
// *
// * @copyright Copyright (c) 2018
// *
// */
//
//#include"gtest/gtest.h"
//#include "FaultyValue.h"
//
//class FaultyValueTest : public ::testing::Test {
//public:
//	void SetUp() override {
//
//	}
//	FaultyValue<bool> val0 = FaultyValue<bool>(false, true);
//	FaultyValue<bool> val1 = FaultyValue<bool>(true, true);
//	FaultyValue<bool> val0_iv = FaultyValue<bool>(false, false);
//	FaultyValue<bool> val1_iv = FaultyValue<bool>(true, false);
//	FaultyValue<bool> val;
//
//};
//
////FaultyValue();
////T magnitude() const;
////T min() const;
////T max() const;
//TEST_F(FaultyValueTest, ConstructorTest01) {
//
//	ASSERT_FALSE(val.valid());
//	EXPECT_EQ(std::numeric_limits<bool>::min(), val.min());
//	EXPECT_EQ(std::numeric_limits<bool>::max(), val.max());
//}
//
////	FaultyValue(T _magnitude, bool _valid = true, T _min = std::numeric_limits<T>::min(),T _max = std::numeric_limits<T>::max())
////T magnitude() const;
////T min() const;
////T max() const;
//TEST_F(FaultyValueTest, ConstructorTest02) {
//	ASSERT_TRUE(val0.valid());
//	EXPECT_EQ(false, val0.magnitude());
//	EXPECT_EQ(std::numeric_limits<bool>::min(), val.min());
//	EXPECT_EQ(std::numeric_limits<bool>::max(), val.max());
//}
//
////bool operator == (const FaultyValue<T>& _other) const;
//TEST_F(FaultyValueTest, EQTEST01) {
//
//	ASSERT_TRUE(val0.valid());
//	EXPECT_EQ(false, val0.magnitude());
//	EXPECT_TRUE(val1 == val1);
//}
//
////bool operator == (const FaultyValue<T>& _other) const;
//TEST_F(FaultyValueTest, EQTEST02) {
//	ASSERT_TRUE(val1.valid());
//	ASSERT_TRUE(val0.valid());
//	EXPECT_EQ(true, val1.magnitude());
//	EXPECT_EQ(false, val0.magnitude());
//	EXPECT_FALSE(val1 == val0);
//}
//
////bool operator == (const FaultyValue<T>& _other) const;
//TEST_F(FaultyValueTest, EQTEST03) {
//	ASSERT_FALSE(val0_iv.valid());
//	ASSERT_TRUE(val0.valid());
//	EXPECT_EQ(false, val0_iv.magnitude());
//	EXPECT_EQ(false, val0.magnitude());
//	EXPECT_FALSE(val0_iv == val0);
//}
//
////bool operator != (const FaultyValue<T>& _other) const;
//TEST_F(FaultyValueTest, NETEST01) {
//	ASSERT_TRUE(val0.valid());
//	EXPECT_EQ(false, val0.magnitude());
//	EXPECT_FALSE(val0 != val0);
//}
//
////bool operator != (const FaultyValue<T>& _other) const;
//TEST_F(FaultyValueTest, NETEST02) {
//	ASSERT_TRUE(val1.valid());
//	ASSERT_TRUE(val0.valid());
//	EXPECT_EQ(true, val1.magnitude());
//	EXPECT_EQ(false, val0.magnitude());
//	EXPECT_TRUE(val1 != val0);
//}
//
////bool operator != (const FaultyValue<T>& _other) const;
////DELETE incorrect (changed) behavior
////TEST_F(FaultyValueTest, NETEST03) {
////	ASSERT_FALSE(val0_iv.valid());
////	ASSERT_TRUE(val0.valid());
////	EXPECT_EQ(true, val1_iv.magnitude());
////	EXPECT_EQ(false, val0.magnitude());
////	EXPECT_TRUE(val0_iv != val0);
////}
//
////bool operator < (const FaultyValue<T>& _other) const;
//TEST_F(FaultyValueTest, LTTEST01) {
//	ASSERT_TRUE(val0.valid());
//	EXPECT_EQ(false, val0.magnitude());
//	EXPECT_FALSE(val0 < val0);
//}
//
////bool operator < (const FaultyValue<T>& _other) const;
//TEST_F(FaultyValueTest, LTTEST02) {
//	ASSERT_TRUE(val0.valid());
//	ASSERT_TRUE(val0.valid());
//	EXPECT_EQ(false, val0.magnitude());
//	EXPECT_EQ(true, val1.magnitude());
//	EXPECT_TRUE(val0 < val1);
//}
//
////DELETE incorrect (changed) behavior
////bool operator < (const FaultyValue<T>& _other) const;
////TEST_F(FaultyValueTest, LTTEST03) {
////	ASSERT_FALSE(val0_iv.valid());
////	ASSERT_TRUE(val1.valid());
////	EXPECT_EQ(false, val0.magnitude());
////	EXPECT_EQ(true, val1.magnitude());
////	EXPECT_FALSE(val0_iv < val1);
////}
//
////DELETE obsolete
//////static bool possiblyDifferent(const FaultyValue<T> _value1, const FaultyValue<T> _value2);
////TEST_F(FaultyValueTest, DIFFERENTTEST01) {
////	ASSERT_TRUE(val0.valid());
////	EXPECT_EQ(false, val0.magnitude());
////	EXPECT_FALSE(val0.possiblyDifferent(val0,val0));
////}
////
//////static bool possiblyDifferent(const FaultyValue<T> _value1, const FaultyValue<T> _value2);
////TEST_F(FaultyValueTest, DIFFERENTTEST02) {
////	//FaultyValue<bool> val1(false), val2(true);
////	ASSERT_TRUE(val0.valid());
////	ASSERT_TRUE(val1.valid());
////	EXPECT_EQ(false, val0.magnitude());
////	EXPECT_EQ(true, val1.magnitude());
////	EXPECT_TRUE(val0.possiblyDifferent(val0, val1));
////}
////
//////static bool possiblyDifferent(const FaultyValue<T> _value1, const FaultyValue<T> _value2);
////TEST_F(FaultyValueTest, DIFFERENTTEST03) {
////	//FaultyValue<bool> val1(false, false), val2(true);
////	ASSERT_FALSE(val0_iv.valid());
////	ASSERT_TRUE(val1.valid());
////	EXPECT_EQ(false, val0_iv.magnitude());
////	EXPECT_EQ(true, val1.magnitude());
////	EXPECT_TRUE(val1.possiblyDifferent(val0_iv, val1));
////}
//
////T magnitude(T _magnitude) ;
//TEST_F(FaultyValueTest, MagnitudeTEST01) {
//	EXPECT_EQ(true, val1.magnitude());
//	EXPECT_EQ(false, val0.magnitude());
//}
//
////T magnitude(T _magnitude) ;
//TEST_F(FaultyValueTest, MagnitudeTEST02) {
//	EXPECT_EQ(false, val0.magnitude());
//	val0.magnitude(true);
//	EXPECT_EQ(true, val0.magnitude());
//}
//
////T magnitude(T _magnitude) ;
//TEST_F(FaultyValueTest, MagnitudeTEST03) {
//	EXPECT_EQ(true, val1.magnitude());
//	val1.magnitude(false);
//	EXPECT_EQ(false, val1.magnitude());
//}
//
//
////bool valid(bool _valid);
//TEST_F(FaultyValueTest, VALIDTEST01) {
//	EXPECT_FALSE(val1_iv.valid());
//	val1_iv.valid(true);
//	EXPECT_TRUE(val1_iv.valid());
//}
//
////bool valid(bool _valid);
//TEST_F(FaultyValueTest, VALIDTEST02) {
//	EXPECT_TRUE(val0.valid());
//	val0.valid(false);
//	EXPECT_FALSE(val0.valid());
//}
//
////bool valid(bool _valid);
//TEST_F(FaultyValueTest, VALIDTEST03) {
//	EXPECT_FALSE(val1_iv.valid());
//	val1_iv.valid(false);
//	EXPECT_FALSE(val1_iv.valid());
//}
//
////bool increment();
//TEST_F(FaultyValueTest, INCREMENTTEST01) {
//	ASSERT_TRUE(val1.increment());
//	EXPECT_EQ(false, val1.magnitude());
//}
//
////bool increment();
//TEST_F(FaultyValueTest, TEST02) {
//	ASSERT_FALSE(val0.increment());
//	EXPECT_EQ(true, val0.magnitude());
//}
//
////unsigned long int cardinality() const;
//TEST_F(FaultyValueTest, CARDINALITYTEST) {
//	EXPECT_EQ(2, val0.cardinality());
//}
//class FaultyValuedTest : public ::testing::Test {
//public:
//	void SetUp() override {
//
//	}
//	FaultyValue<bool> val0 = new FaultyValue<bool>(0);
//	FaultyValue<bool> val1 = new FaultyValue<bool>(1);
//	FaultyValue<bool> val;
//	Valued<bool> valued;
//	Valued<bool> valueded = new Valued<bool>(val);
//	Valued<bool> valued1 = new Valued<bool>(val1);
//};
////Valued();
//TEST_F(FaultyValuedTest, CONTRUCTORTEST01) {
//
//	EXPECT_FALSE(valued.value().valid());
//}
//
////Valued(FaultyValue<T> _value);
//TEST_F(FaultyValuedTest, CONTRUCTORTEST02) {
//
//	EXPECT_TRUE(valued1.value().valid());
//}
//
////Valued(FaultyValue<T> _value);
//TEST_F(FaultyValuedTest, CONTRUCTORTEST03) {
//
//	EXPECT_TRUE(valued1.value().valid());
//	EXPECT_EQ(1, valued1.value().magnitude());
//
//}
