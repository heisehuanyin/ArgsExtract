#include "args_parse.h"
#include <iostream>
#include <regex>

using namespace std;
using namespace args_extra;


ex_base::ex_base(const string &type)
    :_type(type){}

const string ex_base::type(){
    return _type;
}

ex_base &ex_base::setDescription(const string &reason){
    this->_description = reason;
    return *this;
}

const string ex_base::description(){
    return _description;
}

ex_inputs::ex_inputs(const string &reason)
    :ex_base ("输入参数错误"){
    setDescription(reason);
}

ex_design::ex_design(const string &reason)
    :ex_base ("选项设计错误"){
    setDescription(reason);
}

posix::posix(const string &name, const string &shortDesc)
    :cmd_name(name),cmd_detial(shortDesc){}

void posix::resetDescription(const string &desc){
    this->cmd_description = desc;
}

void posix::setShortOptions(const initializer_list<ArgsPack> &list)
{
    regex args_pattern("^(-+).*");
    smatch result;

    for (auto itor=list.begin();itor!=list.end();++itor) {
        if(regex_match(itor->opt, result, args_pattern)){
            throw new ex_design("选项格式设计错误，不应以“-”标识起始,option<"+itor->opt+">.");
        }

        if(itor->opt.size() > 1){
            throw new ex_design("选项设计错误，短选项只能为一位字母或数字。option<"+itor->opt+">");
        }

        for (auto ext:short_options) {
            if(itor->opt != "" && ext.opt == itor->opt){
                throw new ex_design("选项设计与已知选项重复。option<"+ext.opt+">");
            }
        }

        short_options.push_back(*itor);
    }
}

void posix::argsParse(vector<pair<string, int> > &argv)
{
    unsigned long argc = argv.size();

    regex pattern("^(-+).*");
    smatch result;

    for (unsigned long args_index=1; args_index<argc; ++args_index) {
        auto args_item = argv[args_index];

        // 非option参数，压入额外堆栈
        if(!regex_match(args_item.first, result, pattern)){
            args_lift.push_back(argv[args_index]);
            continue;
        }

        if(result.str(1).length() > 1)
            throw new ex_inputs("输入posix选项格式错误，option<" + args_item.first + ">");

        int supply_count = 0;
        for (unsigned long char_i=1; char_i < args_item.first.size(); ++char_i) {
            auto opt_char = args_item.first[char_i];
            auto itor=short_options.cbegin();

            // 查找注册option
            for (; itor!=short_options.cend(); ++itor) {
                if(itor->opt == string(1, opt_char)){
                    if (itor->value_pl != "")
                        supply_count++;
                    break;
                }
            }

            // 未知option
            if (itor == short_options.cend())
                throw new ex_inputs(string("未知参数选项，option<").append(&opt_char) + ">");

            if (supply_count > 1)
                throw new ex_inputs("多个补参选项错误结合,option<"+args_item.first+">");

            if (supply_count == 1) {
                if (args_index < argc - 1) {
                    string suply_item = argv[args_index+1].first;

                    if(!regex_match(suply_item, result, pattern)){
                        auto u = *itor; u.value_pl = suply_item;
                        parser_result.push_back(make_pair(u, args_item));
                        args_index++;
                        continue;
                    }
                }

                throw new ex_inputs(string("选项缺少补充参数，option<").append(&opt_char)+">");
            }else {
                auto u = *itor; u.value_pl.clear();
                parser_result.push_back(make_pair(u, args_item));
            }
        }
    }
}

pair<string,int> posix::peekOption(const initializer_list<string> &opts, string &argument) const
{
    list<pair<int, decltype((*parser_result.cbegin()))>> args_marks;

    for (auto input_itor=opts.begin();
         input_itor!=opts.end();
         input_itor++) {
        auto calc_index = 0;

        for (auto args_itor=parser_result.cbegin();
             args_itor!=parser_result.cend();
             ++args_itor,++calc_index) {

            if(args_itor->first.opt == *input_itor){
                args_marks.push_back(make_pair(calc_index, *args_itor));
            }
        }
    }

    if (args_marks.size() == 0)
        return make_pair("",-1);

    pair<ArgsPack,pair<string,int>> at;
    int num_index = -1;
    for (auto it3=args_marks.begin();  it3!=args_marks.end();  ++it3) {
        auto unit = *it3;

        if (unit.first > num_index || num_index == -1){
            num_index = unit.first;
            at = unit.second;
        }
    }

    argument = at.first.value_pl;
    return make_pair(at.first.opt, at.second.second);
}

const list<pair<string,int>>& posix::elseArguments() const {
    return args_lift;
}

const string posix::helpString()
{
    string doc_string="NAME:\n";
    doc_string += "\t" + cmd_name + " -- " + cmd_detial + "\n\n";


    doc_string += "USAGE:\n";
    doc_string += "\t" + cmd_name + " ";
    string temp1 = "[-", temp2 = "";
    {
        for (auto itor1=short_options.begin();
             itor1!=short_options.end();
             itor1++) {
            if ((*itor1).opt!="" && (*itor1).value_pl=="") {
                temp1 += (*itor1).opt;
            }
            if ((*itor1).opt!="" && (*itor1).value_pl!="") {
                temp2 += "[-" + (*itor1).opt + " " + (*itor1).value_pl + "]";
            }
            if ((*itor1).opt=="" && (*itor1).value_pl!="") {
                temp2 += "[" + (*itor1).value_pl + "] ";
            }
        }
        temp1 += "] ";
    }
    doc_string += temp1 + temp2 + "\n\n";



    doc_string += "DESCRIPTION:\n";
    doc_string += "\t" + (cmd_description==""?"NONE.":cmd_description) + "\n\n";



    doc_string += "OPTIONS:\n";
    for (auto itor2=short_options.begin();
         itor2!=short_options.end();
         ++itor2) {
        auto item = *itor2;

        if (item.opt!="") {
            doc_string += "\t-" + item.opt + " " + item.value_pl + "\n"
                                                                   "\t\t" + item.cmt + "\n\n";
        }
        if (item.opt=="" && item.value_pl!="") {
            doc_string += "\t" + item.value_pl + "\n"
                                                 "\t\t" + item.cmt + "\n\n";
        }
    }

    return doc_string;
}

const string posix::getName() const
{
    return cmd_name;
}

const string posix::getShortDescription() const
{
    return cmd_detial;
}

const string posix::getDescription() const
{
    return cmd_description;
}

const list<ArgsPack> &posix::getShortOptions() const
{
    return short_options;
}


gnu::gnu(const string &name, const string &detail)
    :posix (name, detail){}

void gnu::setLongOptions(const initializer_list<ArgsPack> &list){
    regex args_pattern("^(-+).*");
    smatch result;

    for (auto it=list.begin(); it!=list.end(); ++it) {
        if(regex_match(it->opt, result, args_pattern)){
            throw new ex_design("选项格式设计错误，参数不应以“--”起始，option<"+it->opt+">");
        }

        if (it->opt.size() < 2) {
            throw new ex_design("选项参数长度应该大于1，option<"+it->opt+">");
        }

        for (auto itor2=long_options.begin();
             itor2!=long_options.end();
             ++itor2) {

            if (it->opt != "" && (*itor2).opt == it->opt) {
                throw new ex_design("选项参数与已知重复，option<"+it->opt+">");
            }
        }

        long_options.push_back(*it);
    }
}

const list<ArgsPack> &gnu::getLongOptions() const {
    return long_options;
}

void gnu::argsParse(vector<pair<string, int> > &argv)
{
    regex argPattern("^(-+).*");
    smatch result;
    vector<pair<string,int>> posix_stack;

    for (auto args_itor=argv.cbegin();
         args_itor != argv.cend();
         ++args_itor) {

        if(regex_match(args_itor->first, result, argPattern)){
            if(result.str(1).length() > 2)
                throw new ex_inputs("输入gnu选项格式错误<" + args_itor->first + ">.");

            // gnu参数处理
            if(result.str(1).length() == 2){
                auto opt_itor = long_options.cbegin();
                auto gnu_key = args_itor->first.substr(2);  // options选项

                // 参数格式为联合格式
                string combina_value = "";                  // 附加参数
                if (gnu_key.find("=") != string::npos) {
                    combina_value = gnu_key.substr(gnu_key.find("=") + 1);
                    gnu_key = gnu_key.substr(0, gnu_key.size() - combina_value.size() - 1);

                    if (combina_value.size() < 1) {
                        throw new ex_inputs("遗漏必要附加参数，option<"+gnu_key+">");
                    }
                }


                for (;opt_itor != long_options.cend(); ++opt_itor) {
                    if (gnu_key == opt_itor->opt) {
                        //不需要附加参数
                        if (opt_itor->value_pl == "") {
                            if (combina_value != "")
                                throw new ex_inputs("该选项不应携带参数，option<"+gnu_key+">");

                            auto unit = *opt_itor; unit.value_pl = "";
                            this->parser_result.push_back(make_pair(unit, *args_itor));
                        }
                        else {
                            if (combina_value != "") {
                                auto unit = *opt_itor; unit.value_pl = combina_value;
                                this->parser_result.push_back(make_pair(unit, *args_itor));
                            }
                            else {
                                auto value = args_itor + 1;

                                if(value == argv.cend() || regex_match(value->first, result, argPattern))
                                    throw new ex_inputs("选项未提供附加参数，option<" + args_itor->first + ">.");
                                else {
                                    auto unit = *opt_itor; unit.value_pl = value->first;
                                    this->parser_result.push_back(make_pair(unit, *args_itor));
                                    args_itor++;
                                }
                            }
                        }
                        break;
                    }
                }
                if(opt_itor == long_options.cend())
                    throw new ex_inputs("输入未知选项<" + args_itor->first + ">.");
            }
        }

        posix_stack.push_back(*args_itor);
    }

    posix::argsParse(posix_stack);
}

pair<string, int> gnu::peekOption(const initializer_list<string> &opts, string &argument) const
{
    list<pair<int, decltype (*parser_result.cbegin())>> index_list;

    for (auto input_opt=opts.begin(); input_opt!=opts.end(); ++input_opt) {
        int calc_index = 0;

        for (auto result_item = parser_result.cbegin();
             result_item != parser_result.cend();
             ++result_item, ++calc_index) {

            if ((*input_opt)==result_item->first.opt){
                index_list.push_back(make_pair(calc_index, *result_item));
            }
        }
    }

    int min_num = -1;
    pair<ArgsPack, pair<string,int>> item;

    for (auto it=index_list.cbegin();
         it != index_list.cend();
         ++it) {
        if(min_num == -1 || min_num < it->first){
            min_num = it->first;
            item = it->second;
        }
    }

    auto preuslt = posix::peekOption(opts, argument);

    if(preuslt.second > item.second.second)
        return preuslt;

    argument = item.first.value_pl;
    return make_pair(item.first.opt, item.second.second);
}

const string gnu::helpString(){
    string doc="NAME:\n";
    doc += "\t"+getName() + " -- " + getShortDescription() + "\n\n";


    doc += "USAGE:\n";
    doc += "\t" + getName() + " ";
    string temp1="[-", temp2="", options_details="";

    auto short_opts = getShortOptions();
    for (auto itor=short_opts.begin(); itor!=short_opts.end(); ++itor) {
        if(itor->opt == "")
            continue;

        if (itor->value_pl==""){
            temp1 += itor->opt;
            options_details += "\t-" + itor->opt + "\n" +
                               "\t\t" + itor->cmt + "\n\n";
            continue;
        }

        temp2 += "[-" + itor->opt + " " + itor->value_pl + "] ";
        options_details += "\t-" + itor->opt + " " + itor->value_pl +"\n" +
                           "\t\t" + itor->cmt + "\n\n";

    }
    temp1 += "] ";

    if(short_opts.size()==0){
        temp1 = "";
    }

    for (auto itor=long_options.begin(); itor!=long_options.end(); ++itor) {
        if (itor->value_pl==""){
            temp2 += "[--"+itor->opt + "] ";
            options_details += "\t--" + itor->opt + "\n" +
                               "\t\t" + itor->cmt + "\n\n";
        }
        else{
            temp2 += "[--"+itor->opt + " " + itor->value_pl + "] ";
            options_details += "\t--" + itor->opt + " " + itor->value_pl + "\n" +
                               "\t\t" + itor->cmt + "\n\n";
        }
    }

    for(auto itor=short_opts.cbegin(); itor != short_opts.cend(); ++itor){
        if (itor->opt==""){
            temp2 += "[" + itor->value_pl + "] ";
            options_details += "\t" + itor->value_pl + "\n" +
                               "\t\t" + itor->cmt + "\n\n";
        }
    }

    doc += temp1 + temp2 + "\n\n";


    doc += "DESCRIPTION:\n";
    doc += "\t" + (getDescription()==""?"NONE.":getDescription()) + "\n\n";


    doc += "OPTIONS:\n";
    doc += options_details;

    return doc;
}







void args_extra::args_check_print(posix &tool, int argc, char *argv[]){
    vector<pair<string,int>> args;
    for (int var=0; var<argc; var++) {
        args.push_back(make_pair(argv[var], var));
    }

    try {
        tool.argsParse(args);
    } catch (ex_base* e) {
        cout<< e->type() << "::" << e->description() << endl;
        exit(0);
    }
}


























