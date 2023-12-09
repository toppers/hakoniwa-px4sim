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

#define REFERENCE_LATITUDE      47.641468
#define REFERENCE_LONGTITUDE    -122.140165
#define REFERENCE_ALTITUDE      121.321

#define DEGREE2RADIAN(v)    ( (v) * M_PI / (180.0) )
#define RADIAN2DEGREE(v)    ( (180.0 * (v)) / M_PI )
#define PARAMS_MAG_F   53045.1
#define PARAMS_MAG_D    DEGREE2RADIAN(15.306)
#define PARAMS_MAG_I    DEGREE2RADIAN(68.984)

/*
 * Total (F)    : 53045.1  nT 
 * Northward (X): 18348.5  nT
 * Eastward (Y) :  5021.6  nT
 * Downward (Z) : 49516.6  nT
 */

TEST_F(MagTest, SensorMag_001) 
{
    SensorMag mag(0.001, 3);
    mag.set_params(PARAMS_MAG_F, PARAMS_MAG_I, PARAMS_MAG_D);

    DroneAngleType angle;
    angle.data.x = DEGREE2RADIAN(0);
    angle.data.y = -PARAMS_MAG_I;
    angle.data.z = PARAMS_MAG_D;

    mag.run(angle);


    //smag.print();
    auto result = mag.sensor_value();

    EXPECT_FLOAT_EQ(PARAMS_MAG_F, result.data.x);
    EXPECT_FLOAT_EQ(0, result.data.y);
    EXPECT_FLOAT_EQ(0, result.data.z);
}
