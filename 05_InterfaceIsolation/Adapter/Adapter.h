#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

// ============================================================
// Adapter 模式 - 第三方支付接口适配
// 核心思想：将一个类的接口转换成客户期望的另一个接口，
//          使得原本接口不兼容的类可以一起工作
// ============================================================

// -------------------- 支付结果 --------------------

// 统一的支付结果结构
struct PaymentResult {
    bool success = false;
    std::string transactionId;
    std::string message;
    double amount = 0.0;
    std::string currency = "CNY";
};

// -------------------- 目标接口（Target） --------------------

// 统一支付接口 —— 我们系统期望的标准接口
// 设计要点：这是"我们的规范"，所有支付方式都必须适配到这个接口
class UnifiedPayment {
public:
    virtual ~UnifiedPayment() = default;

    // 统一的支付方法
    virtual PaymentResult pay(const std::string& orderId,
                              double amount,
                              const std::string& currency) = 0;

    // 统一的退款方法
    virtual PaymentResult refund(const std::string& transactionId,
                                 double amount) = 0;

    // 统一的查询方法
    virtual PaymentResult queryStatus(const std::string& transactionId) = 0;

    // 获取支付渠道名称
    virtual std::string getChannelName() const = 0;
};

// -------------------- 第三方支付 SDK（Adaptee） --------------------

// 第三方支付 A：支付宝风格的 SDK
// 注意：接口与我们的 UnifiedPayment 完全不同
class AlipaySDK {
public:
    // 支付宝用 map 传参数，返回 map
    std::unordered_map<std::string, std::string> sendPayRequest(
        const std::unordered_map<std::string, std::string>& params);

    // 退款接口也是 map 风格
    std::unordered_map<std::string, std::string> sendRefundRequest(
        const std::unordered_map<std::string, std::string>& params);

    // 查询接口
    std::unordered_map<std::string, std::string> queryTransaction(
        const std::string& tradeNo);
};

// 第三方支付 B：微信支付风格的 SDK
// 注意：微信支付用 XML 风格的字符串接口（这里简化为参数列表）
class WechatPaySDK {
public:
    // 微信支付用分为单位（而非元），且参数命名完全不同
    struct WxPayResponse {
        std::string returnCode;   // SUCCESS 或 FAIL
        std::string resultCode;
        std::string prepayId;
        std::string errCodeDes;
    };

    WxPayResponse unifiedOrder(const std::string& outTradeNo,
                                int totalFee,  // 单位：分
                                const std::string& body,
                                const std::string& feeType);

    WxPayResponse refundOrder(const std::string& transactionId,
                               int refundFee,   // 单位：分
                               int totalFee);

    WxPayResponse orderQuery(const std::string& transactionId);
};

// 第三方支付 C：Stripe 风格的 SDK（国际支付）
// 注意：Stripe 用英文接口，金额用最小货币单位（美分），异常机制不同
class StripeSDK {
public:
    struct Charge {
        std::string id;
        int amountInCents;   // 金额（美分）
        std::string currency;
        std::string status;  // "succeeded", "failed", "pending"
        std::string failureMessage;
    };

    // Stripe 的接口风格：创建 Charge 对象
    // 可能抛出异常表示错误
    Charge createCharge(int amountInCents,
                        const std::string& currency,
                        const std::string& description);

    Charge createRefund(const std::string& chargeId,
                         int amountInCents);

    Charge retrieveCharge(const std::string& chargeId);
};

// -------------------- 适配器（Adapter） --------------------

// 支付宝适配器 —— 将 AlipaySDK 适配为 UnifiedPayment
// 设计决策：使用对象适配器（组合），而非类适配器（多重继承）
//          因为 C++ 多重继承容易引发菱形继承等问题
class AlipayAdapter : public UnifiedPayment {
public:
    AlipayAdapter();

    PaymentResult pay(const std::string& orderId,
                      double amount,
                      const std::string& currency) override;

    PaymentResult refund(const std::string& transactionId,
                         double amount) override;

    PaymentResult queryStatus(const std::string& transactionId) override;

    std::string getChannelName() const override { return "支付宝"; }

private:
    // 对象适配器：通过组合持有被适配对象
    std::unique_ptr<AlipaySDK> sdk_;
};

// 微信支付适配器
class WechatPayAdapter : public UnifiedPayment {
public:
    WechatPayAdapter();

    PaymentResult pay(const std::string& orderId,
                      double amount,
                      const std::string& currency) override;

    PaymentResult refund(const std::string& transactionId,
                         double amount) override;

    PaymentResult queryStatus(const std::string& transactionId) override;

    std::string getChannelName() const override { return "微信支付"; }

private:
    std::unique_ptr<WechatPaySDK> sdk_;

    // 辅助方法：元转分
    static int yuanToFen(double yuan);
};

// Stripe 适配器
class StripeAdapter : public UnifiedPayment {
public:
    StripeAdapter();

    PaymentResult pay(const std::string& orderId,
                      double amount,
                      const std::string& currency) override;

    PaymentResult refund(const std::string& transactionId,
                         double amount) override;

    PaymentResult queryStatus(const std::string& transactionId) override;

    std::string getChannelName() const override { return "Stripe"; }

private:
    std::unique_ptr<StripeSDK> sdk_;

    // 辅助方法：元/美元转换为美分
    static int toCents(double amount);
};

// -------------------- 支付工厂（辅助类） --------------------

// 支付渠道工厂 —— 根据渠道名称创建对应的适配器
class PaymentFactory {
public:
    static std::unique_ptr<UnifiedPayment> create(const std::string& channel);
};
