---
sidebar_position: 2
title: 源代码
description: Iterator 模式的完整实现代码
---

# 源代码

## 头文件（Iterator.h）

```cpp title="Iterator.h" showLineNumbers
#ifndef ITERATOR_H
#define ITERATOR_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <ctime>

// ============================================================
// 迭代器模式（Iterator Pattern）
// 场景：通知队列系统 —— 支持 FIFO、优先级、过滤等多种遍历方式，
//       将遍历逻辑与集合本身解耦。
// ============================================================

// 通知优先级
enum class Priority {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3
};

// 通知类型
enum class NotificationType {
    System,     // 系统通知
    Security,   // 安全告警
    Business,   // 业务通知
    Promotion   // 推广消息
};

// 通知实体
struct Notification {
    int id;
    std::string title;
    std::string message;
    Priority priority;
    NotificationType type;
    std::time_t timestamp;
    bool read;

    Notification(int id, const std::string& title, const std::string& message,
                 Priority priority, NotificationType type);

    // 辅助方法
    std::string getPriorityString() const;
    std::string getTypeString() const;
    std::string getTimeString() const;
};

// ============================================================
// 抽象迭代器接口
// 设计要点：提供统一的遍历协议，隐藏集合的内部表示
// ============================================================
template<typename T>
class Iterator {
public:
    virtual ~Iterator() = default;

    // 是否还有下一个元素
    virtual bool hasNext() const = 0;

    // 获取下一个元素
    virtual T& next() = 0;

    // 重置到起始位置
    virtual void reset() = 0;
};

// ============================================================
// 通知集合（Aggregate）
// 设计要点：提供创建不同迭代器的工厂方法
// ============================================================
class NotificationQueue {
public:
    using FilterPredicate = std::function<bool(const Notification&)>;

    void addNotification(const Notification& notification);
    void addNotification(const std::string& title, const std::string& message,
                         Priority priority, NotificationType type);

    size_t size() const { return notifications_.size(); }
    bool empty() const { return notifications_.empty(); }

    Notification& at(size_t index) { return notifications_[index]; }
    const Notification& at(size_t index) const { return notifications_[index]; }

    // 工厂方法：创建不同遍历策略的迭代器
    std::unique_ptr<Iterator<Notification>> createFifoIterator();
    std::unique_ptr<Iterator<Notification>> createPriorityIterator();
    std::unique_ptr<Iterator<Notification>> createFilterIterator(FilterPredicate predicate);
    std::unique_ptr<Iterator<Notification>> createUnreadIterator();

private:
    std::vector<Notification> notifications_;
    int nextId_ = 1;
};

// ============================================================
// 具体迭代器：FIFO 顺序迭代
// 按照添加顺序（先进先出）遍历
// ============================================================
class FifoIterator : public Iterator<Notification> {
public:
    explicit FifoIterator(NotificationQueue& queue);

    bool hasNext() const override;
    Notification& next() override;
    void reset() override;

private:
    NotificationQueue& queue_;
    size_t currentIndex_;
};

// ============================================================
// 具体迭代器：优先级迭代
// 按优先级从高到低遍历，同优先级按时间排序
// ============================================================
class PriorityIterator : public Iterator<Notification> {
public:
    explicit PriorityIterator(NotificationQueue& queue);

    bool hasNext() const override;
    Notification& next() override;
    void reset() override;

private:
    NotificationQueue& queue_;
    std::vector<size_t> sortedIndices_; // 按优先级排序后的索引
    size_t currentPos_;

    void buildSortedIndices();
};

// ============================================================
// 具体迭代器：过滤迭代
// 只遍历满足指定条件的元素
// ============================================================
class FilterIterator : public Iterator<Notification> {
public:
    using Predicate = std::function<bool(const Notification&)>;

    FilterIterator(NotificationQueue& queue, Predicate predicate);

    bool hasNext() const override;
    Notification& next() override;
    void reset() override;

private:
    NotificationQueue& queue_;
    Predicate predicate_;
    std::vector<size_t> matchedIndices_; // 满足条件的元素索引
    size_t currentPos_;

    void buildMatchedIndices();
};

#endif // ITERATOR_H
```

## 实现文件（Iterator.cpp）

```cpp title="Iterator.cpp" showLineNumbers
#include "Iterator.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdexcept>

// ============================================================
// Notification 实现
// ============================================================

Notification::Notification(int id, const std::string& title, const std::string& message,
                           Priority priority, NotificationType type)
    : id(id), title(title), message(message),
      priority(priority), type(type), timestamp(std::time(nullptr)), read(false) {}

std::string Notification::getPriorityString() const {
    switch (priority) {
        case Priority::Low:      return "Low";
        case Priority::Normal:   return "Normal";
        case Priority::High:     return "High";
        case Priority::Critical: return "Critical";
    }
    return "Unknown";
}

std::string Notification::getTypeString() const {
    switch (type) {
        case NotificationType::System:    return "System";
        case NotificationType::Security:  return "Security";
        case NotificationType::Business:  return "Business";
        case NotificationType::Promotion: return "Promotion";
    }
    return "Unknown";
}

std::string Notification::getTimeString() const {
    char buf[20];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&timestamp));
    return std::string(buf);
}

// ============================================================
// NotificationQueue 实现
// ============================================================

void NotificationQueue::addNotification(const Notification& notification) {
    notifications_.push_back(notification);
}

void NotificationQueue::addNotification(const std::string& title, const std::string& message,
                                         Priority priority, NotificationType type) {
    notifications_.emplace_back(nextId_++, title, message, priority, type);
}

// 工厂方法：创建不同迭代器
// 设计要点：将迭代器创建封装在集合类中，客户端无需知道具体迭代器类型
std::unique_ptr<Iterator<Notification>> NotificationQueue::createFifoIterator() {
    return std::make_unique<FifoIterator>(*this);
}

std::unique_ptr<Iterator<Notification>> NotificationQueue::createPriorityIterator() {
    return std::make_unique<PriorityIterator>(*this);
}

std::unique_ptr<Iterator<Notification>> NotificationQueue::createFilterIterator(FilterPredicate predicate) {
    return std::make_unique<FilterIterator>(*this, std::move(predicate));
}

std::unique_ptr<Iterator<Notification>> NotificationQueue::createUnreadIterator() {
    return createFilterIterator([](const Notification& n) { return !n.read; });
}

// ============================================================
// FifoIterator 实现 —— 最简单的顺序遍历
// ============================================================

FifoIterator::FifoIterator(NotificationQueue& queue)
    : queue_(queue), currentIndex_(0) {}

bool FifoIterator::hasNext() const {
    return currentIndex_ < queue_.size();
}

Notification& FifoIterator::next() {
    if (!hasNext()) {
        throw std::out_of_range("No more elements in FIFO iterator");
    }
    return queue_.at(currentIndex_++);
}

void FifoIterator::reset() {
    currentIndex_ = 0;
}

// ============================================================
// PriorityIterator 实现
// 设计要点：不改变原集合的顺序，而是维护一个排序后的索引数组
// ============================================================

PriorityIterator::PriorityIterator(NotificationQueue& queue)
    : queue_(queue), currentPos_(0) {
    buildSortedIndices();
}

bool PriorityIterator::hasNext() const {
    return currentPos_ < sortedIndices_.size();
}

Notification& PriorityIterator::next() {
    if (!hasNext()) {
        throw std::out_of_range("No more elements in Priority iterator");
    }
    return queue_.at(sortedIndices_[currentPos_++]);
}

void PriorityIterator::reset() {
    currentPos_ = 0;
    buildSortedIndices();
}

void PriorityIterator::buildSortedIndices() {
    sortedIndices_.clear();
    for (size_t i = 0; i < queue_.size(); ++i) {
        sortedIndices_.push_back(i);
    }
    // 按优先级降序排列；同优先级按 id 升序（即添加顺序）
    std::sort(sortedIndices_.begin(), sortedIndices_.end(),
        [this](size_t a, size_t b) {
            const auto& na = queue_.at(a);
            const auto& nb = queue_.at(b);
            if (na.priority != nb.priority) {
                return static_cast<int>(na.priority) > static_cast<int>(nb.priority);
            }
            return na.id < nb.id;
        });
}

// ============================================================
// FilterIterator 实现
// 设计要点：通过谓词函数实现灵活的过滤策略，支持任意条件组合
// ============================================================

FilterIterator::FilterIterator(NotificationQueue& queue, Predicate predicate)
    : queue_(queue), predicate_(std::move(predicate)), currentPos_(0) {
    buildMatchedIndices();
}

bool FilterIterator::hasNext() const {
    return currentPos_ < matchedIndices_.size();
}

Notification& FilterIterator::next() {
    if (!hasNext()) {
        throw std::out_of_range("No more elements in Filter iterator");
    }
    return queue_.at(matchedIndices_[currentPos_++]);
}

void FilterIterator::reset() {
    currentPos_ = 0;
    buildMatchedIndices();
}

void FilterIterator::buildMatchedIndices() {
    matchedIndices_.clear();
    for (size_t i = 0; i < queue_.size(); ++i) {
        if (predicate_(queue_.at(i))) {
            matchedIndices_.push_back(i);
        }
    }
}

// ============================================================
// 辅助函数：打印通知
// ============================================================
static void printNotification(const Notification& n) {
    std::cout << "    [#" << std::setw(2) << n.id << "] "
              << std::setw(8) << std::left << n.getPriorityString() << " | "
              << std::setw(9) << std::left << n.getTypeString() << " | "
              << (n.read ? "Read  " : "Unread") << " | "
              << n.title << std::endl;
}

static void printAllFromIterator(Iterator<Notification>& it, const std::string& label) {
    std::cout << "  " << label << ":" << std::endl;
    int count = 0;
    while (it.hasNext()) {
        printNotification(it.next());
        ++count;
    }
    if (count == 0) {
        std::cout << "    (empty)" << std::endl;
    }
    std::cout << "    --- Total: " << count << " notification(s) ---" << std::endl;
}

// ============================================================
// main() —— 演示迭代器模式
// ============================================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Iterator Pattern - Notification Queue" << std::endl;
    std::cout << "========================================" << std::endl;

    // --- 构建通知队列 ---
    NotificationQueue queue;

    queue.addNotification("System Update", "New version available",
                          Priority::Low, NotificationType::System);
    queue.addNotification("Login Alert", "Unusual login detected from new device",
                          Priority::Critical, NotificationType::Security);
    queue.addNotification("Order Shipped", "Order #12345 has been shipped",
                          Priority::Normal, NotificationType::Business);
    queue.addNotification("Flash Sale", "50% off on all electronics",
                          Priority::Low, NotificationType::Promotion);
    queue.addNotification("Password Expiry", "Your password expires in 3 days",
                          Priority::High, NotificationType::Security);
    queue.addNotification("Payment Received", "Payment of $299.00 confirmed",
                          Priority::Normal, NotificationType::Business);
    queue.addNotification("Server Down", "Production server unreachable",
                          Priority::Critical, NotificationType::System);
    queue.addNotification("New Feature", "Try our new dashboard",
                          Priority::Low, NotificationType::Promotion);

    std::cout << "\nTotal notifications: " << queue.size() << std::endl;

    // --- 1. FIFO 迭代器 ---
    std::cout << "\n[1] FIFO Iterator (insertion order):" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    auto fifo = queue.createFifoIterator();
    printAllFromIterator(*fifo, "All notifications (FIFO)");

    // --- 2. 优先级迭代器 ---
    // 核心价值：同一个集合，不同的遍历顺序，客户端代码完全一致
    std::cout << "\n[2] Priority Iterator (Critical -> Low):" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    auto priority = queue.createPriorityIterator();
    printAllFromIterator(*priority, "All notifications (by priority)");

    // --- 3. 过滤迭代器 —— 按类型过滤 ---
    std::cout << "\n[3] Filter Iterator (Security only):" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    auto securityFilter = queue.createFilterIterator(
        [](const Notification& n) { return n.type == NotificationType::Security; });
    printAllFromIterator(*securityFilter, "Security notifications");

    // --- 4. 过滤迭代器 —— 按优先级过滤 ---
    std::cout << "\n[4] Filter Iterator (High + Critical):" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    auto urgentFilter = queue.createFilterIterator(
        [](const Notification& n) {
            return n.priority == Priority::High || n.priority == Priority::Critical;
        });
    printAllFromIterator(*urgentFilter, "Urgent notifications");

    // --- 5. 标记部分为已读，然后用未读迭代器 ---
    std::cout << "\n[5] Unread Iterator (after marking some as read):" << std::endl;
    std::cout << "------------------------------------" << std::endl;

    // 标记前3条为已读
    auto markReader = queue.createFifoIterator();
    for (int i = 0; i < 3 && markReader->hasNext(); ++i) {
        markReader->next().read = true;
    }
    std::cout << "  (Marked first 3 notifications as read)" << std::endl;

    auto unread = queue.createUnreadIterator();
    printAllFromIterator(*unread, "Unread notifications");

    // --- 6. 组合过滤 —— 未读 + 高优先级 ---
    std::cout << "\n[6] Combined Filter (Unread + High Priority):" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    auto combinedFilter = queue.createFilterIterator(
        [](const Notification& n) {
            return !n.read &&
                   (n.priority == Priority::High || n.priority == Priority::Critical);
        });
    printAllFromIterator(*combinedFilter, "Unread urgent notifications");

    // --- 7. 迭代器重置演示 ---
    std::cout << "\n[7] Iterator Reset Demonstration:" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    auto reusable = queue.createPriorityIterator();

    std::cout << "  First pass (top 3):" << std::endl;
    for (int i = 0; i < 3 && reusable->hasNext(); ++i) {
        printNotification(reusable->next());
    }

    reusable->reset();
    std::cout << "  After reset (top 3 again):" << std::endl;
    for (int i = 0; i < 3 && reusable->hasNext(); ++i) {
        printNotification(reusable->next());
    }

    // --- 8. 统计演示 —— 利用迭代器做聚合 ---
    std::cout << "\n[8] Aggregation via Iterator:" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    auto countIter = queue.createFifoIterator();
    int readCount = 0, unreadCount = 0;
    std::map<std::string, int> typeCount;
    while (countIter->hasNext()) {
        auto& n = countIter->next();
        if (n.read) ++readCount; else ++unreadCount;
        typeCount[n.getTypeString()]++;
    }
    std::cout << "  Read: " << readCount << ", Unread: " << unreadCount << std::endl;
    std::cout << "  By type:" << std::endl;
    for (const auto& [type, count] : typeCount) {
        std::cout << "    " << type << ": " << count << std::endl;
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "   Iterator Pattern Demo Complete" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
```

## 构建方式

```cmake title="CMakeLists.txt"
cmake_minimum_required(VERSION 3.10)
project(Pattern_Iterator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Pattern_Iterator Iterator.cpp Iterator.h)
```

:::tip 编译运行
```bash
cd build
cmake --build . --target Pattern_Iterator
./Pattern_Iterator
```
:::
