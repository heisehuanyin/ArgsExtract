#include <iostream>

#include "args_proc.h"

using namespace std;

int main(int argc, char* argv[])
{
    args_extra::posix utool("extra","提取命令行参数测试程序");

    utool.set_opts_short({
                            {"a","","处理所有文件"},
                            {"h","somefile","显示帮助信息"},
                            {"w","","结尾计算辅助"}
                          });
    ARGS_CHECK(utool);

    auto g=utool.else_args();
    for (auto it=g.begin();it!=g.end();++it) {
        cout << *it << endl;
    }

    cout << utool.help_string() << endl;

    return 0;
}
