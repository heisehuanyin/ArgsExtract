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
        if (item.opt.find("-")==0){
            auto r = new args_ex("选项参数设计错误");
            r->set_detial("选项设计错误，夹带“-”标识。option<"+item.opt+">");
            throw r;
        }

        for (auto ext:opts_table) {
            if(ext.opt == item.opt){
                auto r = new args_ex("选项参数设计错误");
                r->set_detial("选项设计与已知选项重复。option<"+ext.opt+">");
                throw r;
            }
        }

        opts_table.push_back(item);
    }
}

void posix::args_accept(int argc, char *argv[])
{
    for (int args_i=1; args_i<argc; ++args_i) {
        string item = argv[args_i];

        if (item.size()>1 && item.find("-")==0 && item.find("--")==string::npos){
            int suply_count = 0;

            for (unsigned long char_i=1; char_i < item.size(); ++char_i) {
                char single_switch = item[char_i];

                auto itor=opts_table.begin();
                for (; itor!=opts_table.end(); ++itor) {

                    if ((*itor).opt.find(single_switch)==0){
                        if ((*itor)._placeholder!="") {
                            suply_count++;
                        }

                        break;
                    }
                }
                if (itor == opts_table.end()){
                    auto r = new args_ex("输入参数错误");
                    r->set_detial(string("未知参数选项，option<").append(&single_switch) + ">");
                    throw r;
                }

                if (suply_count > 1){
                    auto e = new args_ex("输入参数错误");
                    e->set_detial("多个补参选项错误结合,option<"+item+">");
                    throw e;
                }else if (suply_count == 1) {
                    if (args_i < argc-1) {
                        string suply_item = argv[args_i+1];
                        if(suply_item.find("-")!=0){
                            args_pairs.push_back(std::make_pair(string().append(&single_switch), suply_item));
                            args_i++;
                            continue;
                        }
                    }
                    auto e = new args_ex("输入参数错误");
                    e->set_detial(string("选项缺少补充参数，option<").append(&single_switch)+">");
                    throw e;

                }else {
                    args_pairs.push_back(std::make_pair(string().append(&single_switch),""));
                }
            }
        }
        else {
            args_left.push_back(item);
        }
    }
}

std::list<std::string> posix::else_args() {
    return args_left;
}

std::string posix::help_string() {
    std::string help_doc = "Name:\t"+cmd_name + " -- " + cmd_detial + "\n";
    help_doc += "Usage:\t" + cmd_name + " [-";

    std::string temp = "";
    for (auto it=opts_table.begin(); it!=opts_table.end(); ++it) {
        if ((*it)._placeholder == "") {
            help_doc += (*it).opt;
        }
        else{
            temp += "[-" + (*it).opt + " " + (*it)._placeholder + " ] ";
        }
    }
    help_doc += "] " + temp;
    return help_doc;
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
