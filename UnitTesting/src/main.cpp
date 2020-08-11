#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <iostream>
#include <limits>
#include <ios>
#include <chrono>
/*
docs:
https://github.com/google/googletest/blob/master/googletest/docs/primer.md
https://github.com/google/googletest/blob/master/googletest/docs/advanced.md
*/
std::chrono::system_clock::time_point getCurrentTime();
void getTime(const std::chrono::system_clock::time_point& time, char* output, const rsize_t size);


int main(int argc, char** argv){
    const rsize_t output_time_size = 100;
    char* first = new char[output_time_size];
    char* second = new char[output_time_size];

    const auto start = getCurrentTime();
    getTime(start, first, output_time_size);
    std::cout << "Testing for MLEngine started at: " << first << std::endl; 

    testing::InitGoogleMock(&argc, argv);
    const int result = RUN_ALL_TESTS();

    const auto end = getCurrentTime();
    getTime(end, second, output_time_size);
    std::cout << std::endl << "Testing for MLEngine finished at: " << second << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms." << std::endl << std::endl; 

    std::system("pause");

    return result;
}

void getTime(const std::chrono::system_clock::time_point& time, char* output, const rsize_t size){
    const auto time_transformed = std::chrono::system_clock::to_time_t(time);
    if(ctime_s(output, size, &time_transformed)){
        std::strcpy(output, "No valid time found!\0");
    }
}
std::chrono::system_clock::time_point getCurrentTime(){
    return std::chrono::system_clock::now();
}
