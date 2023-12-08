#include <gtest/gtest.h>
#include <iostream>
#include "sensors/gyro/sensor_gyro.hpp"
#include "utils/sensor_noise.hpp"

class GyroTest : public ::testing::Test {
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
using hako::assets::drone::SensorGyro;
using hako::assets::drone::SensorNoise;

TEST_F(GyroTest, SensorGyro_001) 
{
    SensorGyro gyro(0.001, 3);
    DroneAngularVelocityBodyFrameType value;
    value.data.x = 1;
    value.data.y = 2;
    value.data.z = 3;
    gyro.run(value);
    value.data.x = 2;
    value.data.y = 3;
    value.data.z = 4;
    gyro.run(value);
    value.data.x = 3;
    value.data.y = 4;
    value.data.z = 5;
    gyro.run(value);

    DroneAngularVelocityBodyFrameType result = gyro.sensor_value();

    EXPECT_EQ(2000, result.data.x);
    EXPECT_EQ(3000, result.data.y);
    EXPECT_EQ(4000, result.data.z);
}

TEST_F(GyroTest, SensorGyro_002) 
{
    SensorGyro gyro(0.001, 3);
    SensorNoise noise(10);
    gyro.set_noise(&noise);
    DroneAngularVelocityBodyFrameType value;
    value.data.x = 1;
    value.data.y = 2;
    value.data.z = 3;
    gyro.run(value);
    value.data.x = 2;
    value.data.y = 3;
    value.data.z = 4;
    gyro.run(value);
    value.data.x = 3;
    value.data.y = 4;
    value.data.z = 5;
    gyro.run(value);

    DroneAngularVelocityBodyFrameType result = gyro.sensor_value();

    EXPECT_NEAR(980, result.data.x, 1020);
    EXPECT_NEAR(1980, result.data.y, 2020);
    EXPECT_NEAR(2980, result.data.z, 3020);
}
