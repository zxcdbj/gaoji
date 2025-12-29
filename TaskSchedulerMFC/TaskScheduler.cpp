// TaskScheduler.cpp : 任务调度器的实现
//

#include "pch.h"           
#include "TaskScheduler.h"
#include "ITask.h"
#include "LogWriter.h"      
#include "ConcreteTasks.h"  
#include <iostream>
#include <functional>  // <--- 【关键修复】必须添加这一行，解决 E0135 错误

// ==========================================
// 构造与析构
// ==========================================

TaskScheduler::TaskScheduler() : m_stopRequested(false) {
    // LogWriter 初始化取决于其具体实现
}

TaskScheduler::~TaskScheduler() {
    Stop();
}

TaskScheduler& TaskScheduler::GetInstance() {
    static TaskScheduler instance;
    return instance;
}

// ==========================================
// 启动调度器
// ==========================================
void TaskScheduler::Start() {
    if (!m_workerThread.joinable()) {
        m_stopRequested = false;
        m_workerThread = std::thread(&TaskScheduler::Run, this);
    }
}

// ==========================================
// 停止调度器
// ==========================================
void TaskScheduler::Stop() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stopRequested = true;

        while (!m_taskQueue.empty()) {
            m_taskQueue.pop();
        }
    }

    m_cv.notify_one();

    if (m_workerThread.joinable()) {
        m_workerThread.join();
    }
}

// ==========================================
// 添加一次性任务
// ==========================================
void TaskScheduler::AddOneTimeTask(std::shared_ptr<ITask> task, long long delayMs) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto now = std::chrono::system_clock::now();
        auto runTime = now + std::chrono::milliseconds(delayMs);
        m_taskQueue.emplace(task, runTime, false, 0);
    }
    m_cv.notify_one();
    Start();
}

// ==========================================
// 添加周期性任务
// ==========================================
void TaskScheduler::AddPeriodicTask(std::shared_ptr<ITask> task, long long intervalMs) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto now = std::chrono::system_clock::now();
        auto runTime = now + std::chrono::milliseconds(intervalMs);
        m_taskQueue.emplace(task, runTime, true, intervalMs);
    }
    m_cv.notify_one();
    Start();
}

// ==========================================
// 设置 UI 回调
// ==========================================
void TaskScheduler::SetUICallback(std::function<void(TaskUpdateInfo)> callback) {
    m_uiCallback = callback;
}

// ==========================================
// 工作线程主循环
// ==========================================
void TaskScheduler::Run() {
    while (true) {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_stopRequested) {
            return;
        }

        if (m_taskQueue.empty()) {
            m_cv.wait(lock);
        }
        else {
            auto now = std::chrono::system_clock::now();
            auto& topTask = m_taskQueue.top();

            if (topTask.runTime > now) {
                m_cv.wait_until(lock, topTask.runTime);
            }
        }

        if (m_stopRequested) {
            return;
        }

        if (m_taskQueue.empty()) continue;

        auto now = std::chrono::system_clock::now();

        if (m_taskQueue.top().runTime <= now) {
            ScheduledTask current = m_taskQueue.top();
            m_taskQueue.pop();

            lock.unlock();

            try {
                if (m_uiCallback) {
                    m_uiCallback({ "RUN", current.task->GetName() });
                }

                LogWriter logger;
                current.task->Execute(&logger);

                if (m_uiCallback) {
                    m_uiCallback({ "DONE", current.task->GetName() });
                }
            }
            catch (std::exception& e) {
                if (m_uiCallback) {
                    m_uiCallback({ "ERR", e.what() });
                }
            }

            if (current.isPeriodic && !m_stopRequested) {
                std::lock_guard<std::mutex> reLock(m_mutex);
                auto nextTime = std::chrono::system_clock::now() + std::chrono::milliseconds(current.intervalMs);
                m_taskQueue.emplace(current.task, nextTime, true, current.intervalMs);
            }
        }
    }
}
