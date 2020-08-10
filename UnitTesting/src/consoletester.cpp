#include <gmock/gmock.h>
#include <iostream>

TEST(ConsoleTest, Boh){
    EXPECT_TRUE(false) << "sdgsdgg" << std::endl;
}