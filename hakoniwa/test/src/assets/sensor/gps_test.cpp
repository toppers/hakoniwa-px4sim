#include <gtest/gtest.h>
#include <iostream>
#include "sensors/gps/sensor_gps.hpp"
#include "utils/sensor_noise.hpp"

class GpsTest : public ::testing::Test {
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
using hako::assets::drone::SensorGps;
using hako::assets::drone::SensorNoise;
using hako::assets::drone::DronePositionType;
using hako::assets::drone::DroneVelocityType;
using hako::assets::drone::DroneGpsDataType;

TEST_F(GpsTest, SensorGps_001) 
{
    SensorGps gps(0.001, 3);
    double ref_lat = 35.6895;
    double ref_lon = 139.6917;
    double ref_alt = 10;
    gps.init_pos(ref_lat, ref_lon, ref_alt);

    DronePositionType pos;
    DroneVelocityType vel;

    pos.data.x = 1;
    pos.data.y = 1;
    pos.data.z = 1;

    vel.data.x = 1;
    vel.data.y = 1;
    vel.data.z = 1;

    gps.run(pos, vel);
    gps.run(pos, vel);
    gps.run(pos, vel);

    DroneGpsDataType result = gps.sensor_value();

    EXPECT_FLOAT_EQ(ref_lat + (1/111000), result.lat);
    EXPECT_FLOAT_EQ(ref_lon + (1/111000), result.lon);
    EXPECT_FLOAT_EQ(ref_alt + (-1), result.alt);

    EXPECT_FLOAT_EQ(sqrt(3), result.vel);
    EXPECT_FLOAT_EQ(1, result.vn);
    EXPECT_FLOAT_EQ(1, result.ve);
    EXPECT_FLOAT_EQ(1, result.vd);

    EXPECT_FLOAT_EQ(45, result.cog);

}


TEST_F(GpsTest, SensorGps_002) 
{
    SensorGps gps(0.001, 3);
    SensorNoise noise(0.01);
    double ref_lat = 35.6895;
    double ref_lon = 139.6917;
    double ref_alt = 10;
    gps.init_pos(ref_lat, ref_lon, ref_alt);

    DronePositionType pos;
    DroneVelocityType vel;

    pos.data.x = 1;
    pos.data.y = 1;
    pos.data.z = 1;

    vel.data.x = 1;
    vel.data.y = 1;
    vel.data.z = 1;

    gps.run(pos, vel);
    gps.run(pos, vel);
    gps.run(pos, vel);

    DroneGpsDataType result = gps.sensor_value();

    EXPECT_GT(result.lat, ref_lat + (1/111000) - 0.02);
    EXPECT_LT(result.lat, ref_lat + (1/111000) + 0.02);

    EXPECT_GT(result.lon, ref_lon + (1/111000) - 0.02);
    EXPECT_LT(result.lon, ref_lon + (1/111000) + 0.02);

    EXPECT_GT(result.alt, ref_alt - 1 - 0.02);
    EXPECT_LT(result.alt, ref_alt - 1 + 0.02);

    EXPECT_GT(result.vel, sqrt(3) - 0.02);
    EXPECT_LT(result.vel, sqrt(3) + 0.02);

    EXPECT_GT(result.vn, 1 - 0.02);
    EXPECT_LT(result.vn, 1 + 0.02);
    EXPECT_GT(result.ve, 1 - 0.02);
    EXPECT_LT(result.ve, 1 + 0.02);
    EXPECT_GT(result.vd, 1 - 0.02);
    EXPECT_LT(result.vd, 1 + 0.02);
    EXPECT_GT(result.cog, 45 - 0.02);
    EXPECT_LT(result.cog, 45 + 0.02);
}
