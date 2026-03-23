---
sidebar_position: 2
title: 源代码
description: State 模式的完整实现代码
---

# 源代码

## 头文件（State.h）

```cpp title="State.h" showLineNumbers
#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <vector>
#include <map>

// ============================================================
// 状态模式（State Pattern）—— 自动售货机
// 核心思想：允许一个对象在其内部状态改变时改变它的行为。
//          对象看起来似乎修改了它的类。
// ============================================================

// 前向声明
class VendingMachine;

// ------------------------------------------------------------
// 商品信息
// ------------------------------------------------------------
struct Product {
    std::string name;
    double price;   // 价格（元）
    int quantity;    // 库存数量

    Product(const std::string& n, double p, int q)
        : name(n), price(p), quantity(q) {}
};

// ------------------------------------------------------------
// State（状态基类）：定义售货机在各状态下的行为接口
// 设计要点：将与状态相关的行为封装到独立的状态对象中，
//          让售货机将行为委托给当前状态对象。
// ------------------------------------------------------------
class VendingMachineState {
public:
    virtual ~VendingMachineState() = default;

    // 所有状态下都可能触发的动作
    virtual void insertCoin(VendingMachine& machine, double amount) = 0;
    virtual void selectProduct(VendingMachine& machine, const std::string& productCode) = 0;
    virtual void dispense(VendingMachine& machine) = 0;
    virtual void cancelTransaction(VendingMachine& machine) = 0;

    // 获取状态名称（用于显示）
    virtual std::string getName() const = 0;
};

// ------------------------------------------------------------
// ConcreteState：空闲状态 —— 等待用户投币
// ------------------------------------------------------------
class IdleState : public VendingMachineState {
public:
    void insertCoin(VendingMachine& machine, double amount) override;
    void selectProduct(VendingMachine& machine, const std::string& productCode) override;
    void dispense(VendingMachine& machine) override;
    void cancelTransaction(VendingMachine& machine) override;
    std::string getName() const override { return "Idle (空闲)"; }
};

// ------------------------------------------------------------
// ConcreteState：已投币状态 —— 用户已投入硬币，等待选择商品
// ------------------------------------------------------------
class CoinInsertedState : public VendingMachineState {
public:
    void insertCoin(VendingMachine& machine, double amount) override;
    void selectProduct(VendingMachine& machine, const std::string& productCode) override;
    void dispense(VendingMachine& machine) override;
    void cancelTransaction(VendingMachine& machine) override;
    std::string getName() const override { return "CoinInserted (已投币)"; }
};

// ------------------------------------------------------------
// ConcreteState：出货状态 —— 正在出货
// ------------------------------------------------------------
class DispensingState : public VendingMachineState {
public:
    void insertCoin(VendingMachine& machine, double amount) override;
    void selectProduct(VendingMachine& machine, const std::string& productCode) override;
    void dispense(VendingMachine& machine) override;
    void cancelTransaction(VendingMachine& machine) override;
    std::string getName() const override { return "Dispensing (出货中)"; }
};

// ------------------------------------------------------------
// ConcreteState：售罄状态 —— 所有商品已售完
// ------------------------------------------------------------
class SoldOutState : public VendingMachineState {
public:
    void insertCoin(VendingMachine& machine, double amount) override;
    void selectProduct(VendingMachine& machine, const std::string& productCode) override;
    void dispense(VendingMachine& machine) override;
    void cancelTransaction(VendingMachine& machine) override;
    std::string getName() const override { return "SoldOut (售罄)"; }
};

// ------------------------------------------------------------
// Context（上下文）：VendingMachine —— 售货机
// 设计要点：
// 1. 持有当前状态的指针，将行为委托给状态对象
// 2. 提供 setState() 方法供状态对象触发状态转换
// 3. 维护与状态无关的数据（商品库存、余额等）
// ------------------------------------------------------------
class VendingMachine {
public:
    VendingMachine();

    // 用户操作（委托给当前状态）
    void insertCoin(double amount);
    void selectProduct(const std::string& productCode);
    void dispense();
    void cancelTransaction();

    // 状态转换（供状态对象调用）
    void setState(std::shared_ptr<VendingMachineState> newState);

    // 数据访问接口（供状态对象查询/修改售货机数据）
    void addBalance(double amount) { balance_ += amount; }
    void resetBalance() { balance_ = 0.0; }
    double getBalance() const { return balance_; }

    void setSelectedProduct(const std::string& code) { selectedProduct_ = code; }
    std::string getSelectedProduct() const { return selectedProduct_; }

    Product* findProduct(const std::string& code);
    bool hasAnyStock() const;
    void addProduct(const std::string& code, const std::string& name, double price, int qty);

    // 显示状态
    void displayStatus() const;
    void displayProducts() const;

    // 获取预创建的状态单例（避免重复创建）
    std::shared_ptr<VendingMachineState> getIdleState() const { return idleState_; }
    std::shared_ptr<VendingMachineState> getCoinInsertedState() const { return coinInsertedState_; }
    std::shared_ptr<VendingMachineState> getDispensingState() const { return dispensingState_; }
    std::shared_ptr<VendingMachineState> getSoldOutState() const { return soldOutState_; }

private:
    // 当前状态
    std::shared_ptr<VendingMachineState> currentState_;

    // 预创建的状态对象（共享，避免频繁分配）
    std::shared_ptr<VendingMachineState> idleState_;
    std::shared_ptr<VendingMachineState> coinInsertedState_;
    std::shared_ptr<VendingMachineState> dispensingState_;
    std::shared_ptr<VendingMachineState> soldOutState_;

    // 售货机数据
    std::map<std::string, Product> products_;  // 商品列表（编号 -> 商品）
    double balance_;                            // 当前投入金额
    std::string selectedProduct_;               // 已选商品编号
};
```

## 实现文件（State.cpp）

```cpp title="State.cpp" showLineNumbers
#include "State.h"
#include <iomanip>
#include <algorithm>

// ============================================================
// IdleState（空闲状态）实现
// ============================================================
void IdleState::insertCoin(VendingMachine& machine, double amount) {
    if (amount <= 0) {
        std::cout << "  [Idle] Invalid coin amount.\n";
        return;
    }
    std::cout << "  [Idle] Coin inserted: " << std::fixed << std::setprecision(1)
              << amount << " yuan.\n";
    machine.addBalance(amount);
    // 状态转换：空闲 -> 已投币
    machine.setState(machine.getCoinInsertedState());
}

void IdleState::selectProduct(VendingMachine& machine, const std::string& productCode) {
    std::cout << "  [Idle] Please insert coins first.\n";
}

void IdleState::dispense(VendingMachine& machine) {
    std::cout << "  [Idle] No transaction in progress.\n";
}

void IdleState::cancelTransaction(VendingMachine& machine) {
    std::cout << "  [Idle] No transaction to cancel.\n";
}

// ============================================================
// CoinInsertedState（已投币状态）实现
// ============================================================
void CoinInsertedState::insertCoin(VendingMachine& machine, double amount) {
    if (amount <= 0) {
        std::cout << "  [CoinInserted] Invalid coin amount.\n";
        return;
    }
    machine.addBalance(amount);
    std::cout << "  [CoinInserted] Additional coin: " << std::fixed << std::setprecision(1)
              << amount << " yuan. Total balance: " << machine.getBalance() << " yuan.\n";
    // 状态不变，仍在 CoinInserted
}

void CoinInsertedState::selectProduct(VendingMachine& machine, const std::string& productCode) {
    Product* product = machine.findProduct(productCode);
    if (!product) {
        std::cout << "  [CoinInserted] Product \"" << productCode << "\" not found.\n";
        return;
    }
    if (product->quantity <= 0) {
        std::cout << "  [CoinInserted] Product \"" << product->name << "\" is out of stock.\n";
        return;
    }
    if (machine.getBalance() < product->price) {
        std::cout << "  [CoinInserted] Insufficient balance. Need "
                  << std::fixed << std::setprecision(1) << product->price
                  << " yuan, have " << machine.getBalance() << " yuan.\n";
        return;
    }

    // 选择成功，记录商品并转到出货状态
    std::cout << "  [CoinInserted] Selected: " << product->name
              << " (" << std::fixed << std::setprecision(1) << product->price << " yuan)\n";
    machine.setSelectedProduct(productCode);
    // 状态转换：已投币 -> 出货中
    machine.setState(machine.getDispensingState());
    // 自动触发出货
    machine.dispense();
}

void CoinInsertedState::dispense(VendingMachine& machine) {
    std::cout << "  [CoinInserted] Please select a product first.\n";
}

void CoinInsertedState::cancelTransaction(VendingMachine& machine) {
    double refund = machine.getBalance();
    std::cout << "  [CoinInserted] Transaction cancelled. Refunding "
              << std::fixed << std::setprecision(1) << refund << " yuan.\n";
    machine.resetBalance();
    // 状态转换：已投币 -> 空闲
    machine.setState(machine.getIdleState());
}

// ============================================================
// DispensingState（出货状态）实现
// ============================================================
void DispensingState::insertCoin(VendingMachine& machine, double amount) {
    std::cout << "  [Dispensing] Please wait, dispensing in progress...\n";
}

void DispensingState::selectProduct(VendingMachine& machine, const std::string& productCode) {
    std::cout << "  [Dispensing] Please wait, dispensing in progress...\n";
}

void DispensingState::dispense(VendingMachine& machine) {
    std::string code = machine.getSelectedProduct();
    Product* product = machine.findProduct(code);

    if (!product || product->quantity <= 0) {
        std::cout << "  [Dispensing] ERROR: Product unavailable!\n";
        machine.setState(machine.getIdleState());
        return;
    }

    // 扣款并出货
    double price = product->price;
    double change = machine.getBalance() - price;
    product->quantity--;

    std::cout << "  [Dispensing] >>> Dispensing \"" << product->name << "\" <<<\n";
    if (change > 0.01) {  // 浮点精度容差
        std::cout << "  [Dispensing] Change returned: "
                  << std::fixed << std::setprecision(1) << change << " yuan.\n";
    }

    machine.resetBalance();
    machine.setSelectedProduct("");

    // 判断下一个状态：是否还有库存
    if (machine.hasAnyStock()) {
        // 状态转换：出货中 -> 空闲
        machine.setState(machine.getIdleState());
    } else {
        // 状态转换：出货中 -> 售罄
        std::cout << "  [Dispensing] All products sold out!\n";
        machine.setState(machine.getSoldOutState());
    }
}

void DispensingState::cancelTransaction(VendingMachine& machine) {
    std::cout << "  [Dispensing] Cannot cancel during dispensing.\n";
}

// ============================================================
// SoldOutState（售罄状态）实现
// ============================================================
void SoldOutState::insertCoin(VendingMachine& machine, double amount) {
    std::cout << "  [SoldOut] Sorry, all products are sold out. Coin rejected.\n";
}

void SoldOutState::selectProduct(VendingMachine& machine, const std::string& productCode) {
    std::cout << "  [SoldOut] Sorry, all products are sold out.\n";
}

void SoldOutState::dispense(VendingMachine& machine) {
    std::cout << "  [SoldOut] No products to dispense.\n";
}

void SoldOutState::cancelTransaction(VendingMachine& machine) {
    std::cout << "  [SoldOut] No transaction in progress.\n";
}

// ============================================================
// VendingMachine（Context）实现
// ============================================================
VendingMachine::VendingMachine()
    : balance_(0.0)
    , selectedProduct_("")
{
    // 预创建所有状态对象（共享实例，避免每次状态转换都分配内存）
    idleState_         = std::make_shared<IdleState>();
    coinInsertedState_ = std::make_shared<CoinInsertedState>();
    dispensingState_   = std::make_shared<DispensingState>();
    soldOutState_      = std::make_shared<SoldOutState>();

    // 初始状态为空闲
    currentState_ = idleState_;
}

void VendingMachine::insertCoin(double amount) {
    currentState_->insertCoin(*this, amount);
}

void VendingMachine::selectProduct(const std::string& productCode) {
    currentState_->selectProduct(*this, productCode);
}

void VendingMachine::dispense() {
    currentState_->dispense(*this);
}

void VendingMachine::cancelTransaction() {
    currentState_->cancelTransaction(*this);
}

void VendingMachine::setState(std::shared_ptr<VendingMachineState> newState) {
    std::cout << "  >> State transition: " << currentState_->getName()
              << " -> " << newState->getName() << "\n";
    currentState_ = std::move(newState);
}

Product* VendingMachine::findProduct(const std::string& code) {
    auto it = products_.find(code);
    if (it != products_.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool VendingMachine::hasAnyStock() const {
    for (const auto& [code, product] : products_) {
        if (product.quantity > 0) return true;
    }
    return false;
}

void VendingMachine::addProduct(const std::string& code, const std::string& name,
                                 double price, int qty) {
    products_.emplace(code, Product(name, price, qty));
}

void VendingMachine::displayStatus() const {
    std::cout << "  +--------------------------+\n";
    std::cout << "  | State  : " << std::left << std::setw(16)
              << currentState_->getName() << "|\n";
    std::cout << "  | Balance: " << std::fixed << std::setprecision(1)
              << std::setw(10) << balance_ << " yuan  |\n";
    if (!selectedProduct_.empty()) {
        std::cout << "  | Selected: " << std::setw(15) << selectedProduct_ << "|\n";
    }
    std::cout << "  +--------------------------+\n";
}

void VendingMachine::displayProducts() const {
    std::cout << "  +------+------------------+-------+-----+\n";
    std::cout << "  | Code |       Name       | Price | Qty |\n";
    std::cout << "  +------+------------------+-------+-----+\n";
    for (const auto& [code, p] : products_) {
        std::cout << "  | " << std::left << std::setw(4) << code << " | "
                  << std::setw(16) << p.name << " | "
                  << std::right << std::fixed << std::setprecision(1)
                  << std::setw(5) << p.price << " | "
                  << std::setw(3) << p.quantity << " |\n";
    }
    std::cout << "  +------+------------------+-------+-----+\n";
}

// ============================================================
// main() —— 演示售货机状态转换
// ============================================================
int main() {
    std::cout << "========================================\n";
    std::cout << "  State Pattern - Vending Machine Demo\n";
    std::cout << "========================================\n\n";

    VendingMachine machine;

    // 装载商品
    machine.addProduct("A1", "Cola",         3.0, 2);
    machine.addProduct("A2", "Green Tea",    4.0, 1);
    machine.addProduct("A3", "Coffee",       5.0, 1);

    std::cout << "[Initial] Product inventory:\n";
    machine.displayProducts();
    machine.displayStatus();

    // --- Scenario 1: 正常购买流程 ---
    std::cout << "\n=== Scenario 1: Normal purchase ===\n";
    machine.insertCoin(5.0);
    machine.selectProduct("A1");   // Cola, 3.0 yuan -> change 2.0
    machine.displayStatus();

    // --- Scenario 2: 余额不足 ---
    std::cout << "\n=== Scenario 2: Insufficient balance ===\n";
    machine.insertCoin(2.0);
    machine.selectProduct("A3");   // Coffee, 5.0 yuan -> not enough
    machine.insertCoin(3.0);       // Now 5.0 total
    machine.selectProduct("A3");   // Should succeed now
    machine.displayStatus();

    // --- Scenario 3: 取消交易 ---
    std::cout << "\n=== Scenario 3: Cancel transaction ===\n";
    machine.insertCoin(3.0);
    machine.cancelTransaction();   // Refund 3.0
    machine.displayStatus();

    // --- Scenario 4: 空闲状态下的无效操作 ---
    std::cout << "\n=== Scenario 4: Invalid actions in Idle ===\n";
    machine.selectProduct("A1");   // No coin inserted
    machine.dispense();            // No transaction
    machine.cancelTransaction();   // Nothing to cancel

    // --- Scenario 5: 售罄状态 ---
    std::cout << "\n=== Scenario 5: Buying until sold out ===\n";
    std::cout << "\nCurrent inventory:\n";
    machine.displayProducts();

    // 买掉 Cola (A1: 还剩1个)
    std::cout << "\nBuying last Cola...\n";
    machine.insertCoin(3.0);
    machine.selectProduct("A1");

    // 买掉 Green Tea (A2: 还剩1个)
    std::cout << "\nBuying last Green Tea...\n";
    machine.insertCoin(4.0);
    machine.selectProduct("A2");
    machine.displayStatus();

    // --- Scenario 6: 售罄后的操作 ---
    std::cout << "\n=== Scenario 6: Actions when sold out ===\n";
    machine.insertCoin(5.0);       // Rejected
    machine.selectProduct("A1");   // Rejected
    machine.displayStatus();

    // 最终库存
    std::cout << "\n[Final] Product inventory:\n";
    machine.displayProducts();

    std::cout << "\n========================================\n";
    std::cout << "  Demo complete.\n";
    std::cout << "========================================\n";

    return 0;
}
```

## 构建方式

```cmake title="CMakeLists.txt"
add_executable(Pattern_State State.cpp State.h)
target_compile_features(Pattern_State PRIVATE cxx_std_17)
```

:::tip 编译运行
```bash
cd build
cmake --build . --target Pattern_State
./Pattern_State
```
:::