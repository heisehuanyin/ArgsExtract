#include <iostream>

#include "args_parse.h"

using namespace std;

int main(int argc, char* argv[])
{
    args_extra::gnu utool("extra","提取命令行参数测试程序");

    utool.setShort_opts({
                            {"a","","处理所有文件"},
                            {"h","somefile","显示帮助信息"},
                            {"w","","结尾计算辅助"},
                            {"","somefile","adkfjlakdfj"},
                            {"","a-few-message","adkfjlakdfj"}
                        });
    utool.setLong_opts({
                           {"help","","显示帮助信息"},
                           {"vesion","","显示版本信息"},
                           {"pick","filename","测试pholder参数显示"},
                           {"pppp","aldkfjdslkfjskdfjlskf","akjflaskjflakjflkjflaksjflkjflkjajf"}
                       });

    args_extra::args_check_print(utool, argc, argv);

    auto g=utool.else_args();
    for (auto it=g.begin();it!=g.end();++it) {
        cout << *it << endl;
    }

    string k,v;
    k = utool.get_option({"pppp"},v);
    std::cout << k << ":" << v << std::endl;

    std::cout << utool.help_doc() << endl;

    return 0;
}
