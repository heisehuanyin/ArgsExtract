# ArgsExtract
A simple args-extract library. It is more convenients then getopt and getopt_long.

## HOW TO USE
```
    使用前先填写一些参数，这些参数会用于后期帮助等文档的自动生成。

    使用前先填写执行程序名称，和对名称的解释
    提供两种参数处理posix和gnu
    args_extra::gnu utool("extra","提取命令行参数测试程序");

    设置程序关注的短选项（posix类型），{“短选项”，“是否需要补充参数，如果不需要则留空”，“选项和参数的解释”}
    utool.setShort_opts({
                            {"a","","处理所有文件"},
                            {"h","somefile","显示帮助信息"},
                            {"w","","结尾计算辅助"},
                            {"","somefile","adkfjlakdfj"},
                            {"","a-few-message","adkfjlakdfj"}
                        });
                        
    设置程序关注的长选项（gnu类型），格式同短选项。
    utool.setLong_opts({
                           {"help","","显示帮助信息"},
                           {"vesion","","显示版本信息"},
                           {"pick","filename","测试pholder参数显示"},
                           {"pppp","aldkfjdslkfjskdfjlskf","akjflaskjflakjflkjflaksjflkjflkjajf"}
                       });

    执行默认的参数分析过程，如果出现参数错误，则打印错误信息到标准输出，如果想要定制次过程可以查看此函数源代码
    args_extra::args_check_print(utool, argc, argv);

    抽取其他的非选项补充参数，此参数单独传递给程序，开发者通常通过次序相关的方式使用这些参数
    auto g=utool.else_args();
    for (auto it=g.begin();it!=g.end();++it) {
        cout << *it << endl;
    }

    提供目标参数列表（选项间互斥），获取最终生效的选项和选项的补充参数
    string k,v;
    k = utool.get_option({"pppp"},v);
    std::cout << k << ":" << v << std::endl;
    
    获取自动生成的help-doc
    std::cout << utool.help_doc() << endl;

    return 0;
```
## help-doc format
```
NAME:
	extra -- 提取命令行参数测试程序

USAGE:
	extra [-aw] [-h somefile] [somefile] [a-few-message] [--help] [--vesion] [--pick filename] [--pppp aldkfjdslkfjskdfjlskf] 

DESCRIPTION:
	NONE.

OPTIONS:
	-a
		处理所有文件

	-h somefile
		显示帮助信息

	-w
		结尾计算辅助

	somefile
		adkfjlakdfj

	a-few-message
		adkfjlakdfj

	--help
		显示帮助信息

	--vesion
		显示版本信息

	--pick filename
		测试pholder参数显示

	--pppp aldkfjdslkfjskdfjlskf
		akjflaskjflakjflkjflaksjflkjflkjajf


```