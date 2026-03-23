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
