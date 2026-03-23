#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

// ============================================================
// 原型模式 (Prototype Pattern)
// 场景：游戏实体生成系统 —— 从预配置的原型模板克隆敌人/NPC，
//       避免重复设置大量属性，支持运行时动态注册新原型
// ============================================================

// --- 属性结构体：游戏实体的基础属性 ---
struct EntityStats {
    int health = 100;
    int attack = 10;
    int defense = 5;
    float speed = 1.0f;
    float detectionRange = 10.0f;
};

// --- 装备信息 ---
struct Equipment {
    std::string weapon;
    std::string armor;
    std::string accessory;
};

// --- 原型接口：游戏实体 ---
// 核心方法 clone()：返回自身的深拷贝
class GameEntity {
public:
    virtual ~GameEntity() = default;

    // 原型模式的核心：克隆自身
    virtual std::unique_ptr<GameEntity> clone() const = 0;

    // 实体信息
    virtual std::string getType() const = 0;
    virtual void describe() const = 0;

    // 属性访问
    void setName(const std::string& name);
    std::string getName() const;

    void setPosition(float x, float y);
    float getX() const;
    float getY() const;

    void setStats(const EntityStats& stats);
    EntityStats getStats() const;

    void setEquipment(const Equipment& equip);
    Equipment getEquipment() const;

    // AI 行为标签
    void addBehavior(const std::string& behavior);
    const std::vector<std::string>& getBehaviors() const;

protected:
    std::string name_ = "Unknown";
    float x_ = 0.0f, y_ = 0.0f;
    EntityStats stats_;
    Equipment equipment_;
    std::vector<std::string> behaviors_;
};

// --- 具体原型：骷髅战士 ---
class SkeletonWarrior : public GameEntity {
public:
    SkeletonWarrior();
    std::unique_ptr<GameEntity> clone() const override;
    std::string getType() const override;
    void describe() const override;

    void setUndyingLevel(int level);
    int getUndyingLevel() const;

private:
    // 骷髅特有属性：不死等级（克隆时需要一并复制）
    int undyingLevel_ = 1;
};

// --- 具体原型：暗影刺客 ---
class ShadowAssassin : public GameEntity {
public:
    ShadowAssassin();
    std::unique_ptr<GameEntity> clone() const override;
    std::string getType() const override;
    void describe() const override;

    void setStealthDuration(float seconds);
    float getStealthDuration() const;

private:
    // 刺客特有属性：隐身持续时间
    float stealthDuration_ = 5.0f;
};

// --- 具体原型：火焰法师 ---
class FlameMage : public GameEntity {
public:
    FlameMage();
    std::unique_ptr<GameEntity> clone() const override;
    std::string getType() const override;
    void describe() const override;

    void setMana(int mana);
    int getMana() const;
    void addSpell(const std::string& spell);

private:
    int mana_ = 200;
    // 法师的技能列表（深拷贝容器）
    std::vector<std::string> spells_;
};

// --- 原型注册表（Prototype Registry）---
// 集中管理所有原型模板，按名称查找并克隆
// 支持运行时动态注册新原型（如 mod 系统加载的自定义敌人）
class EntityRegistry {
public:
    // 注册原型
    void registerPrototype(const std::string& key, std::unique_ptr<GameEntity> prototype);

    // 从注册表克隆实体
    std::unique_ptr<GameEntity> spawn(const std::string& key) const;

    // 查询已注册的原型
    std::vector<std::string> listPrototypes() const;
    bool hasPrototype(const std::string& key) const;

private:
    std::unordered_map<std::string, std::unique_ptr<GameEntity>> prototypes_;
};
