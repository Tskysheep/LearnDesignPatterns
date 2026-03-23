#pragma once
#include <string>
#include <memory>
#include <iostream>

// ============================================================
// Facade 模式 - 家庭影院系统
// 核心思想：为复杂子系统提供一个统一的高层接口，
//          使得子系统更容易使用
// ============================================================

// -------------------- 子系统类 --------------------

// DVD 播放器子系统
class DvdPlayer {
public:
    void on();
    void off();
    void play(const std::string& movie);
    void stop();
    void eject();

private:
    std::string currentMovie_;
};

// 投影仪子系统
class Projector {
public:
    void on();
    void off();
    void setInput(const std::string& source);
    void setWideScreenMode();
    void setStandardMode();
};

// 功放/音响子系统
class Amplifier {
public:
    void on();
    void off();
    void setVolume(int level);
    void setSurroundSound();
    void setStereoSound();
    void setInput(const std::string& source);

private:
    int volume_ = 0;
};

// 电动幕布子系统
class Screen {
public:
    void down();  // 放下幕布
    void up();    // 收起幕布
};

// 灯光子系统
class TheaterLights {
public:
    void on();
    void off();
    void dim(int level);  // 调暗灯光，level: 0-100

private:
    int brightness_ = 100;
};

// 流媒体播放器子系统（额外设备，展示子系统可扩展性）
class StreamingPlayer {
public:
    void on();
    void off();
    void play(const std::string& content);
    void pause();
    void stop();

private:
    std::string currentContent_;
};

// -------------------- Facade 类 --------------------

// 家庭影院外观类 —— 对外暴露简洁接口
// 设计要点：Facade 持有所有子系统的引用，但不拥有它们的生命周期
//          这样客户端仍可直接访问子系统进行高级操作
class HomeTheaterFacade {
public:
    // 构造函数接收所有子系统的引用
    // 设计决策：使用引用而非智能指针，表明 Facade 不管理子系统生命周期
    HomeTheaterFacade(DvdPlayer& dvd, Projector& projector,
                      Amplifier& amplifier, Screen& screen,
                      TheaterLights& lights, StreamingPlayer& streaming);

    // 简化接口：一键观影
    void watchMovie(const std::string& movie);

    // 简化接口：一键结束观影
    void endMovie();

    // 简化接口：一键流媒体模式
    void watchStreaming(const std::string& content);

    // 简化接口：一键结束流媒体
    void endStreaming();

    // 简化接口：一键听音乐模式
    void listenToMusic();

    // 简化接口：一键结束音乐
    void endMusic();

private:
    DvdPlayer& dvd_;
    Projector& projector_;
    Amplifier& amplifier_;
    Screen& screen_;
    TheaterLights& lights_;
    StreamingPlayer& streaming_;
};
