#ifndef WS_POSIX_H
#define WS_POSIX_H

#include <string>
#include <list>

#define ARGS_CHECK(utilities) \
    try { \
    (utilities).args_accept(argc, argv); \
    } catch (args_extra::args_ex* e) { \
    std::cout << e->type() << " :: " << e->reason() << std::endl; \
    }

namespace args_extra {

    /**
     * 标准命令参数模板
     * 需要携带参数的命令，_placeholder必须填写，否则必须留空("")
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

        /**
         * @brief 设置命令的详细解释
         * @param desc 解释信息
         */
        void reset_description(const std::string &desc);

        /**
         * @brief 设置命令的开关参数
         * @param list
         */
        void set_opts_short(const std::initializer_list<ArgsGroup> &list);


        /**
         * @brief 处理posix类型参数
         * @param argc 参数数量
         * @param argv 参数buffer
         */
        virtual void args_accept(int argc, char* argv[]);

        /**
         * @brief 检测输入参数，同时获取key-value参数
         * @param keys 输入keys（互斥参数组），只有最后出现的key起作用
         * @param value 输出key配对的参数
         * @return 参数包含指定key且起作用，返回key，否则为空
         */
        virtual std::string get_option(std::initializer_list<std::string> keys,
                                       std::string &value) const{}

        virtual std::list<std::string> else_args();

        virtual std::string help_string(){}

    private:
        const std::string cmd_name;
        const std::string cmd_detial;
        std::string cmd_description;
        std::list<ArgsGroup> opts_table;
        std::list<std::string> args_list;
        std::list<std::string> args_left;
    };

    class dos
    {
    public:
        dos(int argc, char* argv[]);
    };
}

#endif // WS_POSIX_H
