#pragma once
#include "pch.h"
#include <string>
#include <memory>
#include "LogWriter.h"

// Strategy Pattern: 抽象任务接口
class ITask {
public:
    virtual ~ITask() = default;
    virtual std::string GetName() const = 0;
    virtual void Execute(LogWriter* logger) = 0;
};
