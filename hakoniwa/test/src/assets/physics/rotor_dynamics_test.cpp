#include <gtest/gtest.h>
#include <iostream>
#include "rotor_dynamics.hpp"

class RotorDynamicsTest : public ::testing::Test {
protected:
    static void SetUpTestCase()
    {
    }
    static void TearDownTestCase()
    {
    }
    virtual void SetUp()
    {
    }
    virtual void TearDown()
    {
    }

};
using hako::assets::drone::RotorDynamics;

#define DELTA_TIME_SEC 0.001
TEST_F(RotorDynamicsTest, test_01)
{
    RotorDynamics rotor(DELTA_TIME_SEC);

    DroneRotorSpeedType prev_value = rotor.get_rotor_speed();
    EXPECT_EQ(0, prev_value.data);

    DroneRotorSpeedType value;
    double expect_value = 0.5;
    for (int i = 0; i < 1000; i++) {
        rotor.run(expect_value);
        value = rotor.get_rotor_speed();
    }
    //std::cout << "1sec value: " << value.data << std::endl;
    EXPECT_GT(value.data, expect_value - 0.2);
    for (int i = 0; i < 1000; i++) {
        rotor.run(expect_value);
        value = rotor.get_rotor_speed();
    }
    //std::cout << "2sec value: " << value.data << std::endl;
    EXPECT_GT(value.data, expect_value - 0.1);
}
