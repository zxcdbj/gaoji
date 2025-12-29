#pragma once
#include <fstream>
#include <string>

// LogWriter 类：使用 RAII 模式管理日志文件写入
class LogWriter {
public:
    // 默认构造函数：以追加模式打开默认日志文件
    // 这样就可以在 TaskScheduler 中使用 LogWriter logger;
    LogWriter() {
        m_file.open("scheduler_log.txt", std::ios::app);
    }

    // 析构函数：RAII 核心机制，对象销毁时自动关闭文件
    ~LogWriter() {
        if (m_file.is_open()) {
            m_file.close();
        }
    }

    // 禁止拷贝和赋值，防止多个对象操作同一个文件指针
    LogWriter(const LogWriter&) = delete;
    LogWriter& operator=(const LogWriter&) = delete;

    // 写入日志方法
    void Write(const std::string& msg) {
        if (m_file.is_open()) {
            m_file << msg << std::endl;
            m_file.flush(); // 确保立即写入磁盘
        }
    }

private:
    std::ofstream m_file;
};
