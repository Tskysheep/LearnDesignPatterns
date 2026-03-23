---
sidebar_position: 2
title: 源代码
description: Command 模式的完整实现代码
---

# 源代码

## 头文件（Command.h）

```cpp title="Command.h" showLineNumbers
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
```

## 实现文件（Command.cpp）

```cpp title="Command.cpp" showLineNumbers
#include "Command.h"
#include <algorithm>

// ============================================================
// 接收者实现
// ============================================================

// ---- LightSystem ----
LightSystem::LightSystem(const std::string& room) : room_(room) {}

void LightSystem::turnOn() {
    on_ = true;
    std::cout << "  [Light] " << room_ << " light turned ON (brightness: " << brightness_ << "%)\n";
}

void LightSystem::turnOff() {
    on_ = false;
    std::cout << "  [Light] " << room_ << " light turned OFF\n";
}

void LightSystem::setBrightness(int level) {
    brightness_ = std::max(0, std::min(100, level));
    std::cout << "  [Light] " << room_ << " brightness set to " << brightness_ << "%\n";
}

int LightSystem::getBrightness() const { return brightness_; }
bool LightSystem::isOn() const { return on_; }
std::string LightSystem::getRoom() const { return room_; }

// ---- Thermostat ----
Thermostat::Thermostat(const std::string& zone) : zone_(zone) {}

void Thermostat::setTemperature(double temp) {
    temperature_ = temp;
    std::cout << "  [Thermostat] " << zone_ << " temperature set to " << temperature_ << " C\n";
}

double Thermostat::getTemperature() const { return temperature_; }

void Thermostat::setMode(const std::string& mode) {
    mode_ = mode;
    std::cout << "  [Thermostat] " << zone_ << " mode set to " << mode_ << "\n";
}

std::string Thermostat::getMode() const { return mode_; }
std::string Thermostat::getZone() const { return zone_; }

// ---- MusicSystem ----
void MusicSystem::play(const std::string& playlist) {
    playing_ = true;
    currentPlaylist_ = playlist;
    std::cout << "  [Music] Now playing: \"" << playlist << "\" (volume: " << volume_ << ")\n";
}

void MusicSystem::stop() {
    playing_ = false;
    std::cout << "  [Music] Playback stopped\n";
}

void MusicSystem::setVolume(int volume) {
    volume_ = std::max(0, std::min(100, volume));
    std::cout << "  [Music] Volume set to " << volume_ << "\n";
}

int MusicSystem::getVolume() const { return volume_; }
bool MusicSystem::isPlaying() const { return playing_; }
std::string MusicSystem::getCurrentPlaylist() const { return currentPlaylist_; }

// ============================================================
// 具体命令实现
// 每个 execute() 先保存当前状态快照，再执行操作
// 每个 undo() 用保存的快照恢复到执行前的状态
// ============================================================

// ---- LightOnCommand ----
LightOnCommand::LightOnCommand(std::shared_ptr<LightSystem> light)
    : light_(std::move(light)) {}

void LightOnCommand::execute() {
    previousState_ = light_->isOn();
    light_->turnOn();
}

void LightOnCommand::undo() {
    if (!previousState_) {
        light_->turnOff();
    }
}

std::string LightOnCommand::description() const {
    return "Turn on " + light_->getRoom() + " light";
}

// ---- LightOffCommand ----
LightOffCommand::LightOffCommand(std::shared_ptr<LightSystem> light)
    : light_(std::move(light)) {}

void LightOffCommand::execute() {
    previousState_ = light_->isOn();
    light_->turnOff();
}

void LightOffCommand::undo() {
    if (previousState_) {
        light_->turnOn();
    }
}

std::string LightOffCommand::description() const {
    return "Turn off " + light_->getRoom() + " light";
}

// ---- LightBrightnessCommand ----
LightBrightnessCommand::LightBrightnessCommand(std::shared_ptr<LightSystem> light, int brightness)
    : light_(std::move(light)), targetBrightness_(brightness) {}

void LightBrightnessCommand::execute() {
    previousBrightness_ = light_->getBrightness();
    light_->setBrightness(targetBrightness_);
}

void LightBrightnessCommand::undo() {
    light_->setBrightness(previousBrightness_);
}

std::string LightBrightnessCommand::description() const {
    return "Set " + light_->getRoom() + " brightness to " + std::to_string(targetBrightness_) + "%";
}

// ---- ThermostatSetTempCommand ----
ThermostatSetTempCommand::ThermostatSetTempCommand(std::shared_ptr<Thermostat> thermostat, double temp)
    : thermostat_(std::move(thermostat)), targetTemp_(temp) {}

void ThermostatSetTempCommand::execute() {
    previousTemp_ = thermostat_->getTemperature();
    thermostat_->setTemperature(targetTemp_);
}

void ThermostatSetTempCommand::undo() {
    thermostat_->setTemperature(previousTemp_);
}

std::string ThermostatSetTempCommand::description() const {
    return "Set " + thermostat_->getZone() + " temperature to " + std::to_string(static_cast<int>(targetTemp_)) + " C";
}

// ---- ThermostatModeCommand ----
ThermostatModeCommand::ThermostatModeCommand(std::shared_ptr<Thermostat> thermostat, const std::string& mode)
    : thermostat_(std::move(thermostat)), targetMode_(mode) {}

void ThermostatModeCommand::execute() {
    previousMode_ = thermostat_->getMode();
    thermostat_->setMode(targetMode_);
}

void ThermostatModeCommand::undo() {
    thermostat_->setMode(previousMode_);
}

std::string ThermostatModeCommand::description() const {
    return "Set " + thermostat_->getZone() + " mode to " + targetMode_;
}

// ---- MusicPlayCommand ----
MusicPlayCommand::MusicPlayCommand(std::shared_ptr<MusicSystem> music, const std::string& playlist)
    : music_(std::move(music)), playlist_(playlist) {}

void MusicPlayCommand::execute() {
    wasPlaying_ = music_->isPlaying();
    previousPlaylist_ = music_->getCurrentPlaylist();
    music_->play(playlist_);
}

void MusicPlayCommand::undo() {
    if (wasPlaying_) {
        music_->play(previousPlaylist_);
    } else {
        music_->stop();
    }
}

std::string MusicPlayCommand::description() const {
    return "Play playlist \"" + playlist_ + "\"";
}

// ---- MusicStopCommand ----
MusicStopCommand::MusicStopCommand(std::shared_ptr<MusicSystem> music)
    : music_(std::move(music)) {}

void MusicStopCommand::execute() {
    wasPlaying_ = music_->isPlaying();
    previousPlaylist_ = music_->getCurrentPlaylist();
    previousVolume_ = music_->getVolume();
    music_->stop();
}

void MusicStopCommand::undo() {
    if (wasPlaying_) {
        music_->play(previousPlaylist_);
        music_->setVolume(previousVolume_);
    }
}

std::string MusicStopCommand::description() const {
    return "Stop music playback";
}

// ---- MusicVolumeCommand ----
MusicVolumeCommand::MusicVolumeCommand(std::shared_ptr<MusicSystem> music, int volume)
    : music_(std::move(music)), targetVolume_(volume) {}

void MusicVolumeCommand::execute() {
    previousVolume_ = music_->getVolume();
    music_->setVolume(targetVolume_);
}

void MusicVolumeCommand::undo() {
    music_->setVolume(previousVolume_);
}

std::string MusicVolumeCommand::description() const {
    return "Set music volume to " + std::to_string(targetVolume_);
}

// ============================================================
// 宏命令实现 — 按顺序执行所有子命令，撤销时逆序撤销
// ============================================================
MacroCommand::MacroCommand(const std::string& name) : name_(name) {}

void MacroCommand::addCommand(std::unique_ptr<Command> cmd) {
    commands_.push_back(std::move(cmd));
}

void MacroCommand::execute() {
    std::cout << "  >>> Executing macro: " << name_ << " <<<\n";
    for (auto& cmd : commands_) {
        cmd->execute();
    }
    std::cout << "  >>> Macro complete <<<\n";
}

// 关键：撤销宏命令时，按逆序撤销各子命令
void MacroCommand::undo() {
    std::cout << "  >>> Undoing macro: " << name_ << " <<<\n";
    for (auto it = commands_.rbegin(); it != commands_.rend(); ++it) {
        (*it)->undo();
    }
    std::cout << "  >>> Macro undo complete <<<\n";
}

std::string MacroCommand::description() const {
    return "[Macro] " + name_ + " (" + std::to_string(commands_.size()) + " commands)";
}

// ============================================================
// 调用者实现 — 管理命令的执行、撤销、重做
// ============================================================
void SmartHomeController::executeCommand(std::unique_ptr<Command> cmd) {
    std::cout << ">> Execute: " << cmd->description() << "\n";
    cmd->execute();
    history_.push_back(cmd->description());
    undoStack_.push(std::move(cmd));
    // 执行新命令后清空重做栈，因为历史分支已改变
    while (!redoStack_.empty()) {
        redoStack_.pop();
    }
}

void SmartHomeController::undo() {
    if (undoStack_.empty()) {
        std::cout << ">> Nothing to undo\n";
        return;
    }
    auto cmd = std::move(undoStack_.top());
    undoStack_.pop();
    std::cout << ">> Undo: " << cmd->description() << "\n";
    cmd->undo();
    history_.push_back("[UNDO] " + cmd->description());
    redoStack_.push(std::move(cmd));
}

void SmartHomeController::redo() {
    if (redoStack_.empty()) {
        std::cout << ">> Nothing to redo\n";
        return;
    }
    auto cmd = std::move(redoStack_.top());
    redoStack_.pop();
    std::cout << ">> Redo: " << cmd->description() << "\n";
    cmd->execute();
    history_.push_back("[REDO] " + cmd->description());
    undoStack_.push(std::move(cmd));
}

void SmartHomeController::showHistory() const {
    std::cout << "\n=== Command History ===\n";
    for (size_t i = 0; i < history_.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << history_[i] << "\n";
    }
    std::cout << "=======================\n";
}

size_t SmartHomeController::historySize() const {
    return history_.size();
}

// ============================================================
// main() — 演示智能家居命令模式的完整用法
// ============================================================
int main() {
    std::cout << "============================================\n";
    std::cout << " Command Pattern - Smart Home Automation\n";
    std::cout << "============================================\n\n";

    // 创建接收者（智能设备）
    auto livingRoomLight = std::make_shared<LightSystem>("Living Room");
    auto bedroomLight = std::make_shared<LightSystem>("Bedroom");
    auto thermostat = std::make_shared<Thermostat>("Main Floor");
    auto music = std::make_shared<MusicSystem>();

    // 创建调用者（控制器）
    SmartHomeController controller;

    // --- 1. 基本命令执行 ---
    std::cout << "--- 1. Basic Commands ---\n";
    controller.executeCommand(std::make_unique<LightOnCommand>(livingRoomLight));
    controller.executeCommand(std::make_unique<LightBrightnessCommand>(livingRoomLight, 70));
    controller.executeCommand(std::make_unique<ThermostatSetTempCommand>(thermostat, 24.0));
    controller.executeCommand(std::make_unique<MusicPlayCommand>(music, "Evening Jazz"));

    // --- 2. 撤销操作 ---
    std::cout << "\n--- 2. Undo Operations ---\n";
    controller.undo(); // 撤销播放音乐
    controller.undo(); // 撤销温度设置

    // --- 3. 重做操作 ---
    std::cout << "\n--- 3. Redo Operations ---\n";
    controller.redo(); // 重做温度设置

    // --- 4. 宏命令：一键"电影模式" ---
    std::cout << "\n--- 4. Macro Command: Movie Mode ---\n";
    auto movieMode = std::make_unique<MacroCommand>("Movie Mode");
    movieMode->addCommand(std::make_unique<LightBrightnessCommand>(livingRoomLight, 20));
    movieMode->addCommand(std::make_unique<ThermostatSetTempCommand>(thermostat, 23.0));
    movieMode->addCommand(std::make_unique<MusicPlayCommand>(music, "Movie Soundtrack"));
    movieMode->addCommand(std::make_unique<MusicVolumeCommand>(music, 30));
    controller.executeCommand(std::move(movieMode));

    // --- 5. 撤销整个宏命令 ---
    std::cout << "\n--- 5. Undo Entire Macro ---\n";
    controller.undo();

    // --- 6. 宏命令："晚安模式" ---
    std::cout << "\n--- 6. Macro Command: Good Night Mode ---\n";
    auto goodNightMode = std::make_unique<MacroCommand>("Good Night");
    goodNightMode->addCommand(std::make_unique<LightOffCommand>(livingRoomLight));
    goodNightMode->addCommand(std::make_unique<LightOffCommand>(bedroomLight));
    goodNightMode->addCommand(std::make_unique<ThermostatSetTempCommand>(thermostat, 20.0));
    goodNightMode->addCommand(std::make_unique<ThermostatModeCommand>(thermostat, "heating"));
    goodNightMode->addCommand(std::make_unique<MusicStopCommand>(music));
    controller.executeCommand(std::move(goodNightMode));

    // --- 7. 显示完整历史 ---
    controller.showHistory();

    std::cout << "\nTotal commands in history: " << controller.historySize() << "\n";
    std::cout << "\n============================================\n";
    std::cout << " Command Pattern Demo Complete\n";
    std::cout << "============================================\n";

    return 0;
}
```

## 构建方式

```cmake title="CMakeLists.txt"
cmake_minimum_required(VERSION 3.17)
add_executable(Pattern_Command Command.cpp Command.h)
target_compile_features(Pattern_Command PRIVATE cxx_std_17)
```

:::tip 编译运行
```bash
cd build
cmake --build . --target Pattern_Command
./Pattern_Command
```
:::
