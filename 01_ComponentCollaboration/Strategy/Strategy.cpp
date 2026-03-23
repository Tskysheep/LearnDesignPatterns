#include "Strategy.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <random>

// =============================================================================
// 辅助函数
// =============================================================================

// 生成模拟的交易 ID
static std::string generateTransactionId(const std::string& prefix) {
    static std::mt19937 rng(42);  // 固定种子以获得可复现的输出
    std::uniform_int_distribution<int> dist(10000, 99999);
    return prefix + "-" + std::to_string(dist(rng));
}

// =============================================================================
// CreditCardStrategy 实现
// =============================================================================

CreditCardStrategy::CreditCardStrategy(const std::string& cardNumber,
                                       const std::string& holderName,
                                       const std::string& expiry,
                                       const std::string& cvv)
    : cardNumber_(cardNumber)
    , holderName_(holderName)
    , expiry_(expiry)
    , cvv_(cvv) {}

PaymentResult CreditCardStrategy::pay(const PaymentRequest& request) {
    std::cout << "  [信用卡] 处理支付..." << std::endl;
    std::cout << "  [信用卡] 持卡人: " << holderName_ << std::endl;
    std::cout << "  [信用卡] 卡号: ****-****-****-"
              << cardNumber_.substr(cardNumber_.size() - 4) << std::endl;

    // 模拟连接银行网关
    std::cout << "  [信用卡] 连接银行支付网关..." << std::endl;
    std::cout << "  [信用卡] 3D Secure 验证通过" << std::endl;

    double fee = request.amount * FEE_RATE;

    PaymentResult result;
    result.success = true;
    result.transactionId = generateTransactionId("CC");
    result.processedAmount = request.amount;
    result.fee = fee;
    result.message = "信用卡支付成功，手续费 " + std::to_string(fee) + " " + request.currency;

    return result;
}

bool CreditCardStrategy::validate(const PaymentRequest& request) const {
    if (cardNumber_.empty() || cardNumber_.size() < 13) {
        std::cout << "  [验证] 信用卡号无效" << std::endl;
        return false;
    }
    if (cvv_.size() != 3 && cvv_.size() != 4) {
        std::cout << "  [验证] CVV 无效" << std::endl;
        return false;
    }
    if (request.amount <= 0) {
        std::cout << "  [验证] 金额必须大于零" << std::endl;
        return false;
    }
    return luhnCheck(cardNumber_);
}

bool CreditCardStrategy::luhnCheck(const std::string& number) const {
    // 简化的校验：仅检查是否全为数字
    for (char c : number) {
        if (!std::isdigit(c)) return false;
    }
    return true;
}

// =============================================================================
// AlipayStrategy 实现
// =============================================================================

AlipayStrategy::AlipayStrategy(const std::string& accountId)
    : accountId_(accountId) {}

PaymentResult AlipayStrategy::pay(const PaymentRequest& request) {
    std::cout << "  [支付宝] 处理支付..." << std::endl;
    std::cout << "  [支付宝] 账户: " << accountId_ << std::endl;

    // 模拟支付宝支付流程
    std::cout << "  [支付宝] 生成支付二维码..." << std::endl;
    std::cout << "  [支付宝] 用户扫码确认..." << std::endl;
    std::cout << "  [支付宝] 余额扣款成功" << std::endl;

    double fee = request.amount * FEE_RATE;

    PaymentResult result;
    result.success = true;
    result.transactionId = generateTransactionId("ALIPAY");
    result.processedAmount = request.amount;
    result.fee = fee;
    result.message = "支付宝支付成功";

    return result;
}

bool AlipayStrategy::validate(const PaymentRequest& request) const {
    if (accountId_.empty()) {
        std::cout << "  [验证] 支付宝账户 ID 为空" << std::endl;
        return false;
    }
    if (request.amount <= 0) {
        std::cout << "  [验证] 金额必须大于零" << std::endl;
        return false;
    }
    if (request.amount > MAX_AMOUNT) {
        std::cout << "  [验证] 超出支付宝单笔限额 "
                  << MAX_AMOUNT << " 元" << std::endl;
        return false;
    }
    return true;
}

// =============================================================================
// CryptoStrategy 实现
// =============================================================================

CryptoStrategy::CryptoStrategy(const std::string& walletAddress,
                               const std::string& cryptoType)
    : walletAddress_(walletAddress)
    , cryptoType_(cryptoType) {}

PaymentResult CryptoStrategy::pay(const PaymentRequest& request) {
    std::cout << "  [加密货币] 处理 " << cryptoType_ << " 支付..." << std::endl;
    std::cout << "  [加密货币] 钱包地址: " << walletAddress_.substr(0, 10)
              << "..." << std::endl;

    double rate = getExchangeRate();
    double cryptoAmount = request.amount / rate;

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "  [加密货币] 汇率: 1 " << cryptoType_ << " = "
              << rate << " " << request.currency << std::endl;
    std::cout << "  [加密货币] 支付金额: " << cryptoAmount
              << " " << cryptoType_ << std::endl;

    // 模拟区块链确认
    std::cout << "  [加密货币] 广播交易到区块链网络..." << std::endl;
    std::cout << "  [加密货币] 等待确认 (3/6 区块确认)..." << std::endl;
    std::cout << "  [加密货币] 交易确认完成" << std::endl;

    PaymentResult result;
    result.success = true;
    result.transactionId = generateTransactionId("CRYPTO");
    result.processedAmount = request.amount;
    result.fee = NETWORK_FEE;
    result.message = "加密货币支付成功，支付 " + std::to_string(cryptoAmount)
                   + " " + cryptoType_;

    return result;
}

bool CryptoStrategy::validate(const PaymentRequest& request) const {
    if (walletAddress_.empty() || walletAddress_.size() < 20) {
        std::cout << "  [验证] 钱包地址无效" << std::endl;
        return false;
    }
    if (request.amount <= NETWORK_FEE) {
        std::cout << "  [验证] 金额过小，不足以覆盖网络手续费" << std::endl;
        return false;
    }
    return true;
}

double CryptoStrategy::getExchangeRate() const {
    // 模拟汇率（生产环境应调用交易所 API）
    if (cryptoType_ == "BTC") return 65000.0;
    if (cryptoType_ == "ETH") return 3500.0;
    return 1.0;
}

// =============================================================================
// PaymentProcessor（上下文）实现
// =============================================================================

PaymentProcessor::PaymentProcessor(std::unique_ptr<PaymentStrategy> strategy)
    : strategy_(std::move(strategy)) {}

// 运行时动态切换策略——这是策略模式的核心价值
void PaymentProcessor::setStrategy(std::unique_ptr<PaymentStrategy> strategy) {
    strategy_ = std::move(strategy);
    std::cout << "[支付处理器] 切换支付方式为: "
              << strategy_->getName() << std::endl;
}

PaymentResult PaymentProcessor::processPayment(const PaymentRequest& request) {
    if (!strategy_) {
        return {false, "", "未设置支付方式", 0.0, 0.0};
    }

    std::cout << "\n--- 开始处理订单 " << request.orderId << " ---" << std::endl;
    std::cout << "金额: " << std::fixed << std::setprecision(2)
              << request.amount << " " << request.currency << std::endl;
    std::cout << "支付方式: " << strategy_->getName() << std::endl;

    // Step 1: 验证参数
    std::cout << "[支付处理器] 验证支付参数..." << std::endl;
    if (!strategy_->validate(request)) {
        return {false, "", "支付参数验证失败", 0.0, 0.0};
    }
    std::cout << "[支付处理器] 验证通过" << std::endl;

    // Step 2: 委托给策略执行支付（策略模式的核心：委托而非继承）
    PaymentResult result = strategy_->pay(request);

    // Step 3: 输出结果
    if (result.success) {
        std::cout << "[支付处理器] 支付成功！交易号: "
                  << result.transactionId << std::endl;
        std::cout << "[支付处理器] 手续费: " << std::fixed << std::setprecision(2)
                  << result.fee << " " << request.currency << std::endl;
    } else {
        std::cout << "[支付处理器] 支付失败: " << result.message << std::endl;
    }
    std::cout << "--- 订单处理完毕 ---\n" << std::endl;

    return result;
}

std::string PaymentProcessor::getCurrentStrategyName() const {
    return strategy_ ? strategy_->getName() : "未设置";
}

// =============================================================================
// 工厂函数：根据类型字符串创建策略对象
// =============================================================================
std::unique_ptr<PaymentStrategy> createPaymentStrategy(
    const std::string& type,
    const std::map<std::string, std::string>& params) {

    if (type == "credit_card") {
        return std::make_unique<CreditCardStrategy>(
            params.at("card_number"),
            params.at("holder_name"),
            params.at("expiry"),
            params.at("cvv")
        );
    }
    if (type == "alipay") {
        return std::make_unique<AlipayStrategy>(params.at("account_id"));
    }
    if (type == "crypto") {
        std::string cryptoType = "BTC";
        auto it = params.find("crypto_type");
        if (it != params.end()) cryptoType = it->second;
        return std::make_unique<CryptoStrategy>(params.at("wallet_address"), cryptoType);
    }

    return nullptr;
}

// =============================================================================
// 主函数：演示策略模式
// =============================================================================
int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "    策略模式 (Strategy Pattern) 演示" << std::endl;
    std::cout << "    场景：电商支付系统" << std::endl;
    std::cout << "==================================================" << std::endl;

    // 创建支付处理器（上下文对象）
    PaymentProcessor processor;

    // 创建一个订单请求
    PaymentRequest order;
    order.orderId = "ORD-2024-001";
    order.amount = 1299.00;
    order.currency = "CNY";
    order.customerEmail = "customer@example.com";

    // ========================================
    // 演示1：使用信用卡支付
    // ========================================
    std::cout << "\n【演示 1】信用卡支付" << std::endl;
    processor.setStrategy(std::make_unique<CreditCardStrategy>(
        "6222021234567890", "张三", "12/26", "123"
    ));
    processor.processPayment(order);

    // ========================================
    // 演示2：运行时切换为支付宝——策略模式的核心价值
    // 同一个订单，只需切换策略对象即可使用不同的支付方式
    // ========================================
    std::cout << "【演示 2】运行时切换为支付宝" << std::endl;
    order.orderId = "ORD-2024-002";
    order.amount = 688.50;
    processor.setStrategy(std::make_unique<AlipayStrategy>("zhangsan@alipay.com"));
    processor.processPayment(order);

    // ========================================
    // 演示3：切换为加密货币支付
    // ========================================
    std::cout << "【演示 3】切换为加密货币 (ETH) 支付" << std::endl;
    order.orderId = "ORD-2024-003";
    order.amount = 3500.00;
    order.currency = "USD";
    processor.setStrategy(std::make_unique<CryptoStrategy>(
        "0x742d35Cc6634C0532925a3b844Bc9e7595f2bD38", "ETH"
    ));
    processor.processPayment(order);

    // ========================================
    // 演示4：使用工厂函数创建策略
    // ========================================
    std::cout << "【演示 4】使用工厂函数创建策略" << std::endl;
    order.orderId = "ORD-2024-004";
    order.amount = 15000.00;
    order.currency = "CNY";
    auto strategy = createPaymentStrategy("credit_card", {
        {"card_number", "6225881234567890"},
        {"holder_name", "李四"},
        {"expiry", "06/27"},
        {"cvv", "456"}
    });
    processor.setStrategy(std::move(strategy));
    processor.processPayment(order);

    // ========================================
    // 演示5：验证失败的情况（支付宝超限额）
    // ========================================
    std::cout << "【演示 5】验证失败 - 支付宝超出限额" << std::endl;
    order.orderId = "ORD-2024-005";
    order.amount = 60000.00;  // 超过支付宝单笔限额 50000
    processor.setStrategy(std::make_unique<AlipayStrategy>("lisi@alipay.com"));
    PaymentResult failResult = processor.processPayment(order);
    std::cout << "支付结果: " << (failResult.success ? "成功" : "失败")
              << " - " << failResult.message << std::endl;

    // 关键总结
    std::cout << "\n==================================================" << std::endl;
    std::cout << "策略模式要点总结：" << std::endl;
    std::cout << "1. PaymentStrategy 定义统一的支付接口" << std::endl;
    std::cout << "2. 各支付方式独立封装，互不影响" << std::endl;
    std::cout << "3. PaymentProcessor 运行时动态切换策略" << std::endl;
    std::cout << "4. 新增支付方式只需实现接口，无需修改已有代码" << std::endl;
    std::cout << "5. 策略模式用组合替代继承，更加灵活" << std::endl;
    std::cout << "==================================================" << std::endl;

    return 0;
}
