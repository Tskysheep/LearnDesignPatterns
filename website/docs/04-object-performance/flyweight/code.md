---
sidebar_position: 2
title: 源代码
description: Flyweight 模式的完整实现代码
---

# 源代码

## 头文件（Flyweight.h）

```cpp title="Flyweight.h" showLineNumbers
#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <iostream>

// ============================================================
// Flyweight 模式 —— 文本编辑器字符渲染
// 运用共享技术有效地支持大量细粒度的对象
// ============================================================

// ---------- 内在状态（Intrinsic State）—— 可共享 ----------
// 字体样式：被大量字符共享，与字符位置无关
// 这是 Flyweight 对象，存储不随上下文变化的共享数据
class CharacterStyle {
public:
    CharacterStyle(const std::string& fontFamily,
                   int fontSize,
                   bool bold,
                   bool italic,
                   const std::string& color);

    // Flyweight 的操作接口：结合外在状态（位置）进行渲染
    void render(char character, int row, int col) const;

    // 用于调试和展示
    std::string describe() const;

    // 获取属性（用于 key 生成）
    const std::string& getFontFamily() const { return fontFamily_; }
    int getFontSize() const { return fontSize_; }
    bool isBold() const { return bold_; }
    bool isItalic() const { return italic_; }
    const std::string& getColor() const { return color_; }

private:
    // 内在状态 —— 所有使用此样式的字符共享这些属性
    std::string fontFamily_;  // 字体族
    int fontSize_;            // 字号
    bool bold_;               // 是否加粗
    bool italic_;             // 是否斜体
    std::string color_;       // 颜色
};


// ---------- Flyweight 工厂 ----------
// 确保相同样式只创建一个 CharacterStyle 对象
// 这是享元池（Flyweight Pool），也是模式的核心管理器
class StyleFactory {
public:
    // 获取或创建一个样式对象
    // 如果该样式组合已存在，直接返回共享实例；否则创建新实例
    std::shared_ptr<CharacterStyle> getStyle(
        const std::string& fontFamily,
        int fontSize,
        bool bold,
        bool italic,
        const std::string& color);

    // 查看享元池的状态
    size_t styleCount() const { return stylePool_.size(); }
    void printPoolStatus() const;

private:
    // 生成样式的唯一标识符
    static std::string makeKey(const std::string& fontFamily,
                               int fontSize,
                               bool bold,
                               bool italic,
                               const std::string& color);

    // 享元池：key -> 共享的样式对象
    std::unordered_map<std::string, std::shared_ptr<CharacterStyle>> stylePool_;
};


// ---------- 外在状态（Extrinsic State）—— 不可共享 ----------
// 每个字符有自己独立的位置和字符值，不能共享
// 这不是 Flyweight，而是使用 Flyweight 的客户端上下文
struct CharacterContext {
    char character;           // 字符内容
    int row;                  // 行号
    int col;                  // 列号
    std::shared_ptr<CharacterStyle> style;  // 指向共享的样式对象

    CharacterContext(char ch, int r, int c, std::shared_ptr<CharacterStyle> s)
        : character(ch), row(r), col(c), style(std::move(s)) {}

    void render() const;
};


// ---------- 文本编辑器（客户端）----------
// 管理所有字符，使用 StyleFactory 来共享样式
class TextEditor {
public:
    explicit TextEditor(const std::string& name);

    // 插入字符，指定样式属性
    void insertCharacter(char ch, int row, int col,
                         const std::string& fontFamily,
                         int fontSize,
                         bool bold, bool italic,
                         const std::string& color);

    // 渲染所有字符
    void renderAll() const;

    // 内存分析
    void printMemoryAnalysis() const;

    size_t characterCount() const { return characters_.size(); }

private:
    std::string name_;
    StyleFactory styleFactory_;
    std::vector<CharacterContext> characters_;
};
```

## 实现文件（Flyweight.cpp）

```cpp title="Flyweight.cpp" showLineNumbers
#include "Flyweight.h"
#include <sstream>
#include <iomanip>

// ============================================================
// CharacterStyle（Flyweight）实现
// ============================================================

CharacterStyle::CharacterStyle(const std::string& fontFamily,
                               int fontSize,
                               bool bold,
                               bool italic,
                               const std::string& color)
    : fontFamily_(fontFamily)
    , fontSize_(fontSize)
    , bold_(bold)
    , italic_(italic)
    , color_(color) {
    std::cout << "  [新建样式] " << describe() << std::endl;
}

void CharacterStyle::render(char character, int row, int col) const {
    std::cout << "    渲染 '" << character << "' 于 ("
              << row << "," << col << ") "
              << "样式=[" << describe() << "]" << std::endl;
}

std::string CharacterStyle::describe() const {
    std::ostringstream oss;
    oss << fontFamily_ << " " << fontSize_ << "pt";
    if (bold_) oss << " 粗体";
    if (italic_) oss << " 斜体";
    oss << " " << color_;
    return oss.str();
}


// ============================================================
// StyleFactory（Flyweight 工厂）实现
// ============================================================

std::shared_ptr<CharacterStyle> StyleFactory::getStyle(
    const std::string& fontFamily,
    int fontSize,
    bool bold,
    bool italic,
    const std::string& color) {

    std::string key = makeKey(fontFamily, fontSize, bold, italic, color);

    auto it = stylePool_.find(key);
    if (it != stylePool_.end()) {
        // 命中缓存 —— 直接返回已有的共享对象
        return it->second;
    }

    // 未命中 —— 创建新的样式对象并加入池中
    auto style = std::make_shared<CharacterStyle>(fontFamily, fontSize, bold, italic, color);
    stylePool_[key] = style;
    return style;
}

void StyleFactory::printPoolStatus() const {
    std::cout << "[StyleFactory] 享元池中共有 " << stylePool_.size()
              << " 个唯一样式对象:" << std::endl;
    int idx = 1;
    for (const auto& [key, style] : stylePool_) {
        std::cout << "  " << idx++ << ". " << style->describe()
                  << " (引用计数: " << style.use_count() << ")" << std::endl;
    }
}

std::string StyleFactory::makeKey(const std::string& fontFamily,
                                   int fontSize,
                                   bool bold,
                                   bool italic,
                                   const std::string& color) {
    // 将所有内在状态属性组合为唯一的 key
    std::ostringstream oss;
    oss << fontFamily << "|" << fontSize << "|" << bold << "|" << italic << "|" << color;
    return oss.str();
}


// ============================================================
// CharacterContext 实现
// ============================================================

void CharacterContext::render() const {
    if (style) {
        style->render(character, row, col);
    }
}


// ============================================================
// TextEditor（客户端）实现
// ============================================================

TextEditor::TextEditor(const std::string& name) : name_(name) {
    std::cout << "[TextEditor] 创建编辑器: " << name_ << std::endl;
}

void TextEditor::insertCharacter(char ch, int row, int col,
                                  const std::string& fontFamily,
                                  int fontSize,
                                  bool bold, bool italic,
                                  const std::string& color) {
    // 通过工厂获取共享的样式对象（可能是新建的，也可能是复用的）
    auto style = styleFactory_.getStyle(fontFamily, fontSize, bold, italic, color);
    characters_.emplace_back(ch, row, col, std::move(style));
}

void TextEditor::renderAll() const {
    std::cout << "\n[TextEditor] 渲染文档 \"" << name_ << "\" 中的 "
              << characters_.size() << " 个字符:" << std::endl;
    for (const auto& ctx : characters_) {
        ctx.render();
    }
}

void TextEditor::printMemoryAnalysis() const {
    std::cout << "\n[内存分析] 文档 \"" << name_ << "\":" << std::endl;
    std::cout << "  字符总数: " << characters_.size() << std::endl;
    std::cout << "  唯一样式数: " << styleFactory_.styleCount() << std::endl;

    // 计算不使用 Flyweight 时的内存开销（估算）
    // 每个 CharacterStyle 约占 5 个字段的内存
    size_t styleObjectSize = sizeof(std::string) * 2 + sizeof(int) + sizeof(bool) * 2;
    size_t withoutFlyweight = characters_.size() * (sizeof(CharacterContext) + styleObjectSize);
    size_t withFlyweight = characters_.size() * sizeof(CharacterContext)
                         + styleFactory_.styleCount() * styleObjectSize;

    std::cout << "  不使用 Flyweight 的估算内存: ~" << withoutFlyweight << " 字节" << std::endl;
    std::cout << "  使用 Flyweight 的估算内存:   ~" << withFlyweight << " 字节" << std::endl;

    if (withoutFlyweight > 0) {
        double saved = 100.0 * (1.0 - static_cast<double>(withFlyweight) / withoutFlyweight);
        std::cout << "  节省内存约: " << std::fixed << std::setprecision(1)
                  << saved << "%" << std::endl;
    }

    styleFactory_.printPoolStatus();
}


// ============================================================
// 演示程序
// ============================================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Flyweight 模式演示" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ---- 1. 基本演示：文本编辑器 ----
    std::cout << "--- 1. 文本编辑器字符渲染 ---\n" << std::endl;

    TextEditor editor("设计模式讲义.txt");

    // 模拟输入标题 "Hello" —— 标题使用粗体、大号字
    std::cout << "\n插入标题 \"Hello\" (Arial 24pt 粗体 黑色):" << std::endl;
    editor.insertCharacter('H', 0, 0, "Arial", 24, true, false, "#000000");
    editor.insertCharacter('e', 0, 1, "Arial", 24, true, false, "#000000");
    editor.insertCharacter('l', 0, 2, "Arial", 24, true, false, "#000000");
    editor.insertCharacter('l', 0, 3, "Arial", 24, true, false, "#000000");
    editor.insertCharacter('o', 0, 4, "Arial", 24, true, false, "#000000");

    // 模拟输入正文 "World" —— 正文使用普通字体
    std::cout << "\n插入正文 \"World\" (SimSun 12pt 常规 黑色):" << std::endl;
    editor.insertCharacter('W', 1, 0, "SimSun", 12, false, false, "#333333");
    editor.insertCharacter('o', 1, 1, "SimSun", 12, false, false, "#333333");
    editor.insertCharacter('r', 1, 2, "SimSun", 12, false, false, "#333333");
    editor.insertCharacter('l', 1, 3, "SimSun", 12, false, false, "#333333");
    editor.insertCharacter('d', 1, 4, "SimSun", 12, false, false, "#333333");

    // 模拟输入注释 "Note" —— 注释使用斜体、灰色
    std::cout << "\n插入注释 \"Note\" (Arial 10pt 斜体 灰色):" << std::endl;
    editor.insertCharacter('N', 2, 0, "Arial", 10, false, true, "#999999");
    editor.insertCharacter('o', 2, 1, "Arial", 10, false, true, "#999999");
    editor.insertCharacter('t', 2, 2, "Arial", 10, false, true, "#999999");
    editor.insertCharacter('e', 2, 3, "Arial", 10, false, true, "#999999");

    // 渲染文档
    editor.renderAll();

    // 内存分析
    editor.printMemoryAnalysis();

    // ---- 2. 大规模文档演示 ----
    std::cout << "\n\n--- 2. 大规模文档模拟 ---\n" << std::endl;

    TextEditor bigDoc("长篇文档.txt");

    // 模拟一篇有 1000 个字符的文档，但只用了 4 种样式
    std::cout << "插入 1000 个字符（使用 4 种样式）..." << std::endl;
    const char* sampleText = "Design patterns are typical solutions to common problems. ";
    int textLen = static_cast<int>(std::string(sampleText).length());

    // 样式定义
    struct StyleDef {
        std::string font;
        int size;
        bool bold, italic;
        std::string color;
    };

    std::vector<StyleDef> styles = {
        {"Arial",    16, true,  false, "#000000"},   // 标题
        {"SimSun",   12, false, false, "#333333"},   // 正文
        {"Consolas", 11, false, false, "#2E86C1"},   // 代码
        {"Arial",    10, false, true,  "#999999"},   // 注释
    };

    for (int i = 0; i < 1000; ++i) {
        char ch = sampleText[i % textLen];
        int row = i / 80;
        int col = i % 80;
        const auto& s = styles[i % styles.size()];
        bigDoc.insertCharacter(ch, row, col, s.font, s.size, s.bold, s.italic, s.color);
    }

    bigDoc.printMemoryAnalysis();

    // ---- 3. 要点总结 ----
    std::cout << "\n========================================" << std::endl;
    std::cout << "关键点总结:" << std::endl;
    std::cout << "1. 内在状态（字体、字号、颜色）存储在共享的 Flyweight 对象中" << std::endl;
    std::cout << "2. 外在状态（字符值、行列位置）存储在每个 Context 中" << std::endl;
    std::cout << "3. StyleFactory 充当享元池，确保相同样式只创建一次" << std::endl;
    std::cout << "4. 1000 个字符仅需 4 个样式对象，大幅节省内存" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
```

## 构建方式

```cmake title="CMakeLists.txt"
add_executable(Pattern_Flyweight Flyweight.cpp Flyweight.h)
```

:::tip 编译运行
```bash
cd build
cmake --build . --target Pattern_Flyweight
./Pattern_Flyweight
```
:::