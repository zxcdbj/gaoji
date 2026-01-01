MFC Lightweight Multi-Task Scheduler (MFC轻量级多任务调度器)

📖 项目简介

这是一个基于 MFC (Microsoft Foundation Classes) 开发的轻量级多任务调度系统。该项目旨在演示现代 C++ 中的多线程编程、同步机制（互斥锁、条件变量）以及经典软件设计模式的实际应用。

系统支持一次性任务、延迟任务和周期性任务的调度，并提供了图形化界面来监控任务状态和日志。此外，本项目还包含专门用于教学演示的死锁（Deadlock）及其预防机制的模块。

✨ 核心功能

多模式调度：支持立即执行、延时执行（Delay）和周期性执行（Interval）任务。

线程安全：核心调度器运行在独立的工作线程中，通过 std::mutex 和 std::condition_variable 保证线程安全。

实时日志：使用 List Control 控件实时显示任务的运行状态（Running, Finished, Error）。

启停控制：支持动态启动和停止调度器，并清除未执行的任务队列。

死锁演示与预防：内置专门的任务用例，直观演示多线程死锁现象及解决方案。

🏗️ 架构与设计模式

本项目严格遵循面向对象设计原则，采用了以下设计模式：

Singleton (单例模式): TaskScheduler 类，确保系统中只有一个调度器实例。

Strategy (策略模式): ITask 接口，定义了任务的抽象行为，不同任务（如矩阵乘法、文件备份）实现不同的策略。

Command (命令模式): ScheduledTask 类，封装了任务请求及其元数据（执行时间、间隔），便于队列管理。

Observer (观察者模式): 通过 SetUICallback 机制，调度器将状态变化通知给 UI 线程，实现逻辑与界面的解耦。

RAII (资源获取即初始化): LogWriter 类，利用对象生命周期自动管理文件句柄的打开与关闭。

📂 任务清单 (演示用例)

项目内置了 9 种不同类型的任务（Task A - Task I）以演示不同场景：

任务ID

名称

类型

描述

Task A

文件备份

一次性 (延迟 5s)

模拟文件创建及 PowerShell 压缩备份操作。

Task B

矩阵乘法

周期性 (每 5s)

执行 200x200 矩阵乘法，模拟 CPU 密集型计算。

Task C

网络请求

一次性 (立即)

模拟 HTTP GET 请求并保存结果到本地文件。

Task D

课堂提醒

周期性 (每 5s)

跨线程 UI 交互，每 5 秒弹出 "休息 5 分钟" 提示框。

Task E

随机数统计

一次性 (延迟 10s)

生成随机数并计算均值与方差。

Task F

死锁演示 1

一次性

经典的 AB-BA 死锁模型，会导致程序卡死。

Task G

死锁演示 2

一次性

重复死锁逻辑，验证死锁的必然性。

Task H

死锁预防 1

一次性

使用 std::lock 算法同时锁定多个互斥量，防止死锁。

Task I

死锁预防 2

一次性

使用锁层级 (Lock Hierarchy) 策略（固定加锁顺序）防止死锁。

🚀 快速开始

环境要求

Windows 10/11

Visual Studio 2022 (安装了 "使用 C++ 的桌面开发" 及 MFC 组件)

编译步骤

克隆或下载本项目代码。

使用 Visual Studio 2022 打开 TaskSchedulerMFC.sln。

确保解决方案配置为 Debug 或 Release，平台选择 x64 或 x86。

点击菜单栏 生成 (Build) -> 生成解决方案 (Build Solution)。

注意：如果在 ConcreteTasks.h 中遇到字符编码警告，请将文件保存为 "UTF-8 带签名 (BOM)" 格式。

按 F5 运行程序。

🖥️ 使用说明

添加任务：点击界面顶部的 Task A 至 Task E 按钮，将不同类型的任务加入队列。

观察日志：下方的列表框会实时刷新任务的执行时间、状态和详细信息。

测试死锁：

点击 Task F 或 Task G，程序界面将卡死（这是预期行为，演示死锁）。此时需强制结束程序。

测试死锁预防：

点击 Task H 或 Task I，观察日志中多线程交替获取锁并正常结束，验证预防机制有效。

停止调度：点击 Stop Scheduler 按钮，将立即停止后台线程并清空任务队列。再次点击任意任务按钮可自动重启调度器。

📄 目录结构

MFCTaskScheduler/
├── TaskSchedulerMFC.cpp       # 应用程序入口
├── TaskSchedulerMFCDlg.cpp    # 主对话框界面逻辑 (Observer实现)
├── TaskSchedulerMFCDlg.h      # 对话框头文件
├── TaskScheduler.cpp          # 调度器核心实现 (Singleton, Thread Loop)
├── TaskScheduler.h            # 调度器头文件
├── ITask.h                    # 任务抽象接口
├── ConcreteTasks.h            # 具体任务实现 (Task A-I)
├── LogWriter.h                # RAII 日志记录器
├── resource.h                 # 资源ID定义
└── ...
