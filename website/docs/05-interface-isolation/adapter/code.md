---
sidebar_position: 2
title: 源代码
description: Adapter 模式的完整实现代码
---

# 源代码

## 头文件（Adapter.h）

```cpp title="Adapter.h" showLineNumbers
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
```

## 实现文件（Adapter.cpp）

```cpp title="Adapter.cpp" showLineNumbers
#include "Adapter.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <iomanip>

// ============================================================
// 第三方 SDK 实现（模拟真实 SDK 的行为）
// ============================================================

// -------------------- AlipaySDK --------------------
std::unordered_map<std::string, std::string> AlipaySDK::sendPayRequest(
    const std::unordered_map<std::string, std::string>& params) {
    std::cout << "    [支付宝SDK] 发送支付请求..." << std::endl;
    std::cout << "    [支付宝SDK] 参数: out_trade_no="
              << params.at("out_trade_no")
              << ", total_amount=" << params.at("total_amount") << std::endl;

    std::unordered_map<std::string, std::string> response;
    response["code"] = "10000";  // 支付宝的成功码
    response["msg"] = "Success";
    response["trade_no"] = "ALI" + params.at("out_trade_no") + "20260321";
    response["total_amount"] = params.at("total_amount");
    return response;
}

std::unordered_map<std::string, std::string> AlipaySDK::sendRefundRequest(
    const std::unordered_map<std::string, std::string>& params) {
    std::cout << "    [支付宝SDK] 发送退款请求..." << std::endl;

    std::unordered_map<std::string, std::string> response;
    response["code"] = "10000";
    response["msg"] = "Success";
    response["trade_no"] = params.at("trade_no");
    response["refund_fee"] = params.at("refund_amount");
    return response;
}

std::unordered_map<std::string, std::string> AlipaySDK::queryTransaction(
    const std::string& tradeNo) {
    std::cout << "    [支付宝SDK] 查询交易: " << tradeNo << std::endl;

    std::unordered_map<std::string, std::string> response;
    response["code"] = "10000";
    response["trade_status"] = "TRADE_SUCCESS";
    response["trade_no"] = tradeNo;
    response["total_amount"] = "99.90";
    return response;
}

// -------------------- WechatPaySDK --------------------
WechatPaySDK::WxPayResponse WechatPaySDK::unifiedOrder(
    const std::string& outTradeNo, int totalFee,
    const std::string& body, const std::string& feeType) {
    std::cout << "    [微信SDK] 统一下单..." << std::endl;
    std::cout << "    [微信SDK] 参数: out_trade_no=" << outTradeNo
              << ", total_fee=" << totalFee << "分"
              << ", fee_type=" << feeType << std::endl;

    WxPayResponse resp;
    resp.returnCode = "SUCCESS";
    resp.resultCode = "SUCCESS";
    resp.prepayId = "WX" + outTradeNo + "PREPAY";
    return resp;
}

WechatPaySDK::WxPayResponse WechatPaySDK::refundOrder(
    const std::string& transactionId, int refundFee, int totalFee) {
    std::cout << "    [微信SDK] 申请退款: " << transactionId
              << ", 退款金额: " << refundFee << "分" << std::endl;

    WxPayResponse resp;
    resp.returnCode = "SUCCESS";
    resp.resultCode = "SUCCESS";
    resp.prepayId = transactionId;
    return resp;
}

WechatPaySDK::WxPayResponse WechatPaySDK::orderQuery(
    const std::string& transactionId) {
    std::cout << "    [微信SDK] 查询订单: " << transactionId << std::endl;

    WxPayResponse resp;
    resp.returnCode = "SUCCESS";
    resp.resultCode = "SUCCESS";
    resp.prepayId = transactionId;
    return resp;
}

// -------------------- StripeSDK --------------------
StripeSDK::Charge StripeSDK::createCharge(
    int amountInCents, const std::string& currency,
    const std::string& description) {
    std::cout << "    [StripeSDK] Creating charge..." << std::endl;
    std::cout << "    [StripeSDK] amount=" << amountInCents
              << " cents, currency=" << currency << std::endl;

    Charge charge;
    charge.id = "ch_stripe_" + std::to_string(amountInCents) + "_001";
    charge.amountInCents = amountInCents;
    charge.currency = currency;
    charge.status = "succeeded";
    return charge;
}

StripeSDK::Charge StripeSDK::createRefund(
    const std::string& chargeId, int amountInCents) {
    std::cout << "    [StripeSDK] Creating refund for: " << chargeId << std::endl;

    Charge charge;
    charge.id = "re_" + chargeId;
    charge.amountInCents = amountInCents;
    charge.status = "succeeded";
    return charge;
}

StripeSDK::Charge StripeSDK::retrieveCharge(const std::string& chargeId) {
    std::cout << "    [StripeSDK] Retrieving charge: " << chargeId << std::endl;

    Charge charge;
    charge.id = chargeId;
    charge.amountInCents = 1999;
    charge.currency = "usd";
    charge.status = "succeeded";
    return charge;
}

// ============================================================
// 适配器实现 —— 核心：将不兼容的接口转换为统一接口
// ============================================================

// -------------------- AlipayAdapter --------------------

AlipayAdapter::AlipayAdapter()
    : sdk_(std::make_unique<AlipaySDK>()) {}

PaymentResult AlipayAdapter::pay(const std::string& orderId,
                                  double amount,
                                  const std::string& currency) {
    // 适配：将统一接口的参数转换为支付宝 SDK 的 map 格式
    std::unordered_map<std::string, std::string> params;
    params["out_trade_no"] = orderId;

    // 金额格式转换：double → 字符串（保留2位小数）
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount;
    params["total_amount"] = oss.str();
    params["product_code"] = "FAST_INSTANT_TRADE_PAY";

    // 调用被适配对象
    auto response = sdk_->sendPayRequest(params);

    // 适配：将支付宝的 map 响应转换为统一的 PaymentResult
    PaymentResult result;
    result.success = (response["code"] == "10000");
    result.transactionId = response["trade_no"];
    result.message = response["msg"];
    result.amount = amount;
    result.currency = currency;
    return result;
}

PaymentResult AlipayAdapter::refund(const std::string& transactionId,
                                     double amount) {
    std::unordered_map<std::string, std::string> params;
    params["trade_no"] = transactionId;

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << amount;
    params["refund_amount"] = oss.str();

    auto response = sdk_->sendRefundRequest(params);

    PaymentResult result;
    result.success = (response["code"] == "10000");
    result.transactionId = transactionId;
    result.message = response["msg"];
    result.amount = amount;
    return result;
}

PaymentResult AlipayAdapter::queryStatus(const std::string& transactionId) {
    auto response = sdk_->queryTransaction(transactionId);

    PaymentResult result;
    result.success = (response["code"] == "10000");
    result.transactionId = response["trade_no"];
    result.message = response["trade_status"];
    result.amount = std::stod(response["total_amount"]);
    return result;
}

// -------------------- WechatPayAdapter --------------------

WechatPayAdapter::WechatPayAdapter()
    : sdk_(std::make_unique<WechatPaySDK>()) {}

int WechatPayAdapter::yuanToFen(double yuan) {
    // 关键适配：微信支付用分为单位，我们的接口用元
    return static_cast<int>(yuan * 100 + 0.5);  // 四舍五入避免浮点误差
}

PaymentResult WechatPayAdapter::pay(const std::string& orderId,
                                     double amount,
                                     const std::string& currency) {
    // 适配：元 → 分，参数名映射
    int totalFee = yuanToFen(amount);

    // 微信支付的 fee_type 使用 ISO 4217 货币代码
    std::string feeType = (currency == "CNY") ? "CNY" : currency;

    auto response = sdk_->unifiedOrder(orderId, totalFee,
                                        "商品支付", feeType);

    // 适配：微信的响应结构 → 统一 PaymentResult
    PaymentResult result;
    result.success = (response.returnCode == "SUCCESS" &&
                      response.resultCode == "SUCCESS");
    result.transactionId = response.prepayId;
    result.message = result.success ? "支付成功" : response.errCodeDes;
    result.amount = amount;
    result.currency = currency;
    return result;
}

PaymentResult WechatPayAdapter::refund(const std::string& transactionId,
                                        double amount) {
    int refundFee = yuanToFen(amount);
    int totalFee = refundFee;  // 简化：全额退款

    auto response = sdk_->refundOrder(transactionId, refundFee, totalFee);

    PaymentResult result;
    result.success = (response.returnCode == "SUCCESS");
    result.transactionId = transactionId;
    result.message = result.success ? "退款成功" : response.errCodeDes;
    result.amount = amount;
    return result;
}

PaymentResult WechatPayAdapter::queryStatus(const std::string& transactionId) {
    auto response = sdk_->orderQuery(transactionId);

    PaymentResult result;
    result.success = (response.returnCode == "SUCCESS");
    result.transactionId = response.prepayId;
    result.message = result.success ? "交易成功" : "查询失败";
    return result;
}

// -------------------- StripeAdapter --------------------

StripeAdapter::StripeAdapter()
    : sdk_(std::make_unique<StripeSDK>()) {}

int StripeAdapter::toCents(double amount) {
    return static_cast<int>(amount * 100 + 0.5);
}

PaymentResult StripeAdapter::pay(const std::string& orderId,
                                  double amount,
                                  const std::string& currency) {
    // 适配：金额 → 美分，货币代码小写化
    int amountInCents = toCents(amount);
    std::string lowerCurrency = currency;
    for (auto& c : lowerCurrency) c = static_cast<char>(std::tolower(c));

    try {
        // Stripe 通过异常报告错误，而我们的接口通过返回值
        auto charge = sdk_->createCharge(amountInCents, lowerCurrency,
                                          "Order: " + orderId);

        PaymentResult result;
        result.success = (charge.status == "succeeded");
        result.transactionId = charge.id;
        result.message = result.success ? "Payment succeeded" : charge.failureMessage;
        result.amount = amount;
        result.currency = currency;
        return result;
    } catch (const std::exception& e) {
        // 适配异常处理：将 Stripe 的异常转换为 PaymentResult
        PaymentResult result;
        result.success = false;
        result.message = std::string("Stripe error: ") + e.what();
        result.amount = amount;
        return result;
    }
}

PaymentResult StripeAdapter::refund(const std::string& transactionId,
                                     double amount) {
    try {
        int amountInCents = toCents(amount);
        auto charge = sdk_->createRefund(transactionId, amountInCents);

        PaymentResult result;
        result.success = (charge.status == "succeeded");
        result.transactionId = charge.id;
        result.message = result.success ? "Refund succeeded" : "Refund failed";
        result.amount = amount;
        return result;
    } catch (const std::exception& e) {
        PaymentResult result;
        result.success = false;
        result.message = std::string("Stripe refund error: ") + e.what();
        return result;
    }
}

PaymentResult StripeAdapter::queryStatus(const std::string& transactionId) {
    try {
        auto charge = sdk_->retrieveCharge(transactionId);

        PaymentResult result;
        result.success = (charge.status == "succeeded");
        result.transactionId = charge.id;
        result.message = charge.status;
        result.amount = charge.amountInCents / 100.0;
        result.currency = charge.currency;
        return result;
    } catch (const std::exception& e) {
        PaymentResult result;
        result.success = false;
        result.message = std::string("Stripe query error: ") + e.what();
        return result;
    }
}

// ============================================================
// 支付工厂
// ============================================================

std::unique_ptr<UnifiedPayment> PaymentFactory::create(const std::string& channel) {
    if (channel == "alipay") {
        return std::make_unique<AlipayAdapter>();
    } else if (channel == "wechat") {
        return std::make_unique<WechatPayAdapter>();
    } else if (channel == "stripe") {
        return std::make_unique<StripeAdapter>();
    }
    throw std::invalid_argument("不支持的支付渠道: " + channel);
}

// ============================================================
// 辅助函数
// ============================================================
void printResult(const PaymentResult& result) {
    std::cout << "    结果: " << (result.success ? "成功" : "失败") << std::endl;
    std::cout << "    交易号: " << result.transactionId << std::endl;
    std::cout << "    消息: " << result.message << std::endl;
    std::cout << "    金额: " << std::fixed << std::setprecision(2)
              << result.amount << " " << result.currency << std::endl;
}

// 模拟业务层代码 —— 完全不知道底层用的是哪个支付 SDK
void processOrder(UnifiedPayment& payment,
                  const std::string& orderId,
                  double amount,
                  const std::string& currency) {
    std::cout << "\n  使用 [" << payment.getChannelName()
              << "] 处理订单 " << orderId << std::endl;
    std::cout << "  ---------------------------------" << std::endl;

    // 1. 支付
    std::cout << "  >> 发起支付:\n" << std::endl;
    auto payResult = payment.pay(orderId, amount, currency);
    printResult(payResult);

    // 2. 查询
    if (payResult.success) {
        std::cout << "\n  >> 查询交易状态:\n" << std::endl;
        auto queryResult = payment.queryStatus(payResult.transactionId);
        printResult(queryResult);

        // 3. 退款
        std::cout << "\n  >> 发起退款:\n" << std::endl;
        auto refundResult = payment.refund(payResult.transactionId, amount);
        printResult(refundResult);
    }
}

// ============================================================
// 客户端代码
// ============================================================
int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "   Adapter 模式演示 - 第三方支付集成" << std::endl;
    std::cout << "============================================" << std::endl;

    // 场景1：支付宝支付
    std::cout << "\n【场景1】通过适配器使用支付宝" << std::endl;
    {
        auto alipay = PaymentFactory::create("alipay");
        processOrder(*alipay, "ORD20260321001", 99.90, "CNY");
    }

    // 场景2：微信支付
    std::cout << "\n\n【场景2】通过适配器使用微信支付" << std::endl;
    {
        auto wechat = PaymentFactory::create("wechat");
        processOrder(*wechat, "ORD20260321002", 199.00, "CNY");
    }

    // 场景3：Stripe 国际支付
    std::cout << "\n\n【场景3】通过适配器使用 Stripe" << std::endl;
    {
        auto stripe = PaymentFactory::create("stripe");
        processOrder(*stripe, "ORD20260321003", 19.99, "USD");
    }

    // 场景4：多态使用 —— 业务层完全不关心底层支付渠道
    std::cout << "\n\n【场景4】多态使用 —— 批量处理不同渠道的支付" << std::endl;
    {
        std::vector<std::string> channels = {"alipay", "wechat", "stripe"};
        std::vector<double> amounts = {88.88, 66.66, 29.99};
        std::vector<std::string> currencies = {"CNY", "CNY", "USD"};

        for (size_t i = 0; i < channels.size(); ++i) {
            auto payment = PaymentFactory::create(channels[i]);
            std::string orderId = "BATCH" + std::to_string(i + 1);

            std::cout << "\n  --- 批量处理第 " << (i + 1) << " 笔 ---" << std::endl;
            auto result = payment->pay(orderId, amounts[i], currencies[i]);
            std::cout << "    [" << payment->getChannelName() << "] "
                      << (result.success ? "成功" : "失败")
                      << " | 交易号: " << result.transactionId << std::endl;
        }
    }

    std::cout << "\n============================================" << std::endl;
    std::cout << "  Adapter 模式要点总结：" << std::endl;
    std::cout << "  1. 将不兼容的第三方接口适配为统一接口" << std::endl;
    std::cout << "  2. 业务层通过统一接口操作，与具体SDK解耦" << std::endl;
    std::cout << "  3. 新增支付渠道只需新增适配器，无需修改业务代码" << std::endl;
    std::cout << "  4. 适配器负责数据格式转换(元↔分)和错误处理适配" << std::endl;
    std::cout << "============================================" << std::endl;

    return 0;
}
```

## 构建方式

```cmake title="CMakeLists.txt"
add_executable(Pattern_Adapter Adapter.cpp Adapter.h)
target_compile_features(Pattern_Adapter PRIVATE cxx_std_17)
```

:::tip 编译运行
```bash
cd build
cmake --build . --target Pattern_Adapter
./Pattern_Adapter
```
:::
