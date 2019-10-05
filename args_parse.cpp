#include "args_parse.h"
#include <iostream>

using namespace args_extra;
using std::string;


ex_base::ex_base(const string &type)
    :_type(type){}

const std::string &ex_base::type(){
    return _type;
}

void ex_base::setDescription(const string &reason){
    this->_description = reason;
}

const std::string &ex_base::description(){
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
    :cmd_name(name),cmd_detial(shortDesc)
{}

void posix::resetDescription(const string &desc){
    this->cmd_description = desc;
}

void posix::setShortOptions(const std::initializer_list<ArgsPack> &list)
{
    for (auto itor=list.begin();itor!=list.end();++itor) {
        auto item = *itor;
        if (item.opt.find("-")==0){
            throw new ex_design("选项设计错误，夹带“-”标识。option<"+item.opt+">");
        }

        for (auto ext:short_opts_table) {
            if(item.opt!="" && ext.opt == item.opt){
                throw new ex_design("选项设计与已知选项重复。option<"+ext.opt+">");
            }
        }

        short_opts_table.push_back(item);
    }
}

void posix::argsParse(std::vector<std::string> &argv)
{
    unsigned long argc = argv.size();
    for (unsigned long args_i=1; args_i<argc; ++args_i) {
        string item = argv[args_i];

        if (item.find("--")==0) {
            throw new ex_inputs("输入posix选项格式错误，option<"+item+">");
        }


        if (item.size()>1 && item.find("-")==0){
            int suply_count = 0;

            for (unsigned long char_i=1; char_i < item.size(); ++char_i) {
                char single_opt = item[char_i];

                auto itor=short_opts_table.begin();
                for (; itor!=short_opts_table.end(); ++itor) {
                    if ((*itor).opt.find(single_opt)==0){
                        if ((*itor).placeholder!="") {
                            suply_count++;
                        }

                        break;
                    }
                }
                if (itor == short_opts_table.end()){
                    throw new ex_inputs(string("未知参数选项，option<").append(&single_opt) + ">");
                }

                if (suply_count > 1){
                    throw new ex_inputs("多个补参选项错误结合,option<"+item+">");
                }else if (suply_count == 1) {
                    if (args_i < argc-1) {
                        string suply_item = argv[args_i+1];
                        if(suply_item.find("-")!=0){
                            parse_pairs.push_back(std::make_pair(string(&single_opt,1), suply_item));
                            args_i++;
                            continue;
                        }
                    }

                    throw new ex_inputs(string("选项缺少补充参数，option<").append(&single_opt)+">");

                }else {
                    parse_pairs.push_back(std::make_pair(string(&single_opt,1),""));
                }
            }
        }
        else {
            args_left.push_back(item);
        }
    }
}

const std::string posix::peekOption(const std::initializer_list<string> &opts, string &argument) const
{
    std::list<std::pair<int,std::pair<string,string>>> args_marks;

    for (auto opts_itor=opts.begin(); opts_itor!=opts.end(); opts_itor++) {
        auto item = *opts_itor;
        auto calc_index=0;

        for (auto args_itor=parse_pairs.begin(); args_itor!=parse_pairs.end(); ++args_itor,calc_index++) {
            if ((*args_itor).first==item) {
                args_marks.push_back(std::make_pair(calc_index,std::make_pair((*args_itor).first, (*args_itor).second)));
            }
        }
    }

    if (args_marks.size() == 0)
        return "";

    std::pair<string,string> at;
    int num_index = -1;
    for (auto it3=args_marks.begin();  it3!=args_marks.end();  ++it3) {
        auto unit = *it3;

        if (unit.first < num_index || num_index==-1){
            num_index = unit.first;
            at = unit.second;
        }
    }

    argument = at.second;
    return at.first;
}

const std::list<string>& posix::elseArguments() const {
    return args_left;
}

const std::string posix::helpString()
{
    string doc_string="NAME:\n";
    doc_string += "\t" + cmd_name + " -- " + cmd_detial + "\n\n";


    doc_string += "USAGE:\n";
    doc_string += "\t" + cmd_name + " ";
    string temp1 = "[-", temp2 = "";
    {
        for (auto itor1=short_opts_table.begin();
             itor1!=short_opts_table.end();
             itor1++) {
            if ((*itor1).opt!="" && (*itor1).placeholder=="") {
                temp1 += (*itor1).opt;
            }
            if ((*itor1).opt!="" && (*itor1).placeholder!="") {
                temp2 += "[-" + (*itor1).opt + " " + (*itor1).placeholder + "]";
            }
            if ((*itor1).opt=="" && (*itor1).placeholder!="") {
                temp2 += "[" + (*itor1).placeholder + "] ";
            }
        }
        temp1 += "] ";
    }
    doc_string += temp1 + temp2 + "\n\n";



    doc_string += "DESCRIPTION:\n";
    doc_string += "\t" + (cmd_description==""?"NONE.":cmd_description) + "\n\n";



    doc_string += "OPTIONS:\n";
    for (auto itor2=short_opts_table.begin();
         itor2!=short_opts_table.end();
         ++itor2) {
        auto item = *itor2;

        if (item.opt!="") {
            doc_string += "\t-" + item.opt + " " + item.placeholder + "\n"
                                                                      "\t\t" + item.cmt + "\n\n";
        }
        if (item.opt=="" && item.placeholder!="") {
            doc_string += "\t" + item.placeholder + "\n"
                                                    "\t\t" + item.cmt + "\n\n";
        }
    }

    return doc_string;
}

const std::string &posix::getName() const
{
    return cmd_name;
}

const std::string &posix::getShortDescription() const
{
    return cmd_detial;
}

const string& posix::getDescription() const
{
    return cmd_description;
}

const std::list<ArgsPack> &posix::getShortOptions() const
{
    return short_opts_table;
}


gnu::gnu(const string &name, const string &detail)
    :posix (name, detail){}

void gnu::setLongOptions(const std::initializer_list<ArgsPack> &list){
    for (auto it=list.begin(); it!=list.end(); ++it) {
        auto item = *it;
        if(item.opt.find("--")==0) {
            throw new ex_design("选项参数自带--，option<"+item.opt+">");
        }

        if (item.opt.size() <=3) {
            throw new ex_design("选项参数长度应该大于1，option<"+item.opt+">");
        }

        for (auto itor2=long_opts_table.begin(); itor2!=long_opts_table.end(); ++itor2) {
            if (item.opt!="" && (*itor2).opt==item.opt) {
                throw new ex_design("选项参数与已知重复，option<"+item.opt+">");
            }
        }

        long_opts_table.push_back(item);
    }
}

const std::list<ArgsPack> &gnu::getLongOptions() const {
    return long_opts_table;
}

void gnu::argsParse(std::vector<string>& argv)
{
    for (auto item : argv) {
        original_args.push_back(item);
    }

    std::list<string> to_be_remove;
    for (auto args_itor=argv.begin(); args_itor!=argv.end(); args_itor++) {
        if ((*args_itor).find("--")==0) {
            if((*args_itor).size()<=3){
                throw new ex_inputs("选项长度过短，option<"+*args_itor+">");
            }
            to_be_remove.push_back(*args_itor);

            auto one = args_itor->substr(2);// options选项
            // 参数格式为参数联合格式
            string temp="";                 // 附加参数
            if (one.find("=")!=string::npos) {
                temp = one.substr(one.find("=")+1);
                one = one.substr(0, one.size()-temp.size()-1);

                if (temp.size()<1) {
                    throw new ex_inputs("遗漏必要附加参数，option<"+one+">");
                }
            }

            // gnu选项参数解析
            auto opts_itor=long_opts_table.begin();
            for (;opts_itor!=long_opts_table.end(); ++opts_itor) {
                if (one==opts_itor->opt) {
                    //是否需要附加参数
                    if (opts_itor->placeholder=="") {
                        if (temp!=""){
                            throw new ex_inputs("该选项不应携带参数，option<"+one+">");
                        }

                        this->parse_pairs.push_back(std::make_pair(one,""));
                    }
                    else {
                        if (temp!="") {
                            this->parse_pairs.push_back(std::make_pair(one,temp));
                        }
                        else {
                            auto value = args_itor+1;
                            if (value->find("-")==0 || value==argv.end()){
                                throw new ex_inputs("选项未提供附加参数，option<"+*args_itor+">");
                            }
                            else {
                                this->parse_pairs.push_back(std::make_pair(one,*value));
                                to_be_remove.push_back(*value);
                                args_itor++;
                            }
                        }
                    }

                    break;
                }
            }

            if (opts_itor == long_opts_table.end()) {
                throw new ex_inputs("未知命令选项，option<"+one+">");
            }
        }
    }

    for (auto itor=to_be_remove.begin(); itor!=to_be_remove.end(); ++itor) {
        for (auto args_it=argv.begin(); args_it!=argv.end(); ++args_it) {
            if ((*args_it) == *itor){
                argv.erase(args_it);
                break;
            }
        }
    }

    posix::argsParse(argv);
}

const std::string gnu::peekOption(const std::initializer_list<string> &opts, string &value) const{
    std::list<std::pair<int,string>> indices;

    for (auto opts_itor=opts.begin(); opts_itor!=opts.end(); ++opts_itor) {
        auto long_opt = long_opts_table.begin();
        for (;long_opt != long_opts_table.end(); ++long_opt) {
            if ((*opts_itor)==long_opt->opt)
                break;
        }

        if (long_opt!=long_opts_table.end()){
            int args_index=0;
            auto args_itor = original_args.begin();

            for (; args_itor!=original_args.end(); ++args_itor, ++args_index) {
                if (args_itor->find("--"+*opts_itor)==0){
                    indices.push_back(std::make_pair(args_index, long_opt->opt));
                    break;
                }
            }
            continue;
        }


        string value="",key = posix::peekOption({*opts_itor}, value);
        if (key == *opts_itor) {
            int args_index=0;
            auto args_itor = original_args.begin();

            for (; args_itor!=original_args.end(); ++args_itor, ++args_index) {
                if (args_itor->find("-")==0 && args_itor->find("--")!=0 && args_itor->find(key)!=0) {
                    indices.push_back(std::make_pair(args_index, key));
                    break;
                }
            }
            continue;
        }

    }

    auto index = -1;
    string key = "";
    for (auto itor=indices.begin(); itor!=indices.end(); ++itor) {
        if (index==-1 || itor->first < index) {
            index = itor->first;
            key = itor->second;
        }
    }

    if (key==posix::peekOption({key}, value))
        return key;
    else {
        auto parse_item = parse_pairs.begin();
        for (; parse_item!=parse_pairs.end(); ++parse_item) {
            if (parse_item->first == key) {
                value = parse_item->second;
                return key;
            }
        }
        value = "";
        return "";
    }
}

const std::string gnu::helpString(){
    string doc="NAME:\n";
    doc += "\t"+getName() + " -- " + getShortDescription() + "\n\n";


    doc += "USAGE:\n";
    doc += "\t" + getName() + " ";
    string temp1="[-", temp2="", short_options="", long_options="";

    auto short_opts = getShortOptions();
    for (auto itor=short_opts.begin(); itor!=short_opts.end(); ++itor) {
        if(itor->opt == "")
            continue;

        if (itor->placeholder==""){
            temp1 += itor->opt;
            short_options += "\t-" + itor->opt + "\n" +
                             "\t\t" + itor->cmt + "\n\n";
            continue;
        }

        temp2 += "[-" + itor->opt + " " + itor->placeholder + "] ";
        short_options += "\t-" + itor->opt + " " + itor->placeholder +"\n" +
                         "\t\t" + itor->cmt + "\n\n";

    }
    temp1 += "] ";

    for (auto itor=long_opts_table.begin(); itor!=long_opts_table.end(); ++itor) {
        if (itor->placeholder==""){
            temp2 += "[--"+itor->opt + "] ";
            long_options += "\t--" + itor->opt + "\n" +
                            "\t\t" + itor->cmt + "\n\n";
        }
        else{
            temp2 += "[--"+itor->opt + " " + itor->placeholder + "] ";
            long_options += "\t--" + itor->opt + " " + itor->placeholder + "\n" +
                            "\t\t" + itor->cmt + "\n\n";
        }
    }

    for(auto itor=short_opts.cbegin(); itor != short_opts.cend(); ++itor){
        if (itor->opt==""){
            temp2 += "[" + itor->placeholder + "] ";
            short_options += "\t" + itor->placeholder + "\n" +
                             "\t\t" + itor->cmt + "\n\n";
        }
    }

    doc += temp1 + temp2 + "\n\n";


    doc += "DESCRIPTION:\n";
    doc += "\t" + (getDescription()==""?"NONE.":getDescription()) + "\n\n";


    doc += "OPTIONS:\n";
    doc += short_options + long_options;

    return doc;
}







void args_extra::args_check_print(posix &tool, int argc, char *argv[]){
    std::vector<string> args;
    for (int var=0; var<argc; var++) {
        args.push_back(argv[var]);
    }

    try {
        tool.argsParse(args);
    } catch (ex_base* e) {
        std::cout<< e->type() << "::" << e->description() << std::endl;
        exit(0);
    }
}


























