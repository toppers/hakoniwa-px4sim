#include <gtest/gtest.h>
#include <iostream>
#include "utils/sensor_data_assembler.hpp"
#include "utils/sensor_noise.hpp"

class UtilsTest : public ::testing::Test {
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
using hako::assets::drone::SensorNoise;
using hako::assets::drone::SensorDataAssembler;

TEST_F(UtilsTest, NoiseStatisticsTest_001) 
{
    SensorNoise noise(0, 0.1); // 平均 0、分散 0.1 のノイズ
    const int samples = 100;
    double sum = 0;
    double sum_squares = 0;

    for (int i = 0; i < samples; ++i) {
        double noise_value = noise.add_noise(0); // テストデータとして 0 を使用
        sum += noise_value;
        sum_squares += noise_value * noise_value;
    }

    double mean = sum / samples;
    double variance = sum_squares / samples - mean * mean;

    // 平均と分散が期待値に近いかチェック
    EXPECT_NEAR(0, mean, 0.05); // 平均が 0 に近いことを確認
    EXPECT_NEAR(0.05, variance, 0.05); // 分散が 0.1 に近いことを確認
}
TEST_F(UtilsTest, SensorDataAssemblerTest_001)
{
    SensorDataAssembler obj(3);
    EXPECT_EQ(0, obj.get_calculated_value());
}
TEST_F(UtilsTest, SensorDataAssemblerTest_002)
{
    SensorDataAssembler obj(3);
    obj.add_data(1);
    EXPECT_EQ(1, obj.get_calculated_value());
}
TEST_F(UtilsTest, SensorDataAssemblerTest_003)
{
    SensorDataAssembler obj(3);
    obj.add_data(1);
    obj.add_data(2);
    EXPECT_EQ(1.5, obj.get_calculated_value());
}
TEST_F(UtilsTest, SensorDataAssemblerTest_004)
{
    SensorDataAssembler obj(3);
    obj.add_data(1);
    obj.add_data(2);
    obj.add_data(3);
    EXPECT_EQ(2, obj.get_calculated_value());
}
TEST_F(UtilsTest, SensorDataAssemblerTest_005)
{
    SensorDataAssembler obj(3);
    obj.add_data(1);
    obj.add_data(2);
    obj.add_data(3);
    obj.add_data(4);
    EXPECT_EQ(3, obj.get_calculated_value());
}