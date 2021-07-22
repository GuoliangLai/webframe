//
// Created by lai on 2021/7/22.
//
#include "log.h"

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
            for (auto &i:m_appenders) {
                i->log(level, event);
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

    void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            std::cout << m_formatter->format(std::cout,event);
        }

    }

    FileLogAppender::FileLogAppender(const std::string &filename)
            : m_filename(filename) {

    }

    void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            m_filestream << m_formatter->format(std::cout,event);
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

    std::string LogFormatter::format(std::ostream &os, LogEvent::ptr event) {
        std::stringstream ss;
        for (auto &i : m_items) {
            i->format(ss, event);
        }
        return ss.str();

    }

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
                if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
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
                        str = m_pattern.substr(i + 1, n - i + 1);
                        fmt_status = 1;
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }
                if (fmt_status == 1) {
                    if (m_pattern[n] == '}') {
                        str = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size()){
                    if (str.empty()){
                        str = m_pattern.substr(i+1);
                    }

                }
            }//while结束


        }


    }
}


