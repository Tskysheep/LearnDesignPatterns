#include "Facade.h"
#include <iostream>
#include <string>

// ============================================================
// 子系统实现 —— 每个子系统都有自己独立的复杂接口
// ============================================================

// -------------------- DvdPlayer --------------------
void DvdPlayer::on() {
    std::cout << "  [DVD] 播放器已开机" << std::endl;
}

void DvdPlayer::off() {
    std::cout << "  [DVD] 播放器已关机" << std::endl;
}

void DvdPlayer::play(const std::string& movie) {
    currentMovie_ = movie;
    std::cout << "  [DVD] 正在播放: \"" << movie << "\"" << std::endl;
}

void DvdPlayer::stop() {
    std::cout << "  [DVD] 停止播放: \"" << currentMovie_ << "\"" << std::endl;
    currentMovie_.clear();
}

void DvdPlayer::eject() {
    std::cout << "  [DVD] 弹出光盘" << std::endl;
}

// -------------------- Projector --------------------
void Projector::on() {
    std::cout << "  [投影仪] 已开机" << std::endl;
}

void Projector::off() {
    std::cout << "  [投影仪] 已关机" << std::endl;
}

void Projector::setInput(const std::string& source) {
    std::cout << "  [投影仪] 输入源切换为: " << source << std::endl;
}

void Projector::setWideScreenMode() {
    std::cout << "  [投影仪] 切换到宽屏模式 (16:9)" << std::endl;
}

void Projector::setStandardMode() {
    std::cout << "  [投影仪] 切换到标准模式 (4:3)" << std::endl;
}

// -------------------- Amplifier --------------------
void Amplifier::on() {
    std::cout << "  [功放] 已开机" << std::endl;
}

void Amplifier::off() {
    std::cout << "  [功放] 已关机" << std::endl;
}

void Amplifier::setVolume(int level) {
    volume_ = level;
    std::cout << "  [功放] 音量设置为: " << level << std::endl;
}

void Amplifier::setSurroundSound() {
    std::cout << "  [功放] 切换到环绕声模式" << std::endl;
}

void Amplifier::setStereoSound() {
    std::cout << "  [功放] 切换到立体声模式" << std::endl;
}

void Amplifier::setInput(const std::string& source) {
    std::cout << "  [功放] 输入源切换为: " << source << std::endl;
}

// -------------------- Screen --------------------
void Screen::down() {
    std::cout << "  [幕布] 幕布已放下" << std::endl;
}

void Screen::up() {
    std::cout << "  [幕布] 幕布已收起" << std::endl;
}

// -------------------- TheaterLights --------------------
void TheaterLights::on() {
    brightness_ = 100;
    std::cout << "  [灯光] 灯光已打开" << std::endl;
}

void TheaterLights::off() {
    brightness_ = 0;
    std::cout << "  [灯光] 灯光已关闭" << std::endl;
}

void TheaterLights::dim(int level) {
    brightness_ = level;
    std::cout << "  [灯光] 灯光调暗至 " << level << "%" << std::endl;
}

// -------------------- StreamingPlayer --------------------
void StreamingPlayer::on() {
    std::cout << "  [流媒体] 播放器已开机" << std::endl;
}

void StreamingPlayer::off() {
    std::cout << "  [流媒体] 播放器已关机" << std::endl;
}

void StreamingPlayer::play(const std::string& content) {
    currentContent_ = content;
    std::cout << "  [流媒体] 正在播放: \"" << content << "\"" << std::endl;
}

void StreamingPlayer::pause() {
    std::cout << "  [流媒体] 暂停播放" << std::endl;
}

void StreamingPlayer::stop() {
    std::cout << "  [流媒体] 停止播放: \"" << currentContent_ << "\"" << std::endl;
    currentContent_.clear();
}

// ============================================================
// Facade 实现 —— 将复杂的子系统操作编排为简单的高层接口
// ============================================================

HomeTheaterFacade::HomeTheaterFacade(DvdPlayer& dvd, Projector& projector,
                                     Amplifier& amplifier, Screen& screen,
                                     TheaterLights& lights, StreamingPlayer& streaming)
    : dvd_(dvd), projector_(projector), amplifier_(amplifier),
      screen_(screen), lights_(lights), streaming_(streaming) {}

// 一键观影：协调 6 个子系统的 10+ 步操作
void HomeTheaterFacade::watchMovie(const std::string& movie) {
    std::cout << "\n>>> 准备观影模式..." << std::endl;

    // 调暗灯光营造氛围
    lights_.dim(10);

    // 放下幕布
    screen_.down();

    // 打开投影仪并设置宽屏
    projector_.on();
    projector_.setInput("DVD");
    projector_.setWideScreenMode();

    // 打开功放并设置环绕声
    amplifier_.on();
    amplifier_.setInput("DVD");
    amplifier_.setSurroundSound();
    amplifier_.setVolume(7);

    // 打开 DVD 播放器并播放电影
    dvd_.on();
    dvd_.play(movie);

    std::cout << ">>> 享受您的电影吧！" << std::endl;
}

void HomeTheaterFacade::endMovie() {
    std::cout << "\n>>> 正在关闭影院系统..." << std::endl;

    dvd_.stop();
    dvd_.eject();
    dvd_.off();

    amplifier_.off();

    projector_.off();

    screen_.up();

    lights_.on();

    std::cout << ">>> 影院系统已关闭" << std::endl;
}

void HomeTheaterFacade::watchStreaming(const std::string& content) {
    std::cout << "\n>>> 准备流媒体模式..." << std::endl;

    lights_.dim(15);
    screen_.down();

    projector_.on();
    projector_.setInput("Streaming");
    projector_.setWideScreenMode();

    amplifier_.on();
    amplifier_.setInput("Streaming");
    amplifier_.setSurroundSound();
    amplifier_.setVolume(6);

    streaming_.on();
    streaming_.play(content);

    std::cout << ">>> 享受您的节目吧！" << std::endl;
}

void HomeTheaterFacade::endStreaming() {
    std::cout << "\n>>> 正在关闭流媒体模式..." << std::endl;

    streaming_.stop();
    streaming_.off();
    amplifier_.off();
    projector_.off();
    screen_.up();
    lights_.on();

    std::cout << ">>> 流媒体模式已关闭" << std::endl;
}

void HomeTheaterFacade::listenToMusic() {
    std::cout << "\n>>> 准备音乐模式..." << std::endl;

    // 音乐模式不需要投影仪和幕布
    lights_.dim(40);

    amplifier_.on();
    amplifier_.setInput("Streaming");
    amplifier_.setStereoSound();
    amplifier_.setVolume(5);

    streaming_.on();
    streaming_.play("Spotify 播放列表");

    std::cout << ">>> 享受音乐吧！" << std::endl;
}

void HomeTheaterFacade::endMusic() {
    std::cout << "\n>>> 正在关闭音乐模式..." << std::endl;

    streaming_.stop();
    streaming_.off();
    amplifier_.off();
    lights_.on();

    std::cout << ">>> 音乐模式已关闭" << std::endl;
}

// ============================================================
// 客户端代码
// ============================================================
int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "   Facade 模式演示 - 家庭影院系统" << std::endl;
    std::cout << "============================================" << std::endl;

    // 创建各子系统实例
    DvdPlayer dvd;
    Projector projector;
    Amplifier amplifier;
    Screen screen;
    TheaterLights lights;
    StreamingPlayer streaming;

    // 创建 Facade —— 客户端只需与 Facade 交互
    HomeTheaterFacade homeTheater(dvd, projector, amplifier,
                                  screen, lights, streaming);

    // 场景1：使用 Facade 一键观影
    std::cout << "\n【场景1】使用 Facade 一键观影" << std::endl;
    homeTheater.watchMovie("盗梦空间");
    homeTheater.endMovie();

    // 场景2：使用 Facade 一键流媒体
    std::cout << "\n【场景2】使用 Facade 一键看流媒体" << std::endl;
    homeTheater.watchStreaming("Netflix: 三体");
    homeTheater.endStreaming();

    // 场景3：使用 Facade 一键音乐模式
    std::cout << "\n【场景3】使用 Facade 一键听音乐" << std::endl;
    homeTheater.listenToMusic();
    homeTheater.endMusic();

    // 场景4：直接访问子系统（Facade 不阻止直接访问）
    std::cout << "\n【场景4】绕过 Facade 直接控制子系统" << std::endl;
    std::cout << "  (Facade 模式不强制使用，客户端仍可直接操作子系统)\n" << std::endl;
    lights.dim(50);
    amplifier.on();
    amplifier.setStereoSound();
    amplifier.setVolume(3);
    amplifier.off();
    lights.on();

    std::cout << "\n============================================" << std::endl;
    std::cout << "  Facade 模式要点总结：" << std::endl;
    std::cout << "  1. 为复杂子系统提供简单统一的接口" << std::endl;
    std::cout << "  2. 不阻止客户端直接访问子系统" << std::endl;
    std::cout << "  3. 减少客户端与子系统之间的耦合" << std::endl;
    std::cout << "  4. 符合最少知识原则(迪米特法则)" << std::endl;
    std::cout << "============================================" << std::endl;

    return 0;
}
