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
