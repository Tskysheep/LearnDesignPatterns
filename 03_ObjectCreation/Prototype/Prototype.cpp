#include "Prototype.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

// ============================================================
// 原型模式 —— 游戏实体生成系统实现
// ============================================================

// ===================== GameEntity 基类实现 =====================
void GameEntity::setName(const std::string& name) { name_ = name; }
std::string GameEntity::getName() const { return name_; }

void GameEntity::setPosition(float x, float y) { x_ = x; y_ = y; }
float GameEntity::getX() const { return x_; }
float GameEntity::getY() const { return y_; }

void GameEntity::setStats(const EntityStats& stats) { stats_ = stats; }
EntityStats GameEntity::getStats() const { return stats_; }

void GameEntity::setEquipment(const Equipment& equip) { equipment_ = equip; }
Equipment GameEntity::getEquipment() const { return equipment_; }

void GameEntity::addBehavior(const std::string& behavior) {
    behaviors_.push_back(behavior);
}
const std::vector<std::string>& GameEntity::getBehaviors() const {
    return behaviors_;
}

// ===================== SkeletonWarrior 实现 =====================
SkeletonWarrior::SkeletonWarrior() {
    name_ = "Skeleton Warrior";
    // 预配置骷髅战士的默认属性——这些属性配置通常来自策划表，非常复杂
    stats_ = {80, 15, 20, 0.8f, 8.0f};
    equipment_ = {"Rusty Sword", "Bone Shield", "Cursed Ring"};
    behaviors_ = {"patrol", "attack_melee", "retreat_when_low_hp"};
    undyingLevel_ = 1;
}

// 深拷贝：创建新对象，复制所有属性（包括子类特有的 undyingLevel_）
std::unique_ptr<GameEntity> SkeletonWarrior::clone() const {
    auto copy = std::make_unique<SkeletonWarrior>(*this);
    return copy;
}

std::string SkeletonWarrior::getType() const { return "SkeletonWarrior"; }

void SkeletonWarrior::describe() const {
    std::cout << "  [" << getType() << "] \"" << name_ << "\""
              << " at (" << x_ << ", " << y_ << ")\n"
              << "    HP=" << stats_.health << " ATK=" << stats_.attack
              << " DEF=" << stats_.defense << " SPD=" << std::fixed
              << std::setprecision(1) << stats_.speed << "\n"
              << "    Weapon: " << equipment_.weapon
              << " | Armor: " << equipment_.armor << "\n"
              << "    Undying Level: " << undyingLevel_ << "\n";
}

void SkeletonWarrior::setUndyingLevel(int level) { undyingLevel_ = level; }
int SkeletonWarrior::getUndyingLevel() const { return undyingLevel_; }

// ===================== ShadowAssassin 实现 =====================
ShadowAssassin::ShadowAssassin() {
    name_ = "Shadow Assassin";
    stats_ = {60, 25, 8, 2.0f, 15.0f};
    equipment_ = {"Shadow Daggers", "Cloak of Shadows", "Smoke Bomb"};
    behaviors_ = {"stealth_approach", "backstab", "vanish_on_damage"};
    stealthDuration_ = 5.0f;
}

std::unique_ptr<GameEntity> ShadowAssassin::clone() const {
    auto copy = std::make_unique<ShadowAssassin>(*this);
    return copy;
}

std::string ShadowAssassin::getType() const { return "ShadowAssassin"; }

void ShadowAssassin::describe() const {
    std::cout << "  [" << getType() << "] \"" << name_ << "\""
              << " at (" << x_ << ", " << y_ << ")\n"
              << "    HP=" << stats_.health << " ATK=" << stats_.attack
              << " DEF=" << stats_.defense << " SPD=" << std::fixed
              << std::setprecision(1) << stats_.speed << "\n"
              << "    Weapon: " << equipment_.weapon
              << " | Accessory: " << equipment_.accessory << "\n"
              << "    Stealth Duration: " << stealthDuration_ << "s\n";
}

void ShadowAssassin::setStealthDuration(float seconds) { stealthDuration_ = seconds; }
float ShadowAssassin::getStealthDuration() const { return stealthDuration_; }

// ===================== FlameMage 实现 =====================
FlameMage::FlameMage() {
    name_ = "Flame Mage";
    stats_ = {50, 30, 5, 1.2f, 20.0f};
    equipment_ = {"Staff of Inferno", "Mage Robe", "Fire Amulet"};
    behaviors_ = {"keep_distance", "cast_spell", "teleport_when_close"};
    mana_ = 200;
    spells_ = {"Fireball", "Flame Wall", "Meteor Strike"};
}

// 深拷贝：vector<string> spells_ 的拷贝由编译器生成的拷贝构造函数自动完成
// 如果包含指针成员则需手动实现深拷贝
std::unique_ptr<GameEntity> FlameMage::clone() const {
    auto copy = std::make_unique<FlameMage>(*this);
    return copy;
}

std::string FlameMage::getType() const { return "FlameMage"; }

void FlameMage::describe() const {
    std::cout << "  [" << getType() << "] \"" << name_ << "\""
              << " at (" << x_ << ", " << y_ << ")\n"
              << "    HP=" << stats_.health << " ATK=" << stats_.attack
              << " DEF=" << stats_.defense << " Mana=" << mana_ << "\n"
              << "    Weapon: " << equipment_.weapon << "\n"
              << "    Spells: ";
    for (size_t i = 0; i < spells_.size(); ++i) {
        std::cout << spells_[i];
        if (i + 1 < spells_.size()) std::cout << ", ";
    }
    std::cout << "\n";
}

void FlameMage::setMana(int mana) { mana_ = mana; }
int FlameMage::getMana() const { return mana_; }
void FlameMage::addSpell(const std::string& spell) { spells_.push_back(spell); }

// ===================== EntityRegistry 实现 =====================
void EntityRegistry::registerPrototype(const std::string& key,
                                       std::unique_ptr<GameEntity> prototype) {
    prototypes_[key] = std::move(prototype);
}

// 从注册表中查找原型并克隆
std::unique_ptr<GameEntity> EntityRegistry::spawn(const std::string& key) const {
    auto it = prototypes_.find(key);
    if (it == prototypes_.end()) {
        throw std::runtime_error("Prototype not found: " + key);
    }
    // 调用原型的 clone() 方法获取副本
    return it->second->clone();
}

std::vector<std::string> EntityRegistry::listPrototypes() const {
    std::vector<std::string> keys;
    keys.reserve(prototypes_.size());
    for (const auto& [key, _] : prototypes_) {
        keys.push_back(key);
    }
    return keys;
}

bool EntityRegistry::hasPrototype(const std::string& key) const {
    return prototypes_.count(key) > 0;
}

// ===================== 客户端演示 =====================
int main() {
    std::cout << "========================================\n";
    std::cout << " Prototype Pattern Demo\n";
    std::cout << " Scenario: Game Entity Spawning System\n";
    std::cout << "========================================\n\n";

    // 1. 创建原型注册表并注册预配置的原型模板
    EntityRegistry registry;

    // 注册骷髅战士原型（普通版和精英版）
    auto skeletonProto = std::make_unique<SkeletonWarrior>();
    registry.registerPrototype("skeleton_warrior", std::move(skeletonProto));

    auto eliteSkeleton = std::make_unique<SkeletonWarrior>();
    eliteSkeleton->setName("Elite Skeleton");
    eliteSkeleton->setStats({150, 25, 35, 1.0f, 12.0f});
    eliteSkeleton->setUndyingLevel(3);
    registry.registerPrototype("elite_skeleton", std::move(eliteSkeleton));

    // 注册暗影刺客原型
    auto assassinProto = std::make_unique<ShadowAssassin>();
    registry.registerPrototype("shadow_assassin", std::move(assassinProto));

    // 注册火焰法师原型
    auto mageProto = std::make_unique<FlameMage>();
    registry.registerPrototype("flame_mage", std::move(mageProto));

    // 显示已注册的原型
    std::cout << "Registered prototypes: ";
    for (const auto& name : registry.listPrototypes()) {
        std::cout << "[" << name << "] ";
    }
    std::cout << "\n\n";

    // 2. 从原型克隆实体——无需知道具体类型，也不需要重复配置属性
    std::cout << "--- Spawning a wave of enemies ---\n\n";

    // 克隆 3 个骷髅战士，只需设置位置
    for (int i = 0; i < 3; ++i) {
        auto skeleton = registry.spawn("skeleton_warrior");
        skeleton->setName("Skeleton #" + std::to_string(i + 1));
        skeleton->setPosition(10.0f + i * 5.0f, 20.0f);
        skeleton->describe();
        std::cout << "\n";
    }

    // 克隆一个精英骷髅
    std::cout << "--- Spawning elite variant ---\n\n";
    auto elite = registry.spawn("elite_skeleton");
    elite->setPosition(50.0f, 50.0f);
    elite->describe();
    std::cout << "\n";

    // 克隆刺客并修改部分属性（克隆后的对象独立于原型）
    std::cout << "--- Spawning and customizing assassin ---\n\n";
    auto assassin = registry.spawn("shadow_assassin");
    assassin->setName("Nightblade");
    assassin->setPosition(30.0f, 40.0f);
    // 向下转型以访问特有属性——演示克隆保留了子类信息
    if (auto* sa = dynamic_cast<ShadowAssassin*>(assassin.get())) {
        sa->setStealthDuration(8.0f); // 自定义隐身时间
    }
    assassin->describe();
    std::cout << "\n";

    // 克隆法师
    std::cout << "--- Spawning mage ---\n\n";
    auto mage = registry.spawn("flame_mage");
    mage->setPosition(0.0f, 60.0f);
    if (auto* fm = dynamic_cast<FlameMage*>(mage.get())) {
        fm->addSpell("Inferno Blast"); // 给克隆体额外增加技能
    }
    mage->describe();
    std::cout << "\n";

    // 3. 验证深拷贝：修改克隆体不影响原型
    std::cout << "--- Deep copy verification ---\n";
    auto original = registry.spawn("flame_mage");
    auto cloned = registry.spawn("flame_mage");
    original->setName("Original Mage");
    cloned->setName("Cloned Mage");
    std::cout << "Original name: " << original->getName() << "\n";
    std::cout << "Cloned name:   " << cloned->getName() << "\n";
    std::cout << "(Names are independent -- deep copy confirmed)\n\n";

    std::cout << "========================================\n";
    std::cout << "Key insight: Cloning pre-configured prototypes is\n";
    std::cout << "far cheaper than re-initializing complex objects.\n";
    std::cout << "New entity types can be registered at runtime\n";
    std::cout << "(e.g., from mod files) without code changes.\n";
    std::cout << "========================================\n";

    return 0;
}
