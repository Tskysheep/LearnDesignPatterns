#pragma once
#include <string>
#include <memory>
#include <iostream>

// ============================================================
// 抽象工厂模式 (Abstract Factory Pattern)
// 场景：跨平台 UI 工具包 —— 为不同操作系统主题（Windows、macOS、Linux）
//       创建一族相关的 UI 组件（Button、TextBox、Checkbox）
// ============================================================

// --- 抽象产品 A：按钮 ---
class Button {
public:
    virtual ~Button() = default;
    virtual void render() const = 0;
    virtual void onClick(const std::string& handler) const = 0;
    virtual std::string getStyleInfo() const = 0;
};

// --- 抽象产品 B：文本框 ---
class TextBox {
public:
    virtual ~TextBox() = default;
    virtual void render() const = 0;
    virtual void setText(const std::string& text) = 0;
    virtual std::string getStyleInfo() const = 0;
};

// --- 抽象产品 C：复选框 ---
class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual void render() const = 0;
    virtual void toggle() = 0;
    virtual bool isChecked() const = 0;
    virtual std::string getStyleInfo() const = 0;
};

// ===================== Windows 风格组件 =====================
class WindowsButton : public Button {
public:
    void render() const override;
    void onClick(const std::string& handler) const override;
    std::string getStyleInfo() const override;
};

class WindowsTextBox : public TextBox {
public:
    void render() const override;
    void setText(const std::string& text) override;
    std::string getStyleInfo() const override;
private:
    std::string text_;
};

class WindowsCheckbox : public Checkbox {
public:
    void render() const override;
    void toggle() override;
    bool isChecked() const override;
    std::string getStyleInfo() const override;
private:
    bool checked_ = false;
};

// ===================== macOS 风格组件 =====================
class MacButton : public Button {
public:
    void render() const override;
    void onClick(const std::string& handler) const override;
    std::string getStyleInfo() const override;
};

class MacTextBox : public TextBox {
public:
    void render() const override;
    void setText(const std::string& text) override;
    std::string getStyleInfo() const override;
private:
    std::string text_;
};

class MacCheckbox : public Checkbox {
public:
    void render() const override;
    void toggle() override;
    bool isChecked() const override;
    std::string getStyleInfo() const override;
private:
    bool checked_ = false;
};

// ===================== Linux 风格组件 =====================
class LinuxButton : public Button {
public:
    void render() const override;
    void onClick(const std::string& handler) const override;
    std::string getStyleInfo() const override;
};

class LinuxTextBox : public TextBox {
public:
    void render() const override;
    void setText(const std::string& text) override;
    std::string getStyleInfo() const override;
private:
    std::string text_;
};

class LinuxCheckbox : public Checkbox {
public:
    void render() const override;
    void toggle() override;
    bool isChecked() const override;
    std::string getStyleInfo() const override;
private:
    bool checked_ = false;
};

// --- 抽象工厂：UI 组件工厂 ---
// 核心：每个方法创建一族产品中的一个成员
// 保证同一工厂创建的组件在视觉风格上一致
class UIComponentFactory {
public:
    virtual ~UIComponentFactory() = default;

    virtual std::unique_ptr<Button> createButton() const = 0;
    virtual std::unique_ptr<TextBox> createTextBox() const = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() const = 0;

    virtual std::string getThemeName() const = 0;
};

// --- 具体工厂 ---
class WindowsUIFactory : public UIComponentFactory {
public:
    std::unique_ptr<Button> createButton() const override;
    std::unique_ptr<TextBox> createTextBox() const override;
    std::unique_ptr<Checkbox> createCheckbox() const override;
    std::string getThemeName() const override;
};

class MacUIFactory : public UIComponentFactory {
public:
    std::unique_ptr<Button> createButton() const override;
    std::unique_ptr<TextBox> createTextBox() const override;
    std::unique_ptr<Checkbox> createCheckbox() const override;
    std::string getThemeName() const override;
};

class LinuxUIFactory : public UIComponentFactory {
public:
    std::unique_ptr<Button> createButton() const override;
    std::unique_ptr<TextBox> createTextBox() const override;
    std::unique_ptr<Checkbox> createCheckbox() const override;
    std::string getThemeName() const override;
};

// --- 辅助函数：根据平台名称获取对应工厂 ---
std::unique_ptr<UIComponentFactory> createFactoryForPlatform(const std::string& platform);
