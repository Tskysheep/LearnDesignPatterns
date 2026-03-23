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
