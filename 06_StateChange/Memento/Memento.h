#pragma once
#include <string>
#include <vector>
#include <memory>
#include <ctime>
#include <iostream>

// ============================================================
// 备忘录模式（Memento Pattern）—— 文本编辑器撤销/重做
// 核心思想：在不破坏封装性的前提下，捕获并保存对象的内部状态，
//          以便日后可以将对象恢复到先前的状态。
// ============================================================

// 前向声明
class EditorMemento;

// ------------------------------------------------------------
// 编辑器光标与选区信息
// ------------------------------------------------------------
struct CursorInfo {
    int line = 0;       // 光标所在行
    int column = 0;     // 光标所在列
};

struct Selection {
    int startLine = 0;
    int startColumn = 0;
    int endLine = 0;
    int endColumn = 0;
    bool active = false; // 是否有选区

    std::string toString() const;
};

// ------------------------------------------------------------
// Memento（备忘录）：存储编辑器的内部状态快照
// 设计要点：备忘录对外只暴露窄接口（获取时间戳等元信息），
//          而将宽接口（获取实际状态数据）仅对 Originator 开放。
//          C++ 中通过友元类实现这一机制。
// ------------------------------------------------------------
class EditorMemento {
public:
    // 窄接口：外部只能看到元信息
    std::string getTimestamp() const { return timestamp_; }
    std::string getDescription() const { return description_; }

private:
    // 宽接口：只有 TextEditor（Originator）可以访问
    friend class TextEditor;

    EditorMemento(const std::string& content,
                  const CursorInfo& cursor,
                  const Selection& selection,
                  const std::string& description);

    std::string content_;       // 文本内容
    CursorInfo cursor_;         // 光标位置
    Selection selection_;       // 选区信息
    std::string timestamp_;     // 创建时间
    std::string description_;   // 操作描述
};

// ------------------------------------------------------------
// Originator（原发器）：TextEditor —— 负责创建和恢复备忘录
// 编辑器拥有需要保存/恢复的内部状态
// ------------------------------------------------------------
class TextEditor {
public:
    TextEditor();

    // 文本编辑操作
    void type(const std::string& text);
    void newLine();
    void deleteText(int count);
    void moveCursor(int line, int column);
    void select(int startLine, int startCol, int endLine, int endCol);
    void clearSelection();

    // 备忘录接口：创建快照 / 从快照恢复
    std::unique_ptr<EditorMemento> createMemento(const std::string& description) const;
    void restoreFromMemento(const EditorMemento& memento);

    // 显示当前状态
    void display() const;

    // 获取当前文本
    std::string getContent() const { return content_; }
    CursorInfo getCursor() const { return cursor_; }

private:
    std::string content_;
    CursorInfo cursor_;
    Selection selection_;
};

// ------------------------------------------------------------
// Caretaker（管理者）：EditorHistory —— 管理备忘录的存取
// 设计要点：Caretaker 只负责保管备忘录，不能查看或修改其内容。
//          同时维护 undo/redo 两个栈来支持撤销和重做。
// ------------------------------------------------------------
class EditorHistory {
public:
    explicit EditorHistory(TextEditor& editor, size_t maxHistory = 50);

    // 保存当前状态到历史记录
    void save(const std::string& description);

    // 撤销：回退到上一个状态
    bool undo();

    // 重做：前进到下一个状态
    bool redo();

    // 显示历史记录
    void showHistory() const;

    // 查询
    size_t undoCount() const { return undoStack_.size(); }
    size_t redoCount() const { return redoStack_.size(); }

private:
    TextEditor& editor_;
    std::vector<std::unique_ptr<EditorMemento>> undoStack_;  // 撤销栈
    std::vector<std::unique_ptr<EditorMemento>> redoStack_;  // 重做栈
    size_t maxHistory_;  // 最大历史记录数，防止内存无限增长
};
