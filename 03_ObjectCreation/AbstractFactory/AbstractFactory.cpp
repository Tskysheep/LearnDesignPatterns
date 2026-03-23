#include "AbstractFactory.h"
#include <iostream>
#include <stdexcept>

// ============================================================
// 抽象工厂模式 —— 跨平台 UI 工具包实现
// ============================================================

// ===================== Windows 组件实现 =====================
void WindowsButton::render() const {
    std::cout << "  [Win Button] Rendering flat rectangular button with Segoe UI font\n";
}
void WindowsButton::onClick(const std::string& handler) const {
    std::cout << "  [Win Button] Click -> invoking handler: " << handler << "\n";
}
std::string WindowsButton::getStyleInfo() const { return "Windows Fluent Design"; }

void WindowsTextBox::render() const {
    std::cout << "  [Win TextBox] Rendering bordered input field, text=\"" << text_ << "\"\n";
}
void WindowsTextBox::setText(const std::string& text) { text_ = text; }
std::string WindowsTextBox::getStyleInfo() const { return "Windows Fluent TextBox"; }

void WindowsCheckbox::render() const {
    std::cout << "  [Win Checkbox] [" << (checked_ ? "X" : " ") << "] square checkbox\n";
}
void WindowsCheckbox::toggle() { checked_ = !checked_; }
bool WindowsCheckbox::isChecked() const { return checked_; }
std::string WindowsCheckbox::getStyleInfo() const { return "Windows Fluent Checkbox"; }

// ===================== macOS 组件实现 =====================
void MacButton::render() const {
    std::cout << "  [Mac Button] Rendering rounded Aqua-style button with SF Pro font\n";
}
void MacButton::onClick(const std::string& handler) const {
    std::cout << "  [Mac Button] Click -> invoking handler: " << handler << "\n";
}
std::string MacButton::getStyleInfo() const { return "macOS Aqua Design"; }

void MacTextBox::render() const {
    std::cout << "  [Mac TextBox] Rendering rounded-corner input, text=\"" << text_ << "\"\n";
}
void MacTextBox::setText(const std::string& text) { text_ = text; }
std::string MacTextBox::getStyleInfo() const { return "macOS Aqua TextBox"; }

void MacCheckbox::render() const {
    std::cout << "  [Mac Checkbox] " << (checked_ ? "(v)" : "( )") << " rounded checkbox\n";
}
void MacCheckbox::toggle() { checked_ = !checked_; }
bool MacCheckbox::isChecked() const { return checked_; }
std::string MacCheckbox::getStyleInfo() const { return "macOS Aqua Checkbox"; }

// ===================== Linux 组件实现 =====================
void LinuxButton::render() const {
    std::cout << "  [GTK Button] Rendering GTK-themed button with Liberation Sans font\n";
}
void LinuxButton::onClick(const std::string& handler) const {
    std::cout << "  [GTK Button] Click -> invoking handler: " << handler << "\n";
}
std::string LinuxButton::getStyleInfo() const { return "Linux GTK/Adwaita Design"; }

void LinuxTextBox::render() const {
    std::cout << "  [GTK TextBox] Rendering GTK entry widget, text=\"" << text_ << "\"\n";
}
void LinuxTextBox::setText(const std::string& text) { text_ = text; }
std::string LinuxTextBox::getStyleInfo() const { return "Linux GTK TextBox"; }

void LinuxCheckbox::render() const {
    std::cout << "  [GTK Checkbox] " << (checked_ ? "[*]" : "[ ]") << " GTK checkbox\n";
}
void LinuxCheckbox::toggle() { checked_ = !checked_; }
bool LinuxCheckbox::isChecked() const { return checked_; }
std::string LinuxCheckbox::getStyleInfo() const { return "Linux GTK Checkbox"; }

// ===================== 具体工厂实现 =====================
// 每个工厂保证创建的组件属于同一视觉风格族
std::unique_ptr<Button> WindowsUIFactory::createButton() const {
    return std::make_unique<WindowsButton>();
}
std::unique_ptr<TextBox> WindowsUIFactory::createTextBox() const {
    return std::make_unique<WindowsTextBox>();
}
std::unique_ptr<Checkbox> WindowsUIFactory::createCheckbox() const {
    return std::make_unique<WindowsCheckbox>();
}
std::string WindowsUIFactory::getThemeName() const { return "Windows"; }

std::unique_ptr<Button> MacUIFactory::createButton() const {
    return std::make_unique<MacButton>();
}
std::unique_ptr<TextBox> MacUIFactory::createTextBox() const {
    return std::make_unique<MacTextBox>();
}
std::unique_ptr<Checkbox> MacUIFactory::createCheckbox() const {
    return std::make_unique<MacCheckbox>();
}
std::string MacUIFactory::getThemeName() const { return "macOS"; }

std::unique_ptr<Button> LinuxUIFactory::createButton() const {
    return std::make_unique<LinuxButton>();
}
std::unique_ptr<TextBox> LinuxUIFactory::createTextBox() const {
    return std::make_unique<LinuxTextBox>();
}
std::unique_ptr<Checkbox> LinuxUIFactory::createCheckbox() const {
    return std::make_unique<LinuxCheckbox>();
}
std::string LinuxUIFactory::getThemeName() const { return "Linux"; }

// 根据平台字符串创建对应工厂
std::unique_ptr<UIComponentFactory> createFactoryForPlatform(const std::string& platform) {
    if (platform == "Windows" || platform == "windows")
        return std::make_unique<WindowsUIFactory>();
    if (platform == "macOS" || platform == "macos" || platform == "Mac")
        return std::make_unique<MacUIFactory>();
    if (platform == "Linux" || platform == "linux")
        return std::make_unique<LinuxUIFactory>();
    throw std::invalid_argument("Unknown platform: " + platform);
}

// ===================== 客户端代码 =====================
// 关键点：客户端只依赖抽象接口，通过抽象工厂获取一组风格一致的组件
void buildLoginForm(const UIComponentFactory& factory) {
    std::cout << "Building login form with theme: " << factory.getThemeName() << "\n";

    auto usernameBox = factory.createTextBox();
    auto passwordBox = factory.createTextBox();
    auto rememberMe  = factory.createCheckbox();
    auto loginBtn    = factory.createButton();

    usernameBox->setText("admin@example.com");
    passwordBox->setText("********");
    rememberMe->toggle(); // 默认勾选"记住我"

    std::cout << "  -- Username Field --\n";
    usernameBox->render();

    std::cout << "  -- Password Field --\n";
    passwordBox->render();

    std::cout << "  -- Remember Me --\n";
    rememberMe->render();

    std::cout << "  -- Login Button --\n";
    loginBtn->render();
    loginBtn->onClick("onLoginSubmit()");

    std::cout << "  Style info: " << loginBtn->getStyleInfo() << "\n\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << " Abstract Factory Pattern Demo\n";
    std::cout << " Scenario: Cross-Platform UI Toolkit\n";
    std::cout << "========================================\n\n";

    // 演示：同一客户端代码，不同工厂产生不同风格的完整 UI
    std::string platforms[] = {"Windows", "macOS", "Linux"};

    for (const auto& platform : platforms) {
        std::cout << ">>> " << platform << " Theme <<<\n";
        auto factory = createFactoryForPlatform(platform);
        buildLoginForm(*factory);
    }

    // 重点说明
    std::cout << "========================================\n";
    std::cout << "Key insight: The client code (buildLoginForm) uses ONLY\n";
    std::cout << "abstract interfaces. Switching the entire UI theme requires\n";
    std::cout << "only changing which factory is passed in.\n";
    std::cout << "All components from one factory are guaranteed to be\n";
    std::cout << "visually consistent (same design language).\n";
    std::cout << "========================================\n";

    return 0;
}
