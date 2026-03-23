---
sidebar_position: 2
title: 源代码
description: Mediator 模式的完整实现代码
---

# 源代码

## 头文件（Mediator.h）

```cpp title="Mediator.h" showLineNumbers
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

// ============================================================
// Mediator 模式 - 空中交通管制系统
// 核心思想：用一个中介对象来封装一系列对象之间的交互，
//          使各对象不需要显式地相互引用，从而降低耦合度
// ============================================================

// 前向声明
class AirTrafficControl;

// -------------------- 位置和航班信息 --------------------

struct Position {
    double x = 0.0;  // 经度（简化为平面坐标）
    double y = 0.0;  // 纬度
    double altitude = 0.0;  // 高度（英尺）

    double distanceTo(const Position& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        double da = altitude - other.altitude;
        return std::sqrt(dx * dx + dy * dy + da * da);
    }
};

// -------------------- 同事类（Colleague）接口 --------------------

// 飞机基类 —— Colleague 角色
// 设计要点：飞机只与中介者（塔台）通信，不直接与其他飞机通信
class Aircraft {
public:
    Aircraft(const std::string& callSign, const std::string& type,
             AirTrafficControl* atc);
    virtual ~Aircraft() = default;

    // 请求操作（通过中介者协调）
    void requestLanding();
    void requestTakeoff();
    void requestAltitudeChange(double newAltitude);

    // 接收来自中介者的消息
    virtual void receiveMessage(const std::string& message);

    // 更新位置
    void setPosition(double x, double y, double altitude);

    // 获取信息
    const std::string& getCallSign() const { return callSign_; }
    const std::string& getType() const { return type_; }
    const Position& getPosition() const { return position_; }
    bool isOnGround() const { return onGround_; }

    // 着陆和起飞回调（由中介者调用）
    void land();
    void takeoff();
    void changeAltitude(double newAltitude);

protected:
    std::string callSign_;  // 呼号，如 "CA1234"
    std::string type_;      // 机型，如 "Boeing 737"
    Position position_;
    bool onGround_ = false;

    // 关键设计：飞机只持有中介者的指针，不持有其他飞机的引用
    AirTrafficControl* atc_;
};

// 具体飞机类型
class CommercialAircraft : public Aircraft {
public:
    CommercialAircraft(const std::string& callSign,
                       AirTrafficControl* atc, int passengerCount);

    void receiveMessage(const std::string& message) override;

private:
    int passengerCount_;
};

class CargoAircraft : public Aircraft {
public:
    CargoAircraft(const std::string& callSign,
                  AirTrafficControl* atc, double cargoWeight);

    void receiveMessage(const std::string& message) override;

private:
    double cargoWeight_;  // 货物重量（吨）
};

class PrivateJet : public Aircraft {
public:
    PrivateJet(const std::string& callSign,
               AirTrafficControl* atc);

    void receiveMessage(const std::string& message) override;
};

// -------------------- 中介者接口 --------------------

// 空中交通管制接口 —— Mediator 角色
// 设计要点：定义同事对象与中介者交互的接口
class AirTrafficControl {
public:
    virtual ~AirTrafficControl() = default;

    // 注册/注销飞机
    virtual void registerAircraft(Aircraft* aircraft) = 0;
    virtual void removeAircraft(Aircraft* aircraft) = 0;

    // 飞机通过中介者发起的请求
    virtual bool requestLanding(Aircraft* aircraft) = 0;
    virtual bool requestTakeoff(Aircraft* aircraft) = 0;
    virtual bool requestAltitudeChange(Aircraft* aircraft, double newAltitude) = 0;

    // 广播消息给所有飞机
    virtual void broadcast(const std::string& message, Aircraft* sender) = 0;
};

// -------------------- 具体中介者 --------------------

// 区域管制中心 —— ConcreteMediator
// 设计要点：中介者持有所有同事对象的引用，封装所有协调逻辑
class RegionalControlCenter : public AirTrafficControl {
public:
    explicit RegionalControlCenter(const std::string& name);

    void registerAircraft(Aircraft* aircraft) override;
    void removeAircraft(Aircraft* aircraft) override;

    bool requestLanding(Aircraft* aircraft) override;
    bool requestTakeoff(Aircraft* aircraft) override;
    bool requestAltitudeChange(Aircraft* aircraft, double newAltitude) override;

    void broadcast(const std::string& message, Aircraft* sender) override;

    // 查看当前管制状态
    void displayStatus() const;

private:
    // 检查是否有碰撞风险
    bool checkConflict(Aircraft* aircraft, double newAltitude) const;

    // 通知特定飞机
    void notifyAircraft(Aircraft* aircraft, const std::string& message);

    std::string name_;
    std::vector<Aircraft*> aircrafts_;  // 管制区域内的所有飞机
    bool runwayOccupied_ = false;       // 跑道是否被占用
    Aircraft* runwayUser_ = nullptr;     // 当前使用跑道的飞机

    static constexpr double kMinSafeDistance = 50.0;  // 最小安全距离
    static constexpr double kMinAltitudeSep = 1000.0; // 最小高度间隔（英尺）
};
```

## 实现文件（Mediator.cpp）

```cpp title="Mediator.cpp" showLineNumbers
#include "Mediator.h"
#include <iostream>
#include <string>
#include <memory>
#include <vector>

// ============================================================
// Aircraft（同事类）基类实现
// ============================================================

Aircraft::Aircraft(const std::string& callSign, const std::string& type,
                   AirTrafficControl* atc)
    : callSign_(callSign), type_(type), atc_(atc) {
    // 飞机在创建时向塔台注册
    if (atc_) {
        atc_->registerAircraft(this);
    }
}

void Aircraft::requestLanding() {
    std::cout << "  [" << callSign_ << "] 请求降落" << std::endl;
    // 关键：飞机不直接检查跑道状态或其他飞机位置
    //       而是通过中介者来协调
    if (atc_) {
        atc_->requestLanding(this);
    }
}

void Aircraft::requestTakeoff() {
    std::cout << "  [" << callSign_ << "] 请求起飞" << std::endl;
    if (atc_) {
        atc_->requestTakeoff(this);
    }
}

void Aircraft::requestAltitudeChange(double newAltitude) {
    std::cout << "  [" << callSign_ << "] 请求变更高度至 "
              << newAltitude << " 英尺" << std::endl;
    if (atc_) {
        atc_->requestAltitudeChange(this, newAltitude);
    }
}

void Aircraft::receiveMessage(const std::string& message) {
    std::cout << "  [" << callSign_ << "] 收到消息: " << message << std::endl;
}

void Aircraft::setPosition(double x, double y, double altitude) {
    position_ = {x, y, altitude};
}

void Aircraft::land() {
    onGround_ = true;
    position_.altitude = 0;
    std::cout << "  [" << callSign_ << "] 已安全着陆" << std::endl;
}

void Aircraft::takeoff() {
    onGround_ = false;
    position_.altitude = 5000;
    std::cout << "  [" << callSign_ << "] 已起飞，爬升至 5000 英尺" << std::endl;
}

void Aircraft::changeAltitude(double newAltitude) {
    std::cout << "  [" << callSign_ << "] 高度从 " << position_.altitude
              << " 变更至 " << newAltitude << " 英尺" << std::endl;
    position_.altitude = newAltitude;
}

// ============================================================
// 具体飞机类型实现
// ============================================================

CommercialAircraft::CommercialAircraft(const std::string& callSign,
                                       AirTrafficControl* atc, int passengerCount)
    : Aircraft(callSign, "商业客机", atc), passengerCount_(passengerCount) {}

void CommercialAircraft::receiveMessage(const std::string& message) {
    std::cout << "  [" << callSign_ << " 客机/" << passengerCount_
              << "名乘客] 收到: " << message << std::endl;
}

CargoAircraft::CargoAircraft(const std::string& callSign,
                               AirTrafficControl* atc, double cargoWeight)
    : Aircraft(callSign, "货运飞机", atc), cargoWeight_(cargoWeight) {}

void CargoAircraft::receiveMessage(const std::string& message) {
    std::cout << "  [" << callSign_ << " 货机/" << cargoWeight_
              << "吨] 收到: " << message << std::endl;
}

PrivateJet::PrivateJet(const std::string& callSign, AirTrafficControl* atc)
    : Aircraft(callSign, "私人飞机", atc) {}

void PrivateJet::receiveMessage(const std::string& message) {
    std::cout << "  [" << callSign_ << " 私人机] 收到: " << message << std::endl;
}

// ============================================================
// RegionalControlCenter（具体中介者）实现
// ============================================================

RegionalControlCenter::RegionalControlCenter(const std::string& name)
    : name_(name) {
    std::cout << "  [塔台] " << name_ << " 管制中心启动" << std::endl;
}

void RegionalControlCenter::registerAircraft(Aircraft* aircraft) {
    aircrafts_.push_back(aircraft);
    std::cout << "  [塔台] " << aircraft->getCallSign()
              << " (" << aircraft->getType() << ") 进入管制区域" << std::endl;
}

void RegionalControlCenter::removeAircraft(Aircraft* aircraft) {
    aircrafts_.erase(
        std::remove(aircrafts_.begin(), aircrafts_.end(), aircraft),
        aircrafts_.end());
    std::cout << "  [塔台] " << aircraft->getCallSign()
              << " 离开管制区域" << std::endl;

    // 如果离开的是正在使用跑道的飞机，释放跑道
    if (runwayUser_ == aircraft) {
        runwayOccupied_ = false;
        runwayUser_ = nullptr;
    }
}

bool RegionalControlCenter::requestLanding(Aircraft* aircraft) {
    std::cout << "  [塔台] 处理 " << aircraft->getCallSign()
              << " 的降落请求..." << std::endl;

    // 检查跑道是否空闲
    if (runwayOccupied_) {
        std::string msg = "降落请求被拒绝，跑道被 " +
                          runwayUser_->getCallSign() + " 占用，请在等待区盘旋";
        notifyAircraft(aircraft, msg);

        // 通知正在使用跑道的飞机加快速度
        notifyAircraft(runwayUser_,
                       aircraft->getCallSign() + " 等待降落，请尽快完成");
        return false;
    }

    // 检查是否有高度冲突
    if (checkConflict(aircraft, 0)) {
        notifyAircraft(aircraft, "降落请求被延迟，存在空域冲突");
        // 通知冲突飞机调整
        for (auto* other : aircrafts_) {
            if (other != aircraft && !other->isOnGround()) {
                double dist = aircraft->getPosition().distanceTo(other->getPosition());
                if (dist < kMinSafeDistance) {
                    notifyAircraft(other,
                        "注意：" + aircraft->getCallSign() +
                        " 正在降落进近，请保持距离");
                }
            }
        }
        return false;
    }

    // 批准降落
    runwayOccupied_ = true;
    runwayUser_ = aircraft;
    notifyAircraft(aircraft, "降落已批准，跑道 27L 已清空");
    aircraft->land();

    // 广播给其他飞机
    broadcast(aircraft->getCallSign() + " 正在降落，注意避让", aircraft);

    // 降落完成，释放跑道
    runwayOccupied_ = false;
    runwayUser_ = nullptr;
    return true;
}

bool RegionalControlCenter::requestTakeoff(Aircraft* aircraft) {
    std::cout << "  [塔台] 处理 " << aircraft->getCallSign()
              << " 的起飞请求..." << std::endl;

    if (!aircraft->isOnGround()) {
        notifyAircraft(aircraft, "起飞请求无效，您目前不在地面");
        return false;
    }

    if (runwayOccupied_) {
        std::string msg = "起飞请求被延迟，跑道被 " +
                          runwayUser_->getCallSign() + " 占用";
        notifyAircraft(aircraft, msg);
        return false;
    }

    // 批准起飞
    runwayOccupied_ = true;
    runwayUser_ = aircraft;
    notifyAircraft(aircraft, "起飞已批准，跑道 27L");
    aircraft->takeoff();

    // 通知其他飞机
    broadcast(aircraft->getCallSign() + " 已起飞，爬升中", aircraft);

    // 释放跑道
    runwayOccupied_ = false;
    runwayUser_ = nullptr;
    return true;
}

bool RegionalControlCenter::requestAltitudeChange(Aircraft* aircraft,
                                                    double newAltitude) {
    std::cout << "  [塔台] 处理 " << aircraft->getCallSign()
              << " 的高度变更请求 -> " << newAltitude << " 英尺" << std::endl;

    // 检查新高度是否与其他飞机冲突
    for (auto* other : aircrafts_) {
        if (other == aircraft || other->isOnGround()) continue;

        double altDiff = std::abs(newAltitude - other->getPosition().altitude);
        if (altDiff < kMinAltitudeSep) {
            std::ostringstream oss;
            oss << "高度变更被拒绝，与 " << other->getCallSign()
                << " (高度 " << other->getPosition().altitude
                << " 英尺) 的间隔不足 " << kMinAltitudeSep << " 英尺";
            notifyAircraft(aircraft, oss.str());

            // 建议替代高度
            double suggestedAlt = other->getPosition().altitude + kMinAltitudeSep;
            oss.str("");
            oss << "建议高度: " << suggestedAlt << " 英尺";
            notifyAircraft(aircraft, oss.str());
            return false;
        }
    }

    // 批准高度变更
    notifyAircraft(aircraft, "高度变更已批准");
    aircraft->changeAltitude(newAltitude);

    // 通知附近飞机
    for (auto* other : aircrafts_) {
        if (other == aircraft || other->isOnGround()) continue;
        double dist = aircraft->getPosition().distanceTo(other->getPosition());
        if (dist < kMinSafeDistance * 3) {  // 通知较大范围内的飞机
            std::ostringstream oss;
            oss << "注意：" << aircraft->getCallSign()
                << " 高度变更至 " << newAltitude << " 英尺";
            notifyAircraft(other, oss.str());
        }
    }

    return true;
}

void RegionalControlCenter::broadcast(const std::string& message,
                                        Aircraft* sender) {
    // 中介者负责将消息转发给所有相关的同事对象
    for (auto* aircraft : aircrafts_) {
        if (aircraft != sender) {
            aircraft->receiveMessage("[广播] " + message);
        }
    }
}

bool RegionalControlCenter::checkConflict(Aircraft* aircraft,
                                            double newAltitude) const {
    for (auto* other : aircrafts_) {
        if (other == aircraft || other->isOnGround()) continue;
        double dist = aircraft->getPosition().distanceTo(other->getPosition());
        if (dist < kMinSafeDistance) {
            return true;  // 存在冲突
        }
    }
    return false;
}

void RegionalControlCenter::notifyAircraft(Aircraft* aircraft,
                                             const std::string& message) {
    aircraft->receiveMessage("[塔台] " + message);
}

void RegionalControlCenter::displayStatus() const {
    std::cout << "\n  ========= " << name_ << " 管制状态 ========="
              << std::endl;
    std::cout << "  管制飞机数量: " << aircrafts_.size() << std::endl;
    std::cout << "  跑道状态: " << (runwayOccupied_ ? "占用" : "空闲") << std::endl;
    for (auto* ac : aircrafts_) {
        std::cout << "    - " << ac->getCallSign()
                  << " (" << ac->getType() << ")"
                  << " | 高度: " << ac->getPosition().altitude << " 英尺"
                  << " | 状态: " << (ac->isOnGround() ? "地面" : "空中")
                  << std::endl;
    }
    std::cout << "  ======================================\n" << std::endl;
}

// ============================================================
// 客户端代码
// ============================================================
int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "   Mediator 模式演示 - 空中交通管制" << std::endl;
    std::cout << "============================================" << std::endl;

    // 创建管制中心（中介者）
    std::cout << "\n【初始化】创建管制中心和飞机\n" << std::endl;
    RegionalControlCenter atc("华东区域");

    // 创建飞机（同事对象）—— 飞机构造时自动向塔台注册
    CommercialAircraft ca1234("CA1234", &atc, 180);
    ca1234.setPosition(100, 200, 10000);

    CommercialAircraft mu5678("MU5678", &atc, 220);
    mu5678.setPosition(150, 180, 15000);

    CargoAircraft sf001("SF001", &atc, 45.5);
    sf001.setPosition(80, 160, 20000);

    PrivateJet bj100("BJ100", &atc);
    bj100.setPosition(200, 250, 8000);

    atc.displayStatus();

    // 场景1：降落请求
    std::cout << "【场景1】CA1234 请求降落\n" << std::endl;
    ca1234.requestLanding();

    atc.displayStatus();

    // 场景2：跑道冲突
    // CA1234 已降落并占地面。让 BJ100 也着陆，不会冲突
    std::cout << "【场景2】BJ100 也请求降落（跑道已空闲）\n" << std::endl;
    bj100.requestLanding();

    // 场景3：起飞请求
    std::cout << "\n【场景3】CA1234 请求起飞\n" << std::endl;
    ca1234.requestTakeoff();

    // 场景4：高度变更 - 冲突检测
    std::cout << "\n【场景4】SF001 请求降至 15500 英尺（与 MU5678 的 15000 英尺冲突）\n"
              << std::endl;
    sf001.requestAltitudeChange(15500);

    // 场景5：高度变更 - 无冲突
    std::cout << "\n【场景5】SF001 请求降至 25000 英尺（无冲突）\n" << std::endl;
    sf001.requestAltitudeChange(25000);

    // 场景6：广播消息
    std::cout << "\n【场景6】塔台广播紧急消息\n" << std::endl;
    atc.broadcast("紧急通知：雷暴天气即将到来，所有飞机注意避让", nullptr);

    atc.displayStatus();

    std::cout << "============================================" << std::endl;
    std::cout << "  Mediator 模式要点总结：" << std::endl;
    std::cout << "  1. 飞机之间不直接通信，全部通过塔台协调" << std::endl;
    std::cout << "  2. 塔台封装了冲突检测、跑道管理等协调逻辑" << std::endl;
    std::cout << "  3. 新增飞机类型不需要修改其他飞机的代码" << std::endl;
    std::cout << "  4. 将网状通信结构简化为星型结构" << std::endl;
    std::cout << "============================================" << std::endl;

    return 0;
}
```

## 构建方式

```cmake title="CMakeLists.txt"
add_executable(Pattern_Mediator Mediator.cpp Mediator.h)
target_compile_features(Pattern_Mediator PRIVATE cxx_std_17)
```

:::tip 编译运行
```bash
cd build
cmake --build . --target Pattern_Mediator
./Pattern_Mediator
```
:::
