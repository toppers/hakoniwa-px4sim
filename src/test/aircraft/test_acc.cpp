#include <gtest/gtest.h>
#include <iostream>
#include "aircraft/impl/sensors/sensor_acceleration.hpp"
#include "aircraft/impl/noise/sensor_noise.hpp"

class AccTest : public ::testing::Test {
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
using hako::aircraft::SensorAcceleration;
using hako::aircraft::SensorNoise;
using hako::aircraft::DroneVelocityBodyFrameType;
using hako::aircraft::DroneAccelerationBodyFrameType;

TEST_F(AccTest, SensorAcceleration_001) 
{
    SensorAcceleration acc(0.001, 3);
    DroneVelocityBodyFrameType value;
    value.data.x = 1;
    value.data.y = 2;
    value.data.z = 3;
    acc.run(value);
    value.data.x = 2;
    value.data.y = 3;
    value.data.z = 4;
    acc.run(value);
    value.data.x = 3;
    value.data.y = 4;
    value.data.z = 5;
    acc.run(value);

    DroneAccelerationBodyFrameType result = acc.sensor_value();

    EXPECT_EQ(1000, result.data.x);
    EXPECT_EQ(1000, result.data.y);
    EXPECT_EQ(1000-9.81, result.data.z);
}

TEST_F(AccTest, SensorAcceleration_002) 
{
    SensorAcceleration acc(0.001, 3);
    SensorNoise noise(10);
    acc.set_noise(&noise);
    DroneVelocityBodyFrameType value;
    value.data.x = 1;
    value.data.y = 2;
    value.data.z = 3;
    acc.run(value);
    value.data.x = 2;
    value.data.y = 3;
    value.data.z = 4;
    acc.run(value);
    value.data.x = 3;
    value.data.y = 4;
    value.data.z = 5;
    acc.run(value);

    DroneAccelerationBodyFrameType result = acc.sensor_value();

    EXPECT_GT(result.data.x, 900);
    EXPECT_LT(result.data.x, 1100);

    EXPECT_GT(result.data.y, 900);
    EXPECT_LT(result.data.y, 1100);

    EXPECT_GT(result.data.z, 900);
    EXPECT_LT(result.data.z, 1100);

}
