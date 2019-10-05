#ifndef WS_POSIX_H
#define WS_POSIX_H

#include <string>
#include <list>
#include <vector>


namespace args_extra {

    /**
     * 标准命令参数模板
     * 需要携带参数的命令，_placeholder必须填写，反之留空("")
     * opt可以不填，表示操作数占位，获取此操作数位置相关
     */
    typedef struct {
        std::string opt;                    // 开关选项
        std::string placeholder;           // 补充参数
        std::string cmt;                    // 选项解释
    } ArgsPack;



    /**
     * @brief 解析过程异常
     */
    class ex_base : public std::exception
    {
    public:
        explicit ex_base(const std::string &type);
        virtual ~ex_base() = default;

        const std::string &type();

        virtual void setDescription(const std::string& description);
        virtual const std::string& description();

    private:
        const std::string _type;
        std::string _description;
    };

    class ex_inputs : public ex_base
    {
    public:
        explicit ex_inputs(const std::string &description);
        virtual ~ex_inputs() = default;
    };

    class ex_design : public ex_base
    {
    public:
        explicit ex_design(const std::string &description);
        virtual ~ex_design() = default;
    };

    /**
     * @brief 一个处理posix类型命令行参数的工具
     */
    class posix
    {
    public:
        /**
         * @brief 新建处理工具，传入命令名称和相应解释
         * @param name 命令名称
         * @param shortDesc 简短的命令介绍
         */
        posix(const std::string &name, const std::string &shortDesc);
        virtual ~posix() = default;

        /**
         * @brief 获取命令名称
         * @return 命令名称
         */
        const std::string& getName() const;
        /**
         * @brief 获取命令简介
         * @return 命令简介
         */
        const std::string& getShortDescription() const;

        /**
         * @brief 设置命令的详细解释
         * @param desc 解释信息
         */
        void resetDescription(const std::string &desc);
        const std::string& getDescription() const;

        /**
         * @brief 设置短格式命令选项，可以设置携带参数选项、不携带参数选项、无选项纯参数三种格式
         * @param list 详细格式列表
         */
        void setShortOptions(const std::initializer_list<ArgsPack> &list);
        /**
         * @brief 获取内部设定的选项参数
         * @return 参数表
         */
        const std::list<ArgsPack>& getShortOptions() const;


        /**
         * @brief 处理posix类型参数
         * @param argc 参数数量
         * @param argv 参数buffer
         */
        virtual void argsParse(std::vector<std::string>& argv);

        /**
         * @brief 检测输入参数，同时获取key-value参数
         * @param opts 输入keys（互斥参数组），只有最后出现的key起作用
         * @param value 输出key配对的参数
         * @return 参数包含指定key且起作用，返回key，否则为空
         */
        virtual const std::string peekOption(const std::initializer_list<std::string> &opts, std::string &argument) const;

        /**
         * @brief 解析出来的剩余参数
         * @return 其他参数列表
         */
        virtual const std::list<std::string>& elseArguments() const;

        /**
         * @brief 获取自动生成的帮助文档
         * @return helpString
         */
        virtual const std::string helpString();

    private:
        const std::string cmd_name;
        const std::string cmd_detial;
        std::string cmd_description;
        std::list<ArgsPack> short_opts_table;

        std::list<std::pair<std::string,std::string>> parse_pairs;
        std::list<std::string> args_lift;
    };

    class gnu : public posix
    {
    public:
        gnu(const std::string &name, const std::string &detail);
        virtual ~gnu() override = default;

        /**
         * @brief 设置长格式选项，可以设置待参数选项、不带参数选项
         * @param list 参数列表
         */
        void setLongOptions(const std::initializer_list<ArgsPack> &list);
        const std::list<ArgsPack>& getLongOptions() const;

        /**
         * @brief 处理gnu类型参数
         * @param argc 参数数量
         * @param argv 参数buffer
         */
        virtual void argsParse(std::vector<std::string>& argv) override;

        const std::string peekOption(const std::initializer_list<std::string> &opts, std::string &value) const override;
        const std::string helpString() override;

    private:
        std::list<ArgsPack> long_opts_table;
        std::list<std::string> original_args;

        std::list<std::pair<std::string,std::string>> parse_pairs;
    };


    void args_check_print(posix& tool, int argc, char* argv[]);
}

#endif // WS_POSIX_H
