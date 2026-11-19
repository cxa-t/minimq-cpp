#include "contacts.pb.h"
#include <iostream>

int main() 
{
    // 定义对象
    contacts::contact conn;

    // 给对象附上属性
    conn.set_sn(1688);
    conn.set_name("csa");
    conn.set_score(98.5);

    // 序列化
    // 持久化数据放在str中 可以进行持久化或网络传输
    std::string str = conn.SerializeAsString();
    
    // 反序列化
    contacts::contact stu;
    bool res = stu.ParseFromString(str);
    if(!res)
    {
        std::cout << "ParseFromString false" << std::endl;
        return -1;
    }
    std::cout << stu.sn() << std::endl;
    std::cout << stu.score() << std::endl;
    std::cout << stu.name() << std::endl;
    return 0;
}