#pragma once
#include <string>
#include <vector>
#include <memory>
#include <stack>
#include <functional>
#include <iostream>
#include <unordered_map>

// ============================================================
// 命令模式（Command Pattern）— 智能家居自动化系统
// 核心思想：将"请求"封装为对象，从而支持参数化、排队、撤销/重做
// ============================================================

// ---- 接收者（Receiver）：实际执行操作的智能设备 ----

// 智能灯光系统
class LightSystem {
public:
    explicit LightSystem(const std::string& room);

    void turnOn();
    void turnOff();
    void setBrightness(int level);
    int getBrightness() const;
    bool isOn() const;
    std::string getRoom() const;

private:
    std::string room_;
    bool on_ = false;
    int brightness_ = 100; // 0-100
};

// 恒温器
class Thermostat {
public:
    explicit Thermostat(const std::string& zone);

    void setTemperature(double temp);
    double getTemperature() const;
    void setMode(const std::string& mode); // "cooling", "heating", "auto"
    std::string getMode() const;
    std::string getZone() const;

private:
    std::string zone_;
    double temperature_ = 22.0;
    std::string mode_ = "auto";
};

// 音乐系统
class MusicSystem {
public:
    void play(const std::string& playlist);
    void stop();
    void setVolume(int volume);
    int getVolume() const;
    bool isPlaying() const;
    std::string getCurrentPlaylist() const;

private:
    bool playing_ = false;
    int volume_ = 50;
    std::string currentPlaylist_;
};

// ---- 命令接口（Command Interface） ----
// 关键设计：每个命令对象保存执行前的状态快照，以便撤销
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string description() const = 0;
};

// ---- 具体命令（Concrete Commands） ----

// 灯光开关命令
class LightOnCommand : public Command {
public:
    explicit LightOnCommand(std::shared_ptr<LightSystem> light);
    void execute() override;
    void undo() override;
    std::string description() const override;

private:
    std::shared_ptr<LightSystem> light_;
    bool previousState_ = false; // 保存执行前状态，用于撤销
};

class LightOffCommand : public Command {
public:
    explicit LightOffCommand(std::shared_ptr<LightSystem> light);
    void execute() override;
    void undo() override;
    std::string description() const override;

private:
    std::shared_ptr<LightSystem> light_;
    bool previousState_ = false;
};

// 灯光亮度调节命令
class LightBrightnessCommand : public Command {
public:
    LightBrightnessCommand(std::shared_ptr<LightSystem> light, int brightness);
    void execute() override;
    void undo() override;
    std::string description() const override;

private:
    std::shared_ptr<LightSystem> light_;
    int targetBrightness_;
    int previousBrightness_ = 100;
};

// 恒温器温度设置命令
class ThermostatSetTempCommand : public Command {
public:
    ThermostatSetTempCommand(std::shared_ptr<Thermostat> thermostat, double temp);
    void execute() override;
    void undo() override;
    std::string description() const override;

private:
    std::shared_ptr<Thermostat> thermostat_;
    double targetTemp_;
    double previousTemp_ = 22.0;
};

// 恒温器模式切换命令
class ThermostatModeCommand : public Command {
public:
    ThermostatModeCommand(std::shared_ptr<Thermostat> thermostat, const std::string& mode);
    void execute() override;
    void undo() override;
    std::string description() const override;

private:
    std::shared_ptr<Thermostat> thermostat_;
    std::string targetMode_;
    std::string previousMode_;
};

// 音乐播放命令
class MusicPlayCommand : public Command {
public:
    MusicPlayCommand(std::shared_ptr<MusicSystem> music, const std::string& playlist);
    void execute() override;
    void undo() override;
    std::string description() const override;

private:
    std::shared_ptr<MusicSystem> music_;
    std::string playlist_;
    bool wasPlaying_ = false;
    std::string previousPlaylist_;
};

// 音乐停止命令
class MusicStopCommand : public Command {
public:
    explicit MusicStopCommand(std::shared_ptr<MusicSystem> music);
    void execute() override;
    void undo() override;
    std::string description() const override;

private:
    std::shared_ptr<MusicSystem> music_;
    bool wasPlaying_ = false;
    std::string previousPlaylist_;
    int previousVolume_ = 50;
};

// 音量调节命令
class MusicVolumeCommand : public Command {
public:
    MusicVolumeCommand(std::shared_ptr<MusicSystem> music, int volume);
    void execute() override;
    void undo() override;
    std::string description() const override;

private:
    std::shared_ptr<MusicSystem> music_;
    int targetVolume_;
    int previousVolume_ = 50;
};

// ---- 宏命令（Macro Command）：组合多个命令为一个 ----
// 组合模式 + 命令模式的经典结合
class MacroCommand : public Command {
public:
    explicit MacroCommand(const std::string& name);
    void addCommand(std::unique_ptr<Command> cmd);
    void execute() override;
    void undo() override;
    std::string description() const override;

private:
    std::string name_;
    std::vector<std::unique_ptr<Command>> commands_;
};

// ---- 调用者（Invoker）：智能家居遥控器 ----
// 关键设计：维护撤销/重做栈，记录命令历史
class SmartHomeController {
public:
    // 执行命令并压入撤销栈
    void executeCommand(std::unique_ptr<Command> cmd);

    // 撤销最近一条命令
    void undo();

    // 重做最近撤销的命令
    void redo();

    // 显示命令历史
    void showHistory() const;

    // 获取历史记录大小
    size_t historySize() const;

private:
    // 撤销栈：保存已执行的命令
    std::stack<std::unique_ptr<Command>> undoStack_;
    // 重做栈：保存已撤销的命令
    std::stack<std::unique_ptr<Command>> redoStack_;
    // 命令历史记录（描述文本）
    std::vector<std::string> history_;
};
