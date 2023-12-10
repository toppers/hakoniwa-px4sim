#include <gtest/gtest.h>
#include <iostream>
#include "thrust_dynamics.hpp"

class ThrustDynamicsTest : public ::testing::Test {
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
using hako::assets::drone::ThrustDynamics;
using hako::assets::drone::DroneRotorSpeedType;
using hako::assets::drone::DroneThrustType;
using hako::assets::drone::DroneTorqueType;
using hako::assets::drone::GRAVITY;

#define DELTA_TIME_SEC 0.001
#define ROTOR_SPEED_BASE    6000 //rpm

TEST_F(ThrustDynamicsTest, test_01)
{
    ThrustDynamics dynamics(DELTA_TIME_SEC);
    DroneRotorSpeedType rotor_speed[hako::assets::drone::ROTOR_NUM];

    //上昇
    {
        for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++)
        {
            rotor_speed[i].data = ROTOR_SPEED_BASE * 2;
        }
        dynamics.run(rotor_speed);
        DroneThrustType thrust = dynamics.get_thrust();
        DroneTorqueType torque = dynamics.get_torque();
        //dynamics.print();

        //ローター回転数の二乗に比例する
        EXPECT_FLOAT_EQ(GRAVITY * 4, thrust.data);

        EXPECT_FLOAT_EQ(0, torque.data.x);
        EXPECT_FLOAT_EQ(0, torque.data.y);
        EXPECT_FLOAT_EQ(0, torque.data.z);
    }

    //下降
    {
        for (int i = 0; i < hako::assets::drone::ROTOR_NUM; i++)
        {
            rotor_speed[i].data = 0;
        }
        dynamics.run(rotor_speed);
        
        DroneThrustType thrust = dynamics.get_thrust();
        DroneTorqueType torque = dynamics.get_torque();
        EXPECT_FLOAT_EQ(0, thrust.data);

        EXPECT_FLOAT_EQ(0, torque.data.x);
        EXPECT_FLOAT_EQ(0, torque.data.y);
        EXPECT_FLOAT_EQ(0, torque.data.z);
    }
    
}
TEST_F(ThrustDynamicsTest, test_02)
{
    ThrustDynamics dynamics(DELTA_TIME_SEC);
    DroneRotorSpeedType rotor_speed[hako::assets::drone::ROTOR_NUM];
    //ヨー方向の回転：機体を上から見た時の時計回り
    {
        rotor_speed[0].data = 0; //CW
        rotor_speed[1].data = ROTOR_SPEED_BASE * 2; //CCW
        rotor_speed[2].data = 0; //CW
        rotor_speed[3].data = ROTOR_SPEED_BASE * 2; //CCW
        dynamics.run(rotor_speed);
        dynamics.run(rotor_speed);
        DroneThrustType thrust = dynamics.get_thrust();
        DroneTorqueType torque = dynamics.get_torque();
        dynamics.print();

        //ローター回転数の二乗に比例する
        EXPECT_FLOAT_EQ(GRAVITY * 2, thrust.data);

        EXPECT_FLOAT_EQ(0, torque.data.x);
        EXPECT_FLOAT_EQ(0, torque.data.y);
        EXPECT_FLOAT_EQ(2, torque.data.z);
    }

    //ヨー方向の回転：機体を上から見た時の反時計回り
    {
        rotor_speed[0].data = ROTOR_SPEED_BASE * 2; //CW
        rotor_speed[1].data = ROTOR_SPEED_BASE * 0; //CCW
        rotor_speed[2].data = ROTOR_SPEED_BASE * 2; //CW
        rotor_speed[3].data = ROTOR_SPEED_BASE * 0; //CCW
        dynamics.run(rotor_speed);
        dynamics.run(rotor_speed);
        DroneThrustType thrust = dynamics.get_thrust();
        DroneTorqueType torque = dynamics.get_torque();
        dynamics.print();

        //ローター回転数の二乗に比例する
        EXPECT_FLOAT_EQ(GRAVITY * 2, thrust.data);

        EXPECT_FLOAT_EQ(0, torque.data.x);
        EXPECT_FLOAT_EQ(0, torque.data.y);
        EXPECT_FLOAT_EQ(-2, torque.data.z);
    }

}

TEST_F(ThrustDynamicsTest, test_03)
{
    ThrustDynamics dynamics(DELTA_TIME_SEC);
    DroneRotorSpeedType rotor_speed[hako::assets::drone::ROTOR_NUM];
    //ロール方向の回転：時計回り
    {
        rotor_speed[0].data = ROTOR_SPEED_BASE * 0;
        rotor_speed[1].data = ROTOR_SPEED_BASE * 2;
        rotor_speed[2].data = ROTOR_SPEED_BASE * 0;
        rotor_speed[3].data = ROTOR_SPEED_BASE * 0;
        dynamics.run(rotor_speed);
        dynamics.run(rotor_speed);
        DroneThrustType thrust = dynamics.get_thrust();
        DroneTorqueType torque = dynamics.get_torque();
        dynamics.print();

        EXPECT_FLOAT_EQ(GRAVITY * 1, thrust.data);

        EXPECT_GT(torque.data.x, 0);
        EXPECT_FLOAT_EQ(0, torque.data.y);
        EXPECT_FLOAT_EQ(1, torque.data.z);   
    }
    //ロール方向の回転：反時計回り
    {
        rotor_speed[0].data = ROTOR_SPEED_BASE * 0;
        rotor_speed[1].data = ROTOR_SPEED_BASE * 0;
        rotor_speed[2].data = ROTOR_SPEED_BASE * 0;
        rotor_speed[3].data = ROTOR_SPEED_BASE * 2;
        dynamics.run(rotor_speed);
        dynamics.run(rotor_speed);
        DroneThrustType thrust = dynamics.get_thrust();
        DroneTorqueType torque = dynamics.get_torque();
        dynamics.print();

        EXPECT_FLOAT_EQ(GRAVITY * 1, thrust.data);

        EXPECT_LT(torque.data.x, 0);
        EXPECT_FLOAT_EQ(0, torque.data.y);
        EXPECT_FLOAT_EQ(1, torque.data.z);   
    }

}

TEST_F(ThrustDynamicsTest, test_04)
{
    ThrustDynamics dynamics(DELTA_TIME_SEC);
    DroneRotorSpeedType rotor_speed[hako::assets::drone::ROTOR_NUM];
    //ピッチ方向の回転：時計回り
    {
        rotor_speed[0].data = ROTOR_SPEED_BASE * 2;
        rotor_speed[1].data = ROTOR_SPEED_BASE * 0;
        rotor_speed[2].data = ROTOR_SPEED_BASE * 0;
        rotor_speed[3].data = ROTOR_SPEED_BASE * 0;
        dynamics.run(rotor_speed);
        dynamics.run(rotor_speed);
        DroneThrustType thrust = dynamics.get_thrust();
        DroneTorqueType torque = dynamics.get_torque();
        dynamics.print();

        EXPECT_FLOAT_EQ(GRAVITY * 1, thrust.data);

        EXPECT_FLOAT_EQ(0, torque.data.x);
        EXPECT_GT(torque.data.y, 0);
        EXPECT_FLOAT_EQ(-1, torque.data.z);   
    }

    //ピッチ方向の回転：反時計回り
    {
        rotor_speed[0].data = ROTOR_SPEED_BASE * 0;
        rotor_speed[1].data = ROTOR_SPEED_BASE * 0;
        rotor_speed[2].data = ROTOR_SPEED_BASE * 2;
        rotor_speed[3].data = ROTOR_SPEED_BASE * 0;
        dynamics.run(rotor_speed);
        dynamics.run(rotor_speed);
        DroneThrustType thrust = dynamics.get_thrust();
        DroneTorqueType torque = dynamics.get_torque();
        dynamics.print();

        EXPECT_FLOAT_EQ(GRAVITY * 1, thrust.data);

        EXPECT_FLOAT_EQ(0, torque.data.x);
        EXPECT_LT(torque.data.y, 0);
        EXPECT_FLOAT_EQ(-1, torque.data.z);   
    }
}
