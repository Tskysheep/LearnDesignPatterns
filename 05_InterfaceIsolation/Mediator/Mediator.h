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

    // 接��来自中介者的消息
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
