//
// Created by lai on 2021/7/22.
//
#include "string"
#include "stdint.h"
#include "memory"
#include "list"
#include "sstream"
#include "fstream"
#include "iostream"
#include "vector"
#ifndef WEBFRAME_LOG_H
#define WEBFRAME_LOG_H


namespace glweb{
    //由于类顺序不同，可能后面的类用到了前面的类，因此需要前置声明
    class Logger;
    //日志时间
    class LogEvent{
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent() ;
        const char* getFile() const {return m_file;}
        int32_t getLine() const {return m_line;}
        uint32_t getElapse() const {return m_elapse;}
        uint32_t getThreadId() const {return m_threadId;}
        uint32_t getFiberId() const {return m_fiberId;}
        uint64_t getTime() const {return m_time;}
        std::string getContent() const {return m_content;}
    private:
        //定义一个指向字符常量的指针,无法修改mfile来修改内容，常用于文件名设置
        const char* m_file = nullptr;
        int32_t m_line = 0;            //行号
        uint32_t m_elapse = 0;         //程序启动到现在运行的ms数
        uint32_t m_threadId = 0;       //线程id
        uint32_t m_fiberId = 0;        //协程Id
        uint64_t m_time = 0;           //时间戳
        std::string m_content;         //内容

    };
    //日志级别
    class LogLevel{
    public:
        enum Level{
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };
        static const char* Tostring(LogLevel::Level level);


    };
    //日志格式器

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
  */
    class LogFormatter{
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string& pattern);
        //时间+消息+线程号等等。。
        std::string format(std::shared_ptr<Logger> logger,LogLevel::Level level,std::ostream& os,LogEvent::ptr logEvent);
        void init();
    public:
        class FormatterItem{
        public:
            typedef std::shared_ptr<FormatterItem> ptr;
            virtual ~FormatterItem();
            virtual void format(LogLevel::Level level,std::shared_ptr<Logger> logger,std::ostream& os,LogEvent::ptr logEvent) = 0;

        };
    private:
        //m_pattern 是日志的格式，可以自己设置
        std::string m_pattern;
        std::vector<FormatterItem::ptr> m_items;


    };
    //日志输出地,由于其子类需要用到日志级别，因此需要设置为protected
    class LogAppender{
//        friend Logger;
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        //子类必须重载log，并且需要重新声明。一个类函数的调用并不是在编译时刻被确定的，而是在运行时刻被确定的。由于编写代码的时候并不能确定被调用的是基类的函数还是哪个派生类的函数，所以被成为"虚"函数。
        // 虚函数只能借助于指针或者引用来达到多态的效果。纯虚函数是在基类中声明的虚函数，它在基类中没有定义，但要求任何派生类都要定义自己的实现方法。在基类中实现纯虚函数的方法是在函数原型后加 =0:
        virtual ~LogAppender() {};
        virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) = 0;
        void setFormatter(LogFormatter::ptr formatter) { m_formatter=formatter;}
        LogFormatter::ptr getFormatter()const { return m_formatter;}
    protected:
        LogLevel::Level m_level;
        LogFormatter::ptr m_formatter;
    };

    //日志器
    class Logger{
    public:
        typedef std::shared_ptr<Logger> ptr;
        //默认用户名为root
        Logger(const std::string& name = "root");
        void log(LogLevel::Level level,LogEvent::ptr event);
        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);
        //获取和设置日志级别，函数后面加const表示此函数以“只读”的方式运行，即此函数不会修改类内的值，而只是取值。实际上就是
        // 表示成员函数隐含传入的this指针为const指针，决定了在该成员函数中， 任意修改它所在的类的成员的操作都是不允许的（因为隐含了对this指针的const引用）；下式其实省略了this指针
        LogLevel::Level getLevel() const {return m_level;}
        void setLever(LogLevel::Level level) {m_level=level;}
        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);
        std::string getName() const {return m_name;}

    private:
        std::string m_name;
        LogLevel::Level m_level;
        std::list<LogAppender::ptr> m_appenders;

    };
    //输出到控制台的appender
    class StdoutLogAppender : public LogAppender{
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;

    };
    //输出到文件
    class  FileLogAppender : public LogAppender{
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string& filename);
        void log(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event) override;
        bool reopen();
    private:
        std::string m_filename;
        std::ofstream m_filestream;

    };
}
#endif //WEBFRAME_LOG_H
