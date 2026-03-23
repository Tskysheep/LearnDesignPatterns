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
