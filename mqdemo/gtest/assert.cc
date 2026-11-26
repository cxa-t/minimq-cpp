#include <iostream>
#include <gtest/gtest.h>

/*
    断言宏的使用 
        ASSERT_  断言失败则退出
        EXPECT_  断言失败继续运行
    注意：
        断言宏，必须在单元测试宏函数中使用 TEST
*/

TEST(test, great_than)
{
    int age = 18;
    ASSERT_GT(20, age);
}
TEST(test, less_than)
{
    int age = 18;
    ASSERT_LT(17, age);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}