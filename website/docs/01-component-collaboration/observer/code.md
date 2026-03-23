---
sidebar_position: 2
title: 源代码
description: Observer 模式的完整实现代码
---

# 源代码

## 头文件（Observer.h）

```cpp title="Observer.h" showLineNumbers
#ifndef OBSERVER_H
#define OBSERVER_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <algorithm>
#include <chrono>
#include <ctime>

// =============================================================================
// 观察者模式 (Observer Pattern) - 股票交易系统
// =============================================================================
// 核心思想：定义对象间的一种一对多依赖关系，当一个对象（Subject）的状态发生改变时，
// 所有依赖于它的对象（Observers）都会自动收到通知并更新。
// =============================================================================

// 股票价格数据
struct StockPrice {
    std::string symbol;       // 股票代码
    std::string name;         // 股票名称
    double currentPrice;      // 当前价格
    double previousPrice;     // 前一价格
    double changePercent;     // 涨跌幅 (%)
    std::string timestamp;    // 时间戳

    // 计算涨跌幅
    void updateChange() {
        if (previousPrice > 0) {
            changePercent = ((currentPrice - previousPrice) / previousPrice) * 100.0;
        } else {
            changePercent = 0.0;
        }
    }
};

// =============================================================================
// 观察者接口：StockObserver
// 所有关注股价变动的组件都实现此接口
// =============================================================================
class StockObserver {
public:
    virtual ~StockObserver() = default;

    // 当股价变动时被调用（推模型：Subject 主动推送数据给 Observer）
    virtual void onPriceUpdate(const StockPrice& price) = 0;

    // 获取观察者名称（用于日志和管理）
    virtual std::string getObserverName() const = 0;
};

// =============================================================================
// 被观察者（Subject）：StockExchange（股票交易所）
// 维护观察者列表，当股价变化时通知所有已注册的观察者。
// =============================================================================
class StockExchange {
public:
    explicit StockExchange(const std::string& name);
    ~StockExchange() = default;

    // 注册观察者（订阅）
    // 使用 shared_ptr 管理观察者生命周期，避免悬垂指针问题
    void subscribe(std::shared_ptr<StockObserver> observer);

    // 针对特定股票注册观察者（细粒度订阅）
    void subscribe(const std::string& symbol, std::shared_ptr<StockObserver> observer);

    // 移除观察者（取消订阅）
    void unsubscribe(std::shared_ptr<StockObserver> observer);

    // 更新股价（触发通知）
    void updatePrice(const std::string& symbol, const std::string& name, double newPrice);

    // 获取当前股价
    StockPrice getPrice(const std::string& symbol) const;

    // 获取交易所名称
    std::string getName() const { return name_; }

private:
    // 通知所有观察者（核心方法）
    void notifyObservers(const StockPrice& price);

    std::string name_;

    // 全局观察者列表（关注所有股票）
    std::vector<std::shared_ptr<StockObserver>> globalObservers_;

    // 针对特定股票的观察者列表（细粒度订阅）
    std::map<std::string, std::vector<std::shared_ptr<StockObserver>>> symbolObservers_;

    // 当前所有股票的价格
    std::map<std::string, StockPrice> prices_;
};

// =============================================================================
// 具体观察者1：PriceDisplay - 实时行情显示面板
// 收到股价变动通知后更新显示
// =============================================================================
class PriceDisplay : public StockObserver {
public:
    explicit PriceDisplay(const std::string& displayName);

    void onPriceUpdate(const StockPrice& price) override;
    std::string getObserverName() const override;

    // 显示当前面板上的所有股票
    void showDashboard() const;

private:
    std::string displayName_;
    // 缓存最近的价格信息用于面板显示
    std::map<std::string, StockPrice> cachedPrices_;
};

// =============================================================================
// 具体观察者2：PriceAlertSystem - 价格预警系统
// 当股价触达预设的阈值时发出警报
// =============================================================================
class PriceAlertSystem : public StockObserver {
public:
    PriceAlertSystem();

    void onPriceUpdate(const StockPrice& price) override;
    std::string getObserverName() const override { return "价格预警系统"; }

    // 设置价格上限预警
    void setUpperAlert(const std::string& symbol, double price);

    // 设置价格下限预警
    void setLowerAlert(const std::string& symbol, double price);

    // 设置涨跌幅预警（百分比）
    void setChangeAlert(const std::string& symbol, double percentThreshold);

private:
    struct AlertRule {
        double upperLimit = 0.0;
        double lowerLimit = 0.0;
        double changeThreshold = 0.0;  // 涨跌幅阈值 (%)
        bool hasUpper = false;
        bool hasLower = false;
        bool hasChangeAlert = false;
    };

    std::map<std::string, AlertRule> alertRules_;
    int alertCount_ = 0;
};

// =============================================================================
// 具体观察者3：TradeLogger - 交易日志记录器
// 记录所有股价变动到日志
// =============================================================================
class TradeLogger : public StockObserver {
public:
    explicit TradeLogger(const std::string& logPrefix = "TRADE_LOG");

    void onPriceUpdate(const StockPrice& price) override;
    std::string getObserverName() const override { return "交易日志 [" + logPrefix_ + "]"; }

    // 输出日志统计信息
    void printStatistics() const;

private:
    std::string logPrefix_;
    int totalUpdates_ = 0;
    int priceUpCount_ = 0;
    int priceDownCount_ = 0;
    int priceUnchangedCount_ = 0;
};

// =============================================================================
// 具体观察者4：MovingAverageCalculator - 移动均线计算器
// 计算并显示简单移动平均线（SMA）
// =============================================================================
class MovingAverageCalculator : public StockObserver {
public:
    explicit MovingAverageCalculator(int period = 5);

    void onPriceUpdate(const StockPrice& price) override;
    std::string getObserverName() const override;

    double getMovingAverage(const std::string& symbol) const;

private:
    int period_;
    // 每只股票的历史价格窗口
    std::map<std::string, std::vector<double>> priceWindows_;
};

#endif // OBSERVER_H
```

## 实现文件（Observer.cpp）

```cpp title="Observer.cpp" showLineNumbers
#include "Observer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <numeric>

// =============================================================================
// StockExchange（被观察者/Subject）实现
// =============================================================================

StockExchange::StockExchange(const std::string& name) : name_(name) {}

// 注册全局观察者（关注所有股票）
void StockExchange::subscribe(std::shared_ptr<StockObserver> observer) {
    globalObservers_.push_back(observer);
    std::cout << "[" << name_ << "] 观察者 \"" << observer->getObserverName()
              << "\" 已订阅所有股票行情" << std::endl;
}

// 注册针对特定股票的观察者（细粒度订阅）
void StockExchange::subscribe(const std::string& symbol,
                              std::shared_ptr<StockObserver> observer) {
    symbolObservers_[symbol].push_back(observer);
    std::cout << "[" << name_ << "] 观察者 \"" << observer->getObserverName()
              << "\" 已订阅 " << symbol << " 行情" << std::endl;
}

// 移除观察者
void StockExchange::unsubscribe(std::shared_ptr<StockObserver> observer) {
    // 从全局列表中移除
    globalObservers_.erase(
        std::remove(globalObservers_.begin(), globalObservers_.end(), observer),
        globalObservers_.end()
    );

    // 从所有股票特定列表中移除
    for (auto& [symbol, observers] : symbolObservers_) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    std::cout << "[" << name_ << "] 观察者 \"" << observer->getObserverName()
              << "\" 已取消订阅" << std::endl;
}

// 更新股价并通知观察者
void StockExchange::updatePrice(const std::string& symbol,
                                const std::string& name,
                                double newPrice) {
    StockPrice& price = prices_[symbol];
    price.previousPrice = price.currentPrice;
    price.currentPrice = newPrice;
    price.symbol = symbol;
    price.name = name;
    price.updateChange();

    // 生成时间戳
    static int timeStep = 0;
    price.timestamp = "2024-01-15 09:" + std::to_string(30 + timeStep) + ":00";
    ++timeStep;

    std::cout << "\n>>> [" << name_ << "] 股价更新: " << symbol << " (" << name
              << ") -> " << std::fixed << std::setprecision(2) << newPrice << std::endl;

    // 通知所有相关观察者（模式的核心步骤）
    notifyObservers(price);
}

// 通知观察者
void StockExchange::notifyObservers(const StockPrice& price) {
    // 通知全局观察者
    for (const auto& observer : globalObservers_) {
        observer->onPriceUpdate(price);
    }

    // 通知订阅了该股票的特定观察者
    auto it = symbolObservers_.find(price.symbol);
    if (it != symbolObservers_.end()) {
        for (const auto& observer : it->second) {
            // 避免重复通知（如果观察者同时在全局列表中）
            bool alreadyNotified = std::find(globalObservers_.begin(),
                                              globalObservers_.end(),
                                              observer) != globalObservers_.end();
            if (!alreadyNotified) {
                observer->onPriceUpdate(price);
            }
        }
    }
}

StockPrice StockExchange::getPrice(const std::string& symbol) const {
    auto it = prices_.find(symbol);
    if (it != prices_.end()) return it->second;
    return {};
}

// =============================================================================
// PriceDisplay（行情显示面板）实现
// =============================================================================

PriceDisplay::PriceDisplay(const std::string& displayName)
    : displayName_(displayName) {}

void PriceDisplay::onPriceUpdate(const StockPrice& price) {
    cachedPrices_[price.symbol] = price;

    std::cout << "  [" << displayName_ << "] 显示更新: "
              << price.symbol << " " << price.name << " | "
              << std::fixed << std::setprecision(2) << price.currentPrice;

    if (price.previousPrice > 0) {
        std::cout << " | 涨跌: " << std::showpos << price.changePercent
                  << "%" << std::noshowpos;
    }
    std::cout << std::endl;
}

std::string PriceDisplay::getObserverName() const {
    return "行情面板 [" + displayName_ + "]";
}

void PriceDisplay::showDashboard() const {
    std::cout << "\n========== " << displayName_ << " 行情面板 ==========" << std::endl;
    std::cout << std::left << std::setw(8) << "代码"
              << std::setw(12) << "名称"
              << std::right << std::setw(10) << "当前价"
              << std::setw(10) << "涨跌幅" << std::endl;
    std::cout << std::string(40, '-') << std::endl;

    for (const auto& [symbol, price] : cachedPrices_) {
        std::cout << std::left << std::setw(8) << price.symbol
                  << std::setw(12) << price.name
                  << std::right << std::fixed << std::setprecision(2)
                  << std::setw(10) << price.currentPrice
                  << std::setw(9) << std::showpos << price.changePercent
                  << "%" << std::noshowpos << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
}

// =============================================================================
// PriceAlertSystem（价格预警系统）实现
// =============================================================================

PriceAlertSystem::PriceAlertSystem() {}

void PriceAlertSystem::setUpperAlert(const std::string& symbol, double price) {
    alertRules_[symbol].upperLimit = price;
    alertRules_[symbol].hasUpper = true;
    std::cout << "  [预警] 设置 " << symbol << " 上限预警: "
              << std::fixed << std::setprecision(2) << price << std::endl;
}

void PriceAlertSystem::setLowerAlert(const std::string& symbol, double price) {
    alertRules_[symbol].lowerLimit = price;
    alertRules_[symbol].hasLower = true;
    std::cout << "  [预警] 设置 " << symbol << " 下限预警: "
              << std::fixed << std::setprecision(2) << price << std::endl;
}

void PriceAlertSystem::setChangeAlert(const std::string& symbol, double percentThreshold) {
    alertRules_[symbol].changeThreshold = percentThreshold;
    alertRules_[symbol].hasChangeAlert = true;
    std::cout << "  [预警] 设置 " << symbol << " 涨跌幅预警: "
              << std::fixed << std::setprecision(1) << percentThreshold << "%" << std::endl;
}

void PriceAlertSystem::onPriceUpdate(const StockPrice& price) {
    auto it = alertRules_.find(price.symbol);
    if (it == alertRules_.end()) return;

    const AlertRule& rule = it->second;

    // 检查上限预警
    if (rule.hasUpper && price.currentPrice >= rule.upperLimit) {
        ++alertCount_;
        std::cout << "  *** [预警 #" << alertCount_ << "] " << price.symbol
                  << " 触及上限! 当前价: " << std::fixed << std::setprecision(2)
                  << price.currentPrice << " >= 上限: " << rule.upperLimit
                  << " ***" << std::endl;
    }

    // 检查下限预警
    if (rule.hasLower && price.currentPrice <= rule.lowerLimit) {
        ++alertCount_;
        std::cout << "  *** [预警 #" << alertCount_ << "] " << price.symbol
                  << " 跌破下限! 当前价: " << std::fixed << std::setprecision(2)
                  << price.currentPrice << " <= 下限: " << rule.lowerLimit
                  << " ***" << std::endl;
    }

    // 检查涨跌幅预警
    if (rule.hasChangeAlert) {
        double absChange = (price.changePercent >= 0) ? price.changePercent : -price.changePercent;
        if (absChange >= rule.changeThreshold) {
            ++alertCount_;
            std::cout << "  *** [预警 #" << alertCount_ << "] " << price.symbol
                      << " 涨跌幅异常! " << std::showpos << std::fixed
                      << std::setprecision(2) << price.changePercent << "%"
                      << std::noshowpos << " (阈值: " << rule.changeThreshold
                      << "%) ***" << std::endl;
        }
    }
}

// =============================================================================
// TradeLogger（交易日志记录器）实现
// =============================================================================

TradeLogger::TradeLogger(const std::string& logPrefix)
    : logPrefix_(logPrefix) {}

void TradeLogger::onPriceUpdate(const StockPrice& price) {
    ++totalUpdates_;
    if (price.changePercent > 0) ++priceUpCount_;
    else if (price.changePercent < 0) ++priceDownCount_;
    else ++priceUnchangedCount_;

    std::cout << "  [" << logPrefix_ << "] " << price.timestamp << " | "
              << price.symbol << " | "
              << std::fixed << std::setprecision(2) << price.previousPrice
              << " -> " << price.currentPrice
              << " | " << std::showpos << price.changePercent << "%"
              << std::noshowpos << std::endl;
}

void TradeLogger::printStatistics() const {
    std::cout << "\n--- " << logPrefix_ << " 统计信息 ---" << std::endl;
    std::cout << "总更新次数: " << totalUpdates_ << std::endl;
    std::cout << "上涨次数:   " << priceUpCount_ << std::endl;
    std::cout << "下跌次数:   " << priceDownCount_ << std::endl;
    std::cout << "持平次数:   " << priceUnchangedCount_ << std::endl;
    std::cout << "----------------------------\n" << std::endl;
}

// =============================================================================
// MovingAverageCalculator（移动均线计算器）实现
// =============================================================================

MovingAverageCalculator::MovingAverageCalculator(int period) : period_(period) {}

void MovingAverageCalculator::onPriceUpdate(const StockPrice& price) {
    auto& window = priceWindows_[price.symbol];
    window.push_back(price.currentPrice);

    // 保持窗口大小不超过 period
    if (static_cast<int>(window.size()) > period_) {
        window.erase(window.begin());
    }

    double ma = getMovingAverage(price.symbol);
    std::cout << "  [MA" << period_ << "] " << price.symbol
              << " 移动均线: " << std::fixed << std::setprecision(2) << ma
              << " (数据点: " << window.size() << "/" << period_ << ")";

    // 当价格偏离均线较大时给出提示
    if (static_cast<int>(window.size()) >= period_) {
        double deviation = ((price.currentPrice - ma) / ma) * 100.0;
        if (deviation > 2.0) {
            std::cout << " | 价格高于均线 " << std::fixed << std::setprecision(1)
                      << deviation << "%";
        } else if (deviation < -2.0) {
            std::cout << " | 价格低于均线 " << std::fixed << std::setprecision(1)
                      << deviation << "%";
        }
    }
    std::cout << std::endl;
}

std::string MovingAverageCalculator::getObserverName() const {
    return "移动均线 (MA" + std::to_string(period_) + ")";
}

double MovingAverageCalculator::getMovingAverage(const std::string& symbol) const {
    auto it = priceWindows_.find(symbol);
    if (it == priceWindows_.end() || it->second.empty()) return 0.0;
    const auto& window = it->second;
    return std::accumulate(window.begin(), window.end(), 0.0) / window.size();
}

// =============================================================================
// 主函数：演示观察者模式
// =============================================================================
int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "    观察者模式 (Observer Pattern) 演示" << std::endl;
    std::cout << "    场景：股票交易系统" << std::endl;
    std::cout << "==================================================" << std::endl;

    // 创建被观察者：交易所
    StockExchange exchange("沪深交易所");

    // 创建观察者
    auto mainDisplay = std::make_shared<PriceDisplay>("主屏幕");
    auto mobileDisplay = std::make_shared<PriceDisplay>("手机 App");
    auto alertSystem = std::make_shared<PriceAlertSystem>();
    auto logger = std::make_shared<TradeLogger>("MAIN_LOG");
    auto ma5 = std::make_shared<MovingAverageCalculator>(3);  // 3 期均线（演示方便）

    // ========================================
    // 演示1：注册观察者
    // ========================================
    std::cout << "\n【演示 1】注册观察者" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // 全局订阅：关注所有股票
    exchange.subscribe(mainDisplay);
    exchange.subscribe(logger);

    // 细粒度订阅：只关注特定股票
    exchange.subscribe("SH600519", mobileDisplay);
    exchange.subscribe("SH600519", ma5);

    // 设置预警规则
    std::cout << "\n设置预警规则:" << std::endl;
    alertSystem->setUpperAlert("SH600519", 1850.0);
    alertSystem->setLowerAlert("SH600519", 1750.0);
    alertSystem->setChangeAlert("SZ000858", 3.0);
    exchange.subscribe(alertSystem);

    // ========================================
    // 演示2：股价更新 → 自动通知观察者
    // ========================================
    std::cout << "\n【演示 2】股价更新 - 观察者自动收到通知" << std::endl;
    std::cout << "========================================" << std::endl;

    // 模拟一系列股价变动
    exchange.updatePrice("SH600519", "贵州茅台", 1800.00);
    exchange.updatePrice("SZ000858", "五粮液", 165.50);
    exchange.updatePrice("SH600519", "贵州茅台", 1830.00);
    exchange.updatePrice("SH601318", "中国平安", 48.20);

    // ========================================
    // 演示3：触发价格预警
    // ========================================
    std::cout << "\n【演示 3】触发价格预警" << std::endl;
    std::cout << "========================================" << std::endl;

    // 茅台突破上限预警
    exchange.updatePrice("SH600519", "贵州茅台", 1860.00);

    // 五粮液大幅波动触发涨跌幅预警
    exchange.updatePrice("SZ000858", "五粮液", 172.00);

    // ========================================
    // 演示4：动态取消订阅
    // 观察者模式支持运行时添加/移除观察者
    // ========================================
    std::cout << "\n【演示 4】动态取消订阅" << std::endl;
    std::cout << "========================================" << std::endl;

    exchange.unsubscribe(mobileDisplay);

    // 取消订阅后，mobileDisplay 不再收到通知
    exchange.updatePrice("SH600519", "贵州茅台", 1845.00);

    // ========================================
    // 演示5：显示面板和统计信息
    // ========================================
    std::cout << "\n【演示 5】查看面板和统计" << std::endl;
    std::cout << "========================================" << std::endl;

    mainDisplay->showDashboard();
    logger->printStatistics();

    // 关键总结
    std::cout << "==================================================" << std::endl;
    std::cout << "观察者模式要点总结：" << std::endl;
    std::cout << "1. StockExchange(Subject) 维护观察者列表" << std::endl;
    std::cout << "2. 股价变化时自动通知所有已注册的观察者" << std::endl;
    std::cout << "3. 观察者之间彼此独立，互不影响" << std::endl;
    std::cout << "4. 支持运行时动态添加/移除观察者" << std::endl;
    std::cout << "5. 支持全局订阅和细粒度（按股票代码）订阅" << std::endl;
    std::cout << "==================================================" << std::endl;

    return 0;
}
```

## 构建方式

```cmake title="CMakeLists.txt"
add_executable(Pattern_Observer Observer.cpp Observer.h)
target_compile_features(Pattern_Observer PRIVATE cxx_std_17)
```

:::tip 编译运行
```bash
cd build
cmake --build . --target Pattern_Observer
./Pattern_Observer  # or .\Release\Pattern_Observer.exe on Windows
```
:::
