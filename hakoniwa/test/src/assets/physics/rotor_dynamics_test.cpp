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
using hako::assets::drone::DroneRotorSpeedType;

#define DELTA_TIME_SEC 0.001
TEST_F(RotorDynamicsTest, test_01)
{
    RotorDynamics rotor(DELTA_TIME_SEC);
    double rpm_max = 1000;
    rotor.set_params(rpm_max, 1.0, 1.0);
    DroneRotorSpeedType prev_value = rotor.get_rotor_speed();
    EXPECT_EQ(0, prev_value.data);

    DroneRotorSpeedType value;
    for (int i = 0; i < 1000; i++) {
        rotor.run(0.5);
        value = rotor.get_rotor_speed();
    }
    //std::cout << "1sec value: " << value.data << std::endl;
    EXPECT_GT(value.data, 300);
    for (int i = 0; i < 1000; i++) {
        rotor.run(0.5);
        value = rotor.get_rotor_speed();
    }
    //std::cout << "2sec value: " << value.data << std::endl;
    EXPECT_GT(value.data, 400);
}
