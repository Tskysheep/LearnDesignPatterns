#ifndef STRATEGY_H
#define STRATEGY_H

#include <string>
#include <memory>
#include <map>
#include <functional>

// =============================================================================
// 策略模式 (Strategy Pattern) - 电商支付系统
// =============================================================================
// 核心思想：定义一系列算法（支付方式），把它们分别封装起来，并使它们可以互相替换。
// 策略模式让算法的变化独立于使用算法的客户端。
// =============================================================================

// 支付请求数据
struct PaymentRequest {
    std::string orderId;
    double amount;
    std::string currency;
    std::string customerEmail;

    // 支付方式特有的附加信息
    std::map<std::string, std::string> metadata;
};

// 支付结果
struct PaymentResult {
    bool success;
    std::string transactionId;
    std::string message;
    double processedAmount;  // 实际处理金额（可能含手续费）
    double fee;              // 手续费
};

// =============================================================================
// 策略接口：PaymentStrategy
// 所有支付方式都实现此接口，保证可以互相替换（里氏替换原则）
// =============================================================================
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() = default;

    // 执行支付
    virtual PaymentResult pay(const PaymentRequest& request) = 0;

    // 验证支付参数是否有效
    virtual bool validate(const PaymentRequest& request) const = 0;

    // 获取支付方式名称（用于日志和显示）
    virtual std::string getName() const = 0;
};

// =============================================================================
// 具体策略：信用卡支付
// =============================================================================
class CreditCardStrategy : public PaymentStrategy {
public:
    CreditCardStrategy(const std::string& cardNumber,
                       const std::string& holderName,
                       const std::string& expiry,
                       const std::string& cvv);

    PaymentResult pay(const PaymentRequest& request) override;
    bool validate(const PaymentRequest& request) const override;
    std::string getName() const override { return "信用卡支付"; }

private:
    std::string cardNumber_;
    std::string holderName_;
    std::string expiry_;
    std::string cvv_;

    // 信用卡手续费率
    static constexpr double FEE_RATE = 0.02;  // 2%

    // 简单的 Luhn 校验（模拟）
    bool luhnCheck(const std::string& number) const;
};

// =============================================================================
// 具体策略：支付宝支付
// =============================================================================
class AlipayStrategy : public PaymentStrategy {
public:
    explicit AlipayStrategy(const std::string& accountId);

    PaymentResult pay(const PaymentRequest& request) override;
    bool validate(const PaymentRequest& request) const override;
    std::string getName() const override { return "支付宝"; }

private:
    std::string accountId_;

    // 支付宝手续费率
    static constexpr double FEE_RATE = 0.006;  // 0.6%
    // 单笔限额
    static constexpr double MAX_AMOUNT = 50000.0;
};

// =============================================================================
// 具体策略：加密货币支付
// =============================================================================
class CryptoStrategy : public PaymentStrategy {
public:
    CryptoStrategy(const std::string& walletAddress,
                   const std::string& cryptoType = "BTC");

    PaymentResult pay(const PaymentRequest& request) override;
    bool validate(const PaymentRequest& request) const override;
    std::string getName() const override { return "加密货币 (" + cryptoType_ + ")"; }

private:
    std::string walletAddress_;
    std::string cryptoType_;

    // 网络矿工费（固定费用）
    static constexpr double NETWORK_FEE = 5.0;

    // 模拟汇率
    double getExchangeRate() const;
};

// =============================================================================
// 上下文：PaymentProcessor（订单支付处理器）
// 持有一个策略对象的引用，将支付请求委托给策略对象处理。
// 运行时可以动态切换策略（这是策略模式与模板方法的关键区别）。
// =============================================================================
class PaymentProcessor {
public:
    explicit PaymentProcessor(std::unique_ptr<PaymentStrategy> strategy = nullptr);

    // 运行时切换支付策略（这是模式的核心能力）
    void setStrategy(std::unique_ptr<PaymentStrategy> strategy);

    // 处理支付：委托给当前策略
    PaymentResult processPayment(const PaymentRequest& request);

    // 获取当前策略名称
    std::string getCurrentStrategyName() const;

private:
    std::unique_ptr<PaymentStrategy> strategy_;
};

// =============================================================================
// 工厂函数：根据支付方式名称创建策略（简化客户端代码）
// =============================================================================
std::unique_ptr<PaymentStrategy> createPaymentStrategy(
    const std::string& type,
    const std::map<std::string, std::string>& params);

#endif // STRATEGY_H
