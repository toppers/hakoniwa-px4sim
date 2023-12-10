#include <gtest/gtest.h>
#include <iostream>
#include "sensors/mag/sensor_mag.hpp"
#include "utils/sensor_noise.hpp"

class MagTest : public ::testing::Test {
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
using hako::assets::drone::SensorMag;
using hako::assets::drone::SensorNoise;
using hako::assets::drone::DroneAngleType;

#define REFERENCE_LATITUDE      47.641468
#define REFERENCE_LONGTITUDE    -122.140165
#define REFERENCE_ALTITUDE      121.321

#define DEGREE2RADIAN(v)    ( (v) * M_PI / (180.0) )
#define RADIAN2DEGREE(v)    ( (180.0 * (v)) / M_PI )
#define PARAMS_MAG_F   53045.1
#define PARAMS_MAG_H   19023.3
#define PARAMS_MAG_Z   49516.6
#define PARAMS_MAG_D    DEGREE2RADIAN(15.306)
#define PARAMS_MAG_I    DEGREE2RADIAN(68.984)

/*
 * Total (F)     : 53045.1  nT 
 * Northward (X) : 18348.5  nT
 * Eastward (Y)  :  5021.6  nT
 * Downward (Z)  : 49516.6  nT
 * Horizontal (H): 19023.3  nT
 */

TEST_F(MagTest, SensorMag_001) 
{
    SensorMag mag(0.001, 1);
    mag.set_params(PARAMS_MAG_F, PARAMS_MAG_I, PARAMS_MAG_D);
    DroneAngleType angle;
    // head => north
    {
        angle.data.x = DEGREE2RADIAN(0);
        angle.data.y = -PARAMS_MAG_I;
        angle.data.z = PARAMS_MAG_D;
        mag.run(angle);
        auto result = mag.sensor_value();
        EXPECT_FLOAT_EQ(PARAMS_MAG_F, result.data.x);
        EXPECT_FLOAT_EQ(0, result.data.y);
        EXPECT_FLOAT_EQ(0, result.data.z);
    }

    // head => east
    {
        angle.data.x = DEGREE2RADIAN(0);
        angle.data.y = -PARAMS_MAG_I;
        angle.data.z = PARAMS_MAG_D + DEGREE2RADIAN(90);
        mag.run(angle);
        auto result = mag.sensor_value();
        //mag.print();
        EXPECT_NEAR(0, result.data.x, 1e-6);
        EXPECT_FLOAT_EQ(-PARAMS_MAG_F, result.data.y);
        EXPECT_FLOAT_EQ(0, result.data.z);
    }
    // head => west
    {
        angle.data.x = DEGREE2RADIAN(0);
        angle.data.y = -PARAMS_MAG_I;
        angle.data.z = PARAMS_MAG_D + DEGREE2RADIAN(-90);
        mag.run(angle);
        auto result = mag.sensor_value();
        //mag.print();
        EXPECT_NEAR(0, result.data.x, 1e-6);
        EXPECT_FLOAT_EQ(PARAMS_MAG_F, result.data.y);
        EXPECT_FLOAT_EQ(0, result.data.z);
    }
    // head => south
    {
        angle.data.x = DEGREE2RADIAN(0);
        angle.data.y = -PARAMS_MAG_I;
        angle.data.z = PARAMS_MAG_D + DEGREE2RADIAN(180);
        mag.run(angle);
        auto result = mag.sensor_value();
        //mag.print();
        EXPECT_FLOAT_EQ(-PARAMS_MAG_F, result.data.x);
        EXPECT_NEAR(0, result.data.y, 1e-6);
        EXPECT_FLOAT_EQ(0, result.data.z);
    }
}

TEST_F(MagTest, SensorMag_002) 
{
    SensorMag mag(0.001, 1);
    mag.set_params(PARAMS_MAG_F, PARAMS_MAG_I, PARAMS_MAG_D);
    DroneAngleType angle;
    // head => horizontal
    // head => north
    {
        angle.data.x = DEGREE2RADIAN(0);
        angle.data.y = 0;
        angle.data.z = PARAMS_MAG_D;
        mag.run(angle);
        //mag.print();
        auto result = mag.sensor_value();
        EXPECT_NEAR(PARAMS_MAG_H, result.data.x, PARAMS_MAG_H + 0.2);
        EXPECT_FLOAT_EQ(0, result.data.y);
        EXPECT_GT(result.data.z, -PARAMS_MAG_Z-0.1);
        EXPECT_LT(result.data.z, -PARAMS_MAG_Z+0.1);
    }
    // head => east
    {
        angle.data.x = DEGREE2RADIAN(0);
        angle.data.y = 0;
        angle.data.z = PARAMS_MAG_D + DEGREE2RADIAN(90);
        mag.run(angle);
        auto result = mag.sensor_value();
        //mag.print();
        EXPECT_NEAR(0, result.data.x, 1e-6);
        EXPECT_GT(result.data.y, -PARAMS_MAG_H-0.2);
        EXPECT_LT(result.data.y, -PARAMS_MAG_H+0.2);
        EXPECT_GT(result.data.z, -PARAMS_MAG_Z-0.1);
        EXPECT_LT(result.data.z, -PARAMS_MAG_Z+0.1);
    }
    // head => west
    {
        angle.data.x = DEGREE2RADIAN(0);
        angle.data.y = 0;
        angle.data.z = PARAMS_MAG_D + DEGREE2RADIAN(-90);
        mag.run(angle);
        auto result = mag.sensor_value();
        //mag.print();
        EXPECT_NEAR(0, result.data.x, 1e-6);
        EXPECT_GT(result.data.y, PARAMS_MAG_H-0.2);
        EXPECT_LT(result.data.y, PARAMS_MAG_H+0.2);
        EXPECT_GT(result.data.z, -PARAMS_MAG_Z-0.1);
        EXPECT_LT(result.data.z, -PARAMS_MAG_Z+0.1);
    }
    // head => south
    {
        angle.data.x = DEGREE2RADIAN(0);
        angle.data.y = 0;
        angle.data.z = PARAMS_MAG_D + DEGREE2RADIAN(180);
        mag.run(angle);
        auto result = mag.sensor_value();
        //mag.print();
        EXPECT_GT(result.data.x, -PARAMS_MAG_H-0.2);
        EXPECT_LT(result.data.x, -PARAMS_MAG_H+0.2);
        EXPECT_NEAR(0, result.data.y, 1e-6);
        EXPECT_GT(result.data.z, -PARAMS_MAG_Z-0.1);
        EXPECT_LT(result.data.z, -PARAMS_MAG_Z+0.1);
    }

}
