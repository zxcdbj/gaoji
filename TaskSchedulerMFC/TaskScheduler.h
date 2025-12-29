#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <memory>
#include <chrono>
#include <atomic>
#include <functional> // 确保包含这个

// 前向声明
class ITask;
class LogWriter;

// 任务更新信息结构
struct TaskUpdateInfo {
    std::string status;
    std::string message;
};

// 包裹任务的内部类
class ScheduledTask {
public:
    std::shared_ptr<ITask> task;
    std::chrono::system_clock::time_point runTime;
    bool isPeriodic;
    long long intervalMs;

    ScheduledTask(std::shared_ptr<ITask> t, std::chrono::system_clock::time_point time, bool periodic = false, long long interval = 0)
        : task(t), runTime(time), isPeriodic(periodic), intervalMs(interval) {
    }

    // 用于 priority_queue 排序
    bool operator>(const ScheduledTask& other) const {
        return runTime > other.runTime;
    }
};

class TaskScheduler {
public:
    static TaskScheduler& GetInstance();

    void AddOneTimeTask(std::shared_ptr<ITask> task, long long delayMs);
    void AddPeriodicTask(std::shared_ptr<ITask> task, long long intervalMs);

    // 启动和停止方法
    void Start();
    void Stop();

    // 【关键修复】这里必须只接受 1 个参数
    void SetUICallback(std::function<void(TaskUpdateInfo)> callback);

private:
    TaskScheduler();
    ~TaskScheduler();

    TaskScheduler(const TaskScheduler&) = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;

    void Run();

    std::priority_queue<ScheduledTask, std::vector<ScheduledTask>, std::greater<ScheduledTask>> m_taskQueue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::thread m_workerThread;
    std::function<void(TaskUpdateInfo)> m_uiCallback; // 成员变量

    std::atomic<bool> m_stopRequested;
};
