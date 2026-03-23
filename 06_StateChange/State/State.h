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
