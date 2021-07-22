//
// Created by lai on 2021/7/22.
//
#include "log.h"
namespace glweb{
    //构造函数后面加冒号，就是初始化列表，即-用括号内的成员变量来初始化m_name的值
    Logger::Logger(const std::string& name )
    :m_name(name){

    }
    void Logger::addAppender(LogAppender::ptr appender) {
        m_appenders.push_back(appender);

    }
    void Logger::delAppender(LogAppender::ptr appender) {
        //c++11使用迭代器进行遍历删除。
        for (auto it = m_appenders.begin();it != m_appenders.end();++it) {
            if (*it == appender){
                m_appenders.erase(it);
                break;
            }

        }

    }


    void Logger::log(LogLevel::Level level,LogEvent::ptr event) {
        if (level >= m_level){
            for(auto& i:m_appenders){
                i->log(level,event);
            }
        }



    }
    void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG,event);

    }
    void Logger::info(LogEvent::ptr event){
        log (LogLevel::INFO,event);

    }
    void Logger::warn(LogEvent::ptr event){
        log (LogLevel::WARN,event);
    }
    void Logger::error(LogEvent::ptr event){
        log (LogLevel::ERROR,event);
    }
    void Logger::fatal(LogEvent::ptr event){
        log (LogLevel::FATAL,event);
    }
    void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {

    }
    FileLogAppender::FileLogAppender(const std::string &filename)
    :m_filename(filename){

    }
    void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
        if (level >=m_level){
            m_filestream << m_formatter->format(event);
        }

    }
    bool FileLogAppender::reopen() {
        if (m_filestream){
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        //双引号是用来强制返回bool类型
        return !!m_filestream;

    }
}
