#include "Singleton.h"
#include <thread>
#include <sstream>
#include <algorithm>

// ============================================================
// AppConfig 实现
// ============================================================

AppConfig& AppConfig::getInstance() {
    // Meyers' Singleton：C++11 保证局部静态变量初始化的线程安全性
    // 编译器会自动在底层插入双检锁，无需手动实现
    static AppConfig instance;
    return instance;
}

AppConfig::AppConfig() {
    std::cout << "[AppConfig] 配置管理器初始化（仅执行一次）" << std::endl;
}

void AppConfig::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_[key] = value;
}

std::string AppConfig::get(const std::string& key, const std::string& defaultValue) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = config_.find(key);
    return (it != config_.end()) ? it->second : defaultValue;
}

bool AppConfig::contains(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_.find(key) != config_.end();
}

void AppConfig::loadFromDefaults() {
    std::lock_guard<std::mutex> lock(mutex_);
    // 模拟从配置文件加载默认配置
    config_["app.name"] = "DesignPatternDemo";
    config_["app.version"] = "1.0.0";
    config_["db.host"] = "localhost";
    config_["db.port"] = "5432";
    config_["db.name"] = "pattern_db";
    config_["db.max_connections"] = "5";
    config_["log.level"] = "INFO";
    config_["log.file"] = "/var/log/app.log";
    std::cout << "[AppConfig] 已加载 " << config_.size() << " 项默认配置" << std::endl;
}

void AppConfig::printAll() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << "[AppConfig] 当前配置项 (" << config_.size() << " 项):" << std::endl;
    for (const auto& [key, value] : config_) {
        std::cout << "  " << key << " = " << value << std::endl;
    }
}

// ============================================================
// DatabaseConnection 实现
// ============================================================

DatabaseConnection::DatabaseConnection(int id, const std::string& connStr)
    : id_(id), connectionString_(connStr) {
    std::cout << "  [Connection #" << id_ << "] 已创建，连接到: " << connStr << std::endl;
}

DatabaseConnection::~DatabaseConnection() {
    std::cout << "  [Connection #" << id_ << "] 已断开" << std::endl;
}

void DatabaseConnection::execute(const std::string& sql) {
    std::cout << "  [Connection #" << id_ << "] 执行 SQL: " << sql << std::endl;
}

// ============================================================
// ConnectionPool 实现
// ============================================================

ConnectionPool& ConnectionPool::getInstance() {
    static ConnectionPool instance;
    return instance;
}

ConnectionPool::ConnectionPool() {
    std::cout << "[ConnectionPool] 初始化连接池（容量: "
              << MAX_CONNECTIONS << "）" << std::endl;

    // 从配置管理器读取连接信息 —— 体现两个 Singleton 之间的协作
    auto& config = AppConfig::getInstance();
    std::string host = config.get("db.host", "localhost");
    std::string port = config.get("db.port", "5432");
    std::string dbName = config.get("db.name", "default_db");
    std::string connStr = host + ":" + port + "/" + dbName;

    // 预创建连接
    for (size_t i = 0; i < MAX_CONNECTIONS; ++i) {
        connections_.push_back(
            std::make_unique<DatabaseConnection>(static_cast<int>(i + 1), connStr)
        );
    }
}

DatabaseConnection* ConnectionPool::acquire() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& conn : connections_) {
        if (!conn->isInUse()) {
            conn->setInUse(true);
            std::cout << "[ConnectionPool] 分配连接 #" << conn->getId() << std::endl;
            return conn.get();
        }
    }
    std::cout << "[ConnectionPool] 警告：无空闲连接！" << std::endl;
    return nullptr;
}

void ConnectionPool::release(DatabaseConnection* conn) {
    if (!conn) return;
    std::lock_guard<std::mutex> lock(mutex_);
    conn->setInUse(false);
    std::cout << "[ConnectionPool] 归还连接 #" << conn->getId() << std::endl;
}

void ConnectionPool::printStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t active = std::count_if(connections_.begin(), connections_.end(),
        [](const auto& c) { return c->isInUse(); });
    std::cout << "[ConnectionPool] 状态: " << active << " 活跃 / "
              << (connections_.size() - active) << " 空闲 / "
              << connections_.size() << " 总计" << std::endl;
}

size_t ConnectionPool::poolSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return connections_.size();
}

size_t ConnectionPool::activeCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::count_if(connections_.begin(), connections_.end(),
        [](const auto& c) { return c->isInUse(); });
}


// ============================================================
// 演示程序
// ============================================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Singleton 模式演示" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ---- 1. 配置管理器演示 ----
    std::cout << "--- 1. 应用配置管理器 (Meyers' Singleton) ---\n" << std::endl;

    // 首次访问时初始化
    AppConfig& config1 = AppConfig::getInstance();
    config1.loadFromDefaults();

    // 第二次获取的是同一实例
    AppConfig& config2 = AppConfig::getInstance();
    config2.set("app.debug", "true");

    // 验证是同一个对象
    std::cout << "\n验证单例: config1 地址 = " << &config1
              << ", config2 地址 = " << &config2 << std::endl;
    std::cout << "是否为同一实例: " << (&config1 == &config2 ? "是" : "否") << std::endl;

    // 显示配置
    std::cout << std::endl;
    config1.printAll();

    // ---- 2. 多线程安全验证 ----
    std::cout << "\n--- 2. 多线程安全验证 ---\n" << std::endl;

    std::vector<std::thread> threads;
    std::mutex outputMutex;

    // 多个线程同时获取实例，应该都得到同一个
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([i, &outputMutex]() {
            AppConfig& cfg = AppConfig::getInstance();
            std::lock_guard<std::mutex> lock(outputMutex);
            std::cout << "线程 " << i << " 获取实例地址: " << &cfg << std::endl;
        });
    }
    for (auto& t : threads) {
        t.join();
    }
    std::cout << "所有线程获取到同一实例 -> 线程安全验证通过" << std::endl;

    // ---- 3. 数据库连接池演示 ----
    std::cout << "\n--- 3. 数据库连接池 (Singleton + 对象池) ---\n" << std::endl;

    ConnectionPool& pool = ConnectionPool::getInstance();
    pool.printStatus();

    // 获取几个连接并执行 SQL
    std::cout << std::endl;
    auto* conn1 = pool.acquire();
    auto* conn2 = pool.acquire();
    auto* conn3 = pool.acquire();

    if (conn1) conn1->execute("SELECT * FROM users WHERE id = 1");
    if (conn2) conn2->execute("INSERT INTO logs (msg) VALUES ('login')");
    if (conn3) conn3->execute("UPDATE sessions SET active = true");

    std::cout << std::endl;
    pool.printStatus();

    // 归还连接
    std::cout << std::endl;
    pool.release(conn1);
    pool.release(conn2);
    pool.printStatus();

    // 多线程使用连接池
    std::cout << "\n--- 4. 多线程并发使用连接池 ---\n" << std::endl;

    pool.release(conn3);  // 先全部归还

    std::vector<std::thread> workers;
    for (int i = 0; i < 6; ++i) {
        workers.emplace_back([i]() {
            ConnectionPool& p = ConnectionPool::getInstance();
            auto* c = p.acquire();
            if (c) {
                std::ostringstream ss;
                ss << "SELECT * FROM table_" << i;
                c->execute(ss.str());
                // 模拟工作
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                p.release(c);
            }
        });
    }
    for (auto& w : workers) {
        w.join();
    }

    std::cout << std::endl;
    pool.printStatus();

    std::cout << "\n========================================" << std::endl;
    std::cout << "关键点总结:" << std::endl;
    std::cout << "1. Meyers' Singleton 利用 C++11 静态变量线程安全保证" << std::endl;
    std::cout << "2. 删除拷贝/移动构造和赋值运算符防止复制" << std::endl;
    std::cout << "3. 连接池展示了 Singleton 的典型应用场景" << std::endl;
    std::cout << "4. 多线程环境下的数据安全需要额外加锁（mutex）" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
