#pragma once

#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <vector>
#include <iostream>

// ============================================================
// Singleton 模式 —— 应用配置管理器 & 数据库连接池
// 确保一个类仅有一个实例，并提供全局访问点
// ============================================================

// ---------- 场景 1：应用配置管理器 ----------
// 使用 Meyers' Singleton —— 利用 C++11 静态局部变量的线程安全保证
class AppConfig {
public:
    // 获取唯一实例（Meyers' Singleton）
    // C++11 标准保证：局部静态变量的初始化是线程安全的
    static AppConfig& getInstance();

    // 禁止拷贝和移动，防止产生第二个实例
    AppConfig(const AppConfig&) = delete;
    AppConfig& operator=(const AppConfig&) = delete;
    AppConfig(AppConfig&&) = delete;
    AppConfig& operator=(AppConfig&&) = delete;

    // 配置读写接口
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key, const std::string& defaultValue = "") const;
    bool contains(const std::string& key) const;
    void loadFromDefaults();
    void printAll() const;

private:
    // 构造函数私有化 —— Singleton 的核心约束
    AppConfig();
    ~AppConfig() = default;

    // 使用 mutable mutex 允许在 const 方法中加锁
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::string> config_;
};


// ---------- 场景 2：数据库连接池 ----------
// 模拟数据库连接
class DatabaseConnection {
public:
    explicit DatabaseConnection(int id, const std::string& connStr);
    ~DatabaseConnection();

    int getId() const { return id_; }
    bool isInUse() const { return inUse_; }
    void setInUse(bool inUse) { inUse_ = inUse; }
    void execute(const std::string& sql);

private:
    int id_;
    std::string connectionString_;
    bool inUse_ = false;
};

// 数据库连接池 —— Singleton + 对象池
// 全局只有一个连接池实例，管理有限数量的数据库连接
class ConnectionPool {
public:
    static ConnectionPool& getInstance();

    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;

    // 获取一个空闲连接
    DatabaseConnection* acquire();
    // 归还连接
    void release(DatabaseConnection* conn);
    // 查看连接池状态
    void printStatus() const;
    size_t poolSize() const;
    size_t activeCount() const;

private:
    ConnectionPool();
    ~ConnectionPool() = default;

    static constexpr size_t MAX_CONNECTIONS = 5;
    mutable std::mutex mutex_;
    std::vector<std::unique_ptr<DatabaseConnection>> connections_;
};
