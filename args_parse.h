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
        std::string opt;                    // 开关指令
        std::string _placeholder;           // 补充参数
        std::string cmt;                    // 开关解释
    } ArgsGroup;



    class args_ex : public std::exception
    {
    public:
        explicit args_ex(const std::string &type);
        virtual ~args_ex() = default;

        std::string type();

        virtual void set_detial(const std::string& reason);
        virtual std::string reason();

    private:
        const std::string indicator;
        std::string detial;
    };

    class args_input_ex : public args_ex
    {
    public:
        explicit args_input_ex(const std::string &reason);
        virtual ~args_input_ex() = default;
    };

    class opts_design_ex : public args_ex
    {
    public:
        explicit opts_design_ex(const std::string &reason);
        virtual ~opts_design_ex() = default;
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
         * @param detial 命令简略解释
         */
        posix(const std::string &name, const std::string &detial);
        virtual ~posix() = default;

        std::string get_name() const;
        std::string get_detial() const;

        /**
         * @brief 设置命令的详细解释
         * @param desc 解释信息
         */
        void reset_description(const std::string &desc);
        std::string get_description() const;

        /**
         * @brief 设置命令的开关参数
         * @param list
         */
        void setShort_opts(const std::initializer_list<ArgsGroup> &list);
        std::list<ArgsGroup> getShort_opts() const;


        /**
         * @brief 处理posix类型参数
         * @param argc 参数数量
         * @param argv 参数buffer
         */
        virtual void args_accept(std::vector<std::string> argv);

        /**
         * @brief 检测输入参数，同时获取key-value参数
         * @param opts 输入keys（互斥参数组），只有最后出现的key起作用
         * @param value 输出key配对的参数
         * @return 参数包含指定key且起作用，返回key，否则为空
         */
        virtual std::string get_option(const std::initializer_list<std::string> &opts,
                                       std::string &value) const;

        virtual std::list<std::string> else_args();

        virtual std::string help_doc();

    private:
        const std::string cmd_name;
        const std::string cmd_detial;
        std::string cmd_description;
        std::list<ArgsGroup> short_opts_table;

        std::list<std::pair<std::string,std::string>> parse_pairs;
        std::list<std::string> args_left;
    };

    class gnu : public posix
    {
    public:
        gnu(const std::string &name, const std::string &detail);
        virtual ~gnu() override = default;

        void setLong_opts(const std::initializer_list<ArgsGroup> &list);
        std::list<ArgsGroup> getLong_opts() const;

        /**
         * @brief 处理gnu类型参数
         * @param argc 参数数量
         * @param argv 参数buffer
         */
        virtual void args_accept(std::vector<std::string> argv) override;

        std::string get_option(const std::initializer_list<std::string> &opts, std::string &value) const override;
        std::string help_doc() override;

    private:
        std::list<ArgsGroup> long_opts_table;
        std::list<std::string> original_args;

        std::list<std::pair<std::string,std::string>> parse_pairs;
    };


    void args_check_print(posix& tool, int argc, char* argv[]);
}

#endif // WS_POSIX_H
