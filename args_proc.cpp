#include "args_proc.h"
#include <iostream>

using namespace args_extra;
using std::string;


posix::posix(const std::string &name, const std::string &detial)
    :cmd_name(name),cmd_detial(detial)
{}

void posix::reset_description(const std::string &desc){
    this->cmd_description = desc;
}

void posix::set_opts_short(const std::initializer_list<ArgsGroup> &list)
{
    for (auto itor=list.begin();itor!=list.end();++itor) {
        auto item = *itor;

        for (auto ext:opts_table) {
            if(ext.opt == item.opt){
                auto r = new args_ex("选项开关设计错误");
                r->set_detial("开关设计与已知重复。opt<"+ext.opt+">");
                throw r;
            }
        }

        opts_table.push_back(item);
    }
}

void posix::args_accept(int argc, char *argv[])
{
    for (int i=0;i<argc;++i) {
        string arg = argv[i];

        /*
         * 检测此参数是否为开关
         */
        if(arg.find("-")==0 && arg.size()>1) {
            bool need_suply = false;

            /*
             * 逐个检测开关参数合法性
             */
            for(unsigned long i=1; i<arg.size(); i++){
                char cc = arg[i];

                auto itor=opts_table.begin();
                for (;itor!=opts_table.end();itor++) {
                    if((*itor).opt.find(cc)==0)
                        break;
                }
                if (itor == opts_table.end()) {
                    auto r = new args_ex("参数输入错误");
                    r->set_detial(string("输入参数命令开关错误，出现未定义开关。opt<").append(&cc) +">");
                    throw r;
                }
            }
        }

        args_list.push_back(arg);
    }
}

std::list<std::string> posix::else_args() {
    std::list<std::string> rlist;

    for (auto itor=args_list.begin();itor!=args_list.end();++itor) {
        if((*itor).find("-")==0){
            ++itor;
            continue;
        }

        rlist.push_back(*itor);
    }

    return rlist;
}






























args_ex::args_ex(const std::string &type)
    :indicator(type){}

std::string args_ex::type(){
    return indicator;
}

void args_ex::set_detial(const std::string &reason){
    this->detial = reason;
}

std::string args_ex::reason(){
    return detial;
}
