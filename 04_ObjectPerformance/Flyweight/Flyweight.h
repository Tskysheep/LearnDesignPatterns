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
