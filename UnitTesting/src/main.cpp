#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <limits>
#include <ios>
#include <chrono>
void getCurrentTime(char* output, const rsize_t size);


int main(int argc, char** argv){
    const rsize_t output_time_size = 100;
    char* first = new char[output_time_size];
    char* second = new char[output_time_size];

    getCurrentTime(first, output_time_size);
    std::cout << "Testing for MLEngine started at: " << first << std::endl; 
    testing::InitGoogleMock(&argc, argv);
    const int result = RUN_ALL_TESTS();

    getCurrentTime(second, output_time_size);
    std::cout << std::endl << "Testing for MLEngine finished at: " << second << std::endl; 

    std::system("pause");

    return result;
}

void getCurrentTime(char* output, const rsize_t size){
    const auto current_time = std::chrono::system_clock::now();
    const auto current_time_transformed = std::chrono::system_clock::to_time_t(current_time);
    ctime_s(output, size, &current_time_transformed);
}
