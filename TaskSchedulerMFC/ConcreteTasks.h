#pragma once
#include "pch.h" 
#include "ITask.h"
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <thread> 
#include <mutex>  

// 获取当前日期字符串 (防止重复定义：inline)
inline std::string GetCurrentDateStr() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d");
    return ss.str();
}

// 全局互斥锁 (防止重复定义：inline)
inline std::mutex g_mutexA;
inline std::mutex g_mutexB;

// ================= 原有任务 A-E (中文名称) =================

// Task A: 文件备份
class FileBackupTask : public ITask {
public:
    std::string GetName() const override { return "任务 A: 文件备份"; }
    void Execute(LogWriter* logger) override {
        std::string sourceDir = ".\\DemoData";
        std::string backupDir = ".\\Backups";
        system(("mkdir " + sourceDir).c_str());
        std::ofstream(sourceDir + "\\data.txt") << "Important Data " << time(0);
        system(("mkdir " + backupDir).c_str());
        std::string dateStr = GetCurrentDateStr();
        std::string zipPath = backupDir + "\\backup_" + dateStr + ".zip";
        std::stringstream cmd;
        cmd << "powershell -Command \"Compress-Archive -Path '" << sourceDir << "' -DestinationPath '" << zipPath << "' -Force\"";
        logger->Write("Task A: Starting backup...");
        int ret = system(cmd.str().c_str());
        if (ret == 0) logger->Write("Task A: Backup succeeded to " + zipPath);
        else logger->Write("Task A: Backup failed.");
    }
};

// Task B: 矩阵乘法
class MatrixMultiplyTask : public ITask {
public:
    std::string GetName() const override { return "任务 B: 矩阵乘法"; }
    void Execute(LogWriter* logger) override {
        const int N = 200;
        std::vector<std::vector<double>> A(N, std::vector<double>(N, 1.0));
        std::vector<std::vector<double>> B(N, std::vector<double>(N, 1.0));
        std::vector<std::vector<double>> C(N, std::vector<double>(N, 0.0));
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                for (int k = 0; k < N; ++k) C[i][j] += A[i][k] * B[k][j];
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::stringstream ss;
        ss << "Task B: Calc completed in " << duration.count() << " ms.";
        logger->Write(ss.str());
    }
};

// Task C: HTTP GET
class HttpGetZenTask : public ITask {
public:
    std::string GetName() const override { return "任务 C: 网络请求"; }
    void Execute(LogWriter* logger) override {
        logger->Write("Task C: Requesting API...");
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        std::string content = "Keep it logically awesome.";
        std::ofstream ofs("zen.txt");
        if (ofs.is_open()) { ofs << content; ofs.close(); logger->Write("Task C: Written to zen.txt."); }
    }
};

// Task D: 课堂提醒
class ClassReminderTask : public ITask {
public:
    std::string GetName() const override { return "任务 D: 课堂提醒"; }
    void Execute(LogWriter* logger) override {
        logger->Write("Task D: Triggering UI reminder...");
    }
};

// Task E: 随机数统计
class RandomStatsTask : public ITask {
public:
    std::string GetName() const override { return "任务 E: 随机数统计"; }
    void Execute(LogWriter* logger) override {
        std::random_device rd; std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 100);
        double sum = 0, sum_sq = 0; const int count = 1000;
        for (int i = 0; i < count; ++i) { int val = dis(gen); sum += val; sum_sq += val * val; }
        double mean = sum / count;
        double variance = (sum_sq / count) - (mean * mean);
        std::stringstream ss; ss << "Task E: Mean=" << mean << ", Var=" << variance;
        logger->Write(ss.str());
    }
};

// ================= 新增任务 F-I (完整注释版) =================

// ==========================================
// Task F: 演示死锁 (AB-BA 模型)
// ==========================================
class DeadlockTask1 : public ITask {
public:
    std::string GetName() const override { return "任务 F: 死锁演示 (AB-BA)"; }

    void Execute(LogWriter* logger) override {
        logger->Write("Task F: Starting AB-BA Deadlock Demo...");

        std::thread t1([&]() {
            // 线程 1：先尝试获取锁 A
            logger->Write("Thread 1: Waiting for Lock A...");
            g_mutexA.lock();
            logger->Write("Thread 1: Acquired Lock A.");

            // 休眠 100ms，确保线程 2 有时间获取锁 B，从而制造交叉等待的局势
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // 线程 1：现在尝试获取锁 B，但此时 B 已被线程 2 持有
            logger->Write("Thread 1: Waiting for Lock B (Deadlock point)...");
            g_mutexB.lock(); // <--- 程序将在这里永久阻塞
            logger->Write("Thread 1: Acquired Lock B.");
            g_mutexA.unlock();
            g_mutexB.unlock();
            });

        std::thread t2([&]() {
            // 线程 2：先尝试获取锁 B
            logger->Write("Thread 2: Waiting for Lock B...");
            g_mutexB.lock();
            logger->Write("Thread 2: Acquired Lock B.");

            // 休眠 100ms，确保线程 1 已经持有锁 A
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // 线程 2：现在尝试获取锁 A，但此时 A 已被线程 1 持有
            logger->Write("Thread 2: Waiting for Lock A (Deadlock point)...");
            g_mutexA.lock(); // <--- 程序将在这里永久阻塞
            logger->Write("Thread 2: Acquired Lock A.");
            g_mutexB.unlock();
            g_mutexA.unlock();
            });

        // 由于死锁发生，主线程会永远等待 t1 和 t2 结束
        t1.join();
        t2.join();
        logger->Write("Task F: Finished (Unreachable).");
    }
};

// ==========================================
// Task G: 再次演示死锁 (简化版)
// ==========================================
class DeadlockTask2 : public ITask {
public:
    std::string GetName() const override { return "任务 G: 死锁演示 (重复)"; }

    void Execute(LogWriter* logger) override {
        logger->Write("Task G: Starting Second Deadlock Demo...");
        std::thread t1([&]() {
            // 线程 1 的加锁顺序：A -> B
            g_mutexA.lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            g_mutexB.lock(); // 等待 B (被 t2 持有)
            g_mutexA.unlock();
            g_mutexB.unlock();
            });

        std::thread t2([&]() {
            // 线程 2 的加锁顺序：B -> A
            // 顺序相反，会导致死锁
            g_mutexB.lock();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            g_mutexA.lock(); // 等待 A (被 t1 持有)
            g_mutexB.unlock();
            g_mutexA.unlock();
            });
        t1.join();
        t2.join();
    }
};

// ==========================================
// Task H: 演示防死锁 (使用 std::lock) 
// ==========================================
class PreventionTask1 : public ITask {
public:
    // [修复] 这里的字符串必须以双引号 " 结尾
    std::string GetName() const override { return "任务 H: 死锁预防(使用 std::lock)"; }

    void Execute(LogWriter* logger) override {
        logger->Write("Task H: Starting std::lock Demo...");

        std::thread t1([&]() {
            logger->Write("Thread 1: Attempting std::lock(A, B)...");

            // 核心代码：std::lock
            // 这是一个原子操作（或算法实现），同时锁定 A 和 B。
            // 即使另一个线程顺序相反，std::lock 也能避免死锁。
            std::lock(g_mutexA, g_mutexB);

            // 使用 lock_guard 管理，std::adopt_lock 表示锁已被持有
            std::lock_guard<std::mutex> lock1(g_mutexA, std::adopt_lock);
            std::lock_guard<std::mutex> lock2(g_mutexB, std::adopt_lock);

            logger->Write("Thread 1: Acquired Both Locks.");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            logger->Write("Thread 1: Released Locks.");
            });

        std::thread t2([&]() {
            logger->Write("Thread 2: Attempting std::lock(B, A)...");

            // 即使顺序是 B -> A，std::lock 也能安全处理
            std::lock(g_mutexB, g_mutexA);

            std::lock_guard<std::mutex> lock2(g_mutexB, std::adopt_lock);
            std::lock_guard<std::mutex> lock1(g_mutexA, std::adopt_lock);

            logger->Write("Thread 2: Acquired Both Locks.");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            logger->Write("Thread 2: Released Locks.");
            });

        t1.join();
        t2.join();
        logger->Write("Task H: Finished Successfully.");
    }
};

// ==========================================
// Task I: 演示防死锁 (锁顺序层级) 
// ==========================================
class PreventionTask2 : public ITask {
public:
    std::string GetName() const override { return "任务 I: 死锁预防 (层级锁)"; }

    void Execute(LogWriter* logger) override {
        logger->Write("Task I: Starting Hierarchy Lock Demo...");

        std::thread t1([&]() {
            logger->Write("Thread 1: Locking A then B...");

            // 规则：强制所有线程必须按 "先 A 后 B" 的顺序加锁
            std::lock_guard<std::mutex> lock1(g_mutexA);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            std::lock_guard<std::mutex> lock2(g_mutexB);

            logger->Write("Thread 1: Done.");
            });

        std::thread t2([&]() {
            logger->Write("Thread 2: Locking A then B...");

            // 线程 2 遵守 "先 A 后 B" 的规则
            // 这样就不会出现 "我持有B等你A" 的循环等待情况
            std::lock_guard<std::mutex> lock1(g_mutexA);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            std::lock_guard<std::mutex> lock2(g_mutexB);

            logger->Write("Thread 2: Done.");
            });

        t1.join();
        t2.join();
        logger->Write("Task I: Finished Successfully.");
    }
};
