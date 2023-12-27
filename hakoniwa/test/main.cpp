#include <stdio.h>
#include <gtest/gtest.h>
#include "utils/csv_logger.hpp"
uint64_t CsvLogger::time_usec = 0; 

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();
    return result;
}
