#include <iostream>

#include "args_proc.h"

using namespace std;

int main(int argc, char* argv[])
{
    args_extra::posix utool("extra","提取命令行参数测试程序");

    utool.set_opts_short({
                              {"a","","处理所有文件"},
                              {"h","","显示帮助信息"}
                          });
    ARGS_CHECK(utool);

    auto g=utool.else_args();
    for (auto it=g.begin();it!=g.end();++it) {
        cout << *it << endl;
    }

    return 0;
}
