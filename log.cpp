//
// Created by lai on 2021/7/22.
//
#include "log.h"
#include "map"
#include "functional"
namespace glweb {
    //构造函数后面加冒号，就是初始化列表，即-用括号内的成员变量来初始化m_name的值
    Logger::Logger(const std::string &name)
            : m_name(name) {

    }

    void Logger::addAppender(LogAppender::ptr appender) {
        m_appenders.push_back(appender);

    }

    void Logger::delAppender(LogAppender::ptr appender) {
        //c++11使用迭代器进行遍历删除。
        for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
            if (*it == appender) {
                m_appenders.erase(it);
                break;
            }

        }

    }


    void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            auto self = shared_from_this();
            for (auto &i:m_appenders) {
                i->log(self, level, event);
            }
        }


    }

    void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG, event);

    }

    void Logger::info(LogEvent::ptr event) {
        log(LogLevel::INFO, event);

    }

    void Logger::warn(LogEvent::ptr event) {
        log(LogLevel::WARN, event);
    }

    void Logger::error(LogEvent::ptr event) {
        log(LogLevel::ERROR, event);
    }

    void Logger::fatal(LogEvent::ptr event) {
        log(LogLevel::FATAL, event);
    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            std::cout << m_formatter->format(logger, level, std::cout, event);
        }

    }

    FileLogAppender::FileLogAppender(const std::string &filename)
            : m_filename(filename) {

    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            m_filestream << m_formatter->format(logger, level, std::cout, event);
        }

    }

    bool FileLogAppender::reopen() {
        if (m_filestream) {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        //双引号是用来强制返回bool类型
        return !!m_filestream;

    }

    LogFormatter::LogFormatter(const std::string &pattern)
            : m_pattern(pattern) {}

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, std::ostream &os,
                                     LogEvent::ptr logEvent) {
        std::stringstream ss;
        for (auto &i : m_items) {
            i->format(level, logger, ss, logEvent);
        }
        return ss.str();

    }


    //各种格式的输出，每一类的格式都继承于FormatterItem，单独实现功能
    class NameFormatItem : public LogFormatter::FormatterItem {
    public:
        NameFormatItem(const std::string& str = "") {}
        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << logEvent->getLogger()->getName();
        }
    };
    class MessageFormatItem : public LogFormatter::FormatterItem {
    public:
        MessageFormatItem(const std::string& str = "") {}
        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << logEvent->getContent();

        }

    private:
        std::string m_message;

    };

    class LevelFormatItem : public LogFormatter::FormatterItem {
    public:
        LevelFormatItem(const std::string& str = "") {}
        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << LogLevel::Tostring(level);

        }
    };

    class ElapseFormatItem : public LogFormatter::FormatterItem {
    public:
        ElapseFormatItem(const std::string& str = "") {}
        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << logEvent->getElapse();

        }

    };

    class ThreadIdFormatItem : public LogFormatter::FormatterItem {
    public:
        ThreadIdFormatItem(const std::string& str = "") {}
        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << logEvent->getThreadId();

        }

    };

    class FiberFormatItem : public LogFormatter::FormatterItem {
    public:
        FiberFormatItem(const std::string& str = "") {}
        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << logEvent->getFiberId();

        }

    };

    class DataTimeFormatItem : public LogFormatter::FormatterItem {
    public:
        //给默认量需要前加const
        DataTimeFormatItem(const std::string &format = "%Y:%m:%d %H:%M:%S")
                : m_format(format) {

        }

        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << logEvent->getTime();

        }

    private:
        std::string m_format;

    };

    class FilenameFormatItem : public LogFormatter::FormatterItem {
    public:
        FilenameFormatItem(const std::string& str = "") {}
        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << logEvent->getFile();

        }

    };

    class LineFormatItem : public LogFormatter::FormatterItem {
    public:
        LineFormatItem(const std::string& str = "") {}
        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << logEvent->getLine();

        }

    };

    class NewLineFormatItem : public LogFormatter::FormatterItem {
    public:
        NewLineFormatItem(const std::string& str = "") {}
        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << std::endl;

        }
    };

    class TabFormatItem : public LogFormatter::FormatterItem {
    public:
        TabFormatItem(const std::string &str = "") {}

        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << "\t";

        }

    private:
        std::string m_string;
    };

    class StringFormatItem : public LogFormatter::FormatterItem {
    public:
        StringFormatItem(const std::string &str)
                : m_string(str) {
        }

        void format(LogLevel::Level level, std::shared_ptr<Logger> logger, std::ostream &os,
                    LogEvent::ptr logEvent) override {
            os << logEvent->getLine();

        }

    private:
        std::string m_string;

    };


    //各类格式输出类结束



    /**
* @brief 构造函数
* @param[in] pattern 格式模板
* @details
*  %m 消息
*  %p 日志级别
*  %r 累计毫秒数
*  %c 日志名称
*  %t 线程id
*  %n 换行
*  %d 时间
*  %f 文件名
*  %l 行号
*  %T 制表符
*  %F 协程id
*  %N 线程名称
*
*  默认格式 "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"
日志格式器，执行日志格式化，负责日志格式的初始化。
解析日志格式，将用户自定义的日志格式，解析为对应的FormatItem。
日志格式举例：%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n
格式解析：
%d{%Y-%m-%d %H:%M:%S} : %d 标识输出的是时间 {%Y-%m-%d %H:%M:%S}为时间格式，可选
*/
    void LogFormatter::init() {
        std::vector<std::tuple<std::string, std::string, int> > vec;
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); i) {
            //首先判断是不是百分号，如果不是百分号就直接记录下来
            if (m_pattern[i] != '%') {
                //在nstr结尾加入m_pattern[i]字符
                nstr.append(1, m_pattern[i]);
                continue;
            }
            //出现百分号情况，按照如下处理
            //两个百分号，说明确实是百分号，但是只记录百分号一次
            if (m_pattern.size() >= i + 1 && m_pattern[i + 1] == '%') {
                //在nstr结尾加入一个百分号
                nstr.append(1, '%');
                continue;
            }
            //解析格式
            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;
            std::string str;
            std::string fmt;
            //解析{%Y-%m-%d %H:%M:%S}内的内容
            while (n < m_pattern.size()) {
                if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                                    && m_pattern[n] != '}')) {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                //空格直接跳过
                if (isspace(m_pattern[n])) {
                    break;
                }
                if (fmt_status == 0) {
                    if (m_pattern[n] == '{') {
                        //substr(a,b)参数a表示从a开始，参数b表示从a开始数多少位，b可以是负数，负数无论是多少，都是从a开始到结尾,若b为0则会返回空。
                        //substr(a)表示从a开始到结尾
                        str = m_pattern.substr(i + 1, n - i - 1);
                        fmt_status = 1;
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }
                if (fmt_status == 1) {
                    if (m_pattern[n] == '}') {
                        //fmt_begin = n,n = n+1 ;
                        str = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size()) {
                    if (str.empty()) {
                        str = m_pattern.substr(i + 1);
                    }

                }
            }//while结束
            if (fmt_status == 0) {
                if (!nstr.empty()) {
                    //string()返回一个空字符串
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            } else if (fmt_status == 1) {
                std::cout << "pattern error at:" << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern error>>", fmt, 0));
            }


        }
        if (!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
        //这是一个function的泛型应用,利用map查找应该调用的函数
        static std::map<std::string,std::function<FormatterItem::ptr(const std::string& str)> > s_fmt_items = {
                //[](const std::string& fmt){return FormatterItem::ptr(new c(fmt));是lambda表达式，
#define XX(str, c) \
        {#str, [](const std::string& fmt){return FormatterItem::ptr(new c(fmt));}}

                XX(m, MessageFormatItem),           //m:消息
                XX(p, LevelFormatItem),             //p:日志级别
                XX(r, ElapseFormatItem),            //r:累计毫秒数
                XX(c, NameFormatItem),              //c:日志名称
                XX(t, ThreadIdFormatItem),          //t:线程id
                XX(n, NewLineFormatItem),           //n:换行
                XX(d, DataTimeFormatItem),          //d:时间
                XX(f, FilenameFormatItem),          //f:文件名
                XX(l, LineFormatItem),              //l:行号
                XX(T, TabFormatItem),               //T:Tab
                XX(F, FiberFormatItem),           //F:协程id
//                XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
        };
        for (auto& i : vec) {
            if (std::get<2>(i) == 0){
                m_items.push_back(FormatterItem::ptr(new StringFormatItem(std::get<0>(i))));
            } else{
                auto it = s_fmt_items.find(std::get<0>(i));
                if (it == s_fmt_items.end()){
                    m_items.push_back(FormatterItem::ptr(new StringFormatItem("<<error format %"+std::get<0>(i )+">>")));
                } else{
                     m_items.push_back(it->second(std::get<1>(i)));
                }

            }
            std::cout << std::get<0>(i) <<  " - " << std::get<1>(i) << std::endl;

        }


    }

    const char *LogLevel::Tostring(LogLevel::Level level) {
        switch (level) {
            //#define的巧妙用法，define有三个特殊符号：#，##，#@，其中#表示给变量加上双引号，比如#123就变成了“123”，
            //##表示将两者链接，可以是数字也可以是字符串，比如123##456输出123456，或者“abc“##”123“输出”abc123“，
            // #@表示加上单引号，变为字符型，注意字符型只能是一个，#@a-->‘a’，如果两个以上字符会报错，
            // 在实际中，我们可以用以上方式，很快的将int形转为字符或字符串，而以下代码中有\来进行跨行定义表示前后来两个是分开的，实际上就是：
            //#define XX(name)  case LogLevel::name:  return #name;  break;
            // undef表示取消定义
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX
            default:
                return "UNKNOW";
        }
        return "UNKNOW";
    }



}


