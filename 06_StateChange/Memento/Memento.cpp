#include "Memento.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

// ============================================================
// Selection 实现
// ============================================================
std::string Selection::toString() const {
    if (!active) return "(none)";
    std::ostringstream oss;
    oss << "[" << startLine << ":" << startColumn
        << " -> " << endLine << ":" << endColumn << "]";
    return oss.str();
}

// ============================================================
// EditorMemento 实现
// ============================================================
EditorMemento::EditorMemento(const std::string& content,
                             const CursorInfo& cursor,
                             const Selection& selection,
                             const std::string& description)
    : content_(content)
    , cursor_(cursor)
    , selection_(selection)
    , description_(description)
{
    // 生成时间戳
    auto now = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    timestamp_ = oss.str();
}

// ============================================================
// TextEditor（Originator）实现
// ============================================================
TextEditor::TextEditor()
    : content_("")
    , cursor_{0, 0}
    , selection_{0, 0, 0, 0, false}
{
}

void TextEditor::type(const std::string& text) {
    // 如果有选区，先删除选区内容（简化处理）
    if (selection_.active) {
        clearSelection();
    }
    content_ += text;
    cursor_.column += static_cast<int>(text.length());
}

void TextEditor::newLine() {
    content_ += "\n";
    cursor_.line++;
    cursor_.column = 0;
}

void TextEditor::deleteText(int count) {
    if (count <= 0 || content_.empty()) return;
    int toDelete = std::min(count, static_cast<int>(content_.size()));
    content_.erase(content_.size() - toDelete, toDelete);
    // 简化处理：光标回退
    cursor_.column = std::max(0, cursor_.column - toDelete);
}

void TextEditor::moveCursor(int line, int column) {
    cursor_.line = line;
    cursor_.column = column;
}

void TextEditor::select(int startLine, int startCol, int endLine, int endCol) {
    selection_ = {startLine, startCol, endLine, endCol, true};
}

void TextEditor::clearSelection() {
    selection_.active = false;
}

// 创建备忘录：将当前内部状态完整打包
std::unique_ptr<EditorMemento> TextEditor::createMemento(const std::string& description) const {
    // 使用 new 因为构造函数是 private，只有友元类能访问
    return std::unique_ptr<EditorMemento>(
        new EditorMemento(content_, cursor_, selection_, description));
}

// 从备忘录恢复：用备忘录中保存的状态覆盖当前状态
void TextEditor::restoreFromMemento(const EditorMemento& memento) {
    content_   = memento.content_;
    cursor_    = memento.cursor_;
    selection_ = memento.selection_;
}

void TextEditor::display() const {
    std::cout << "  Content  : \"" << content_ << "\"\n";
    std::cout << "  Cursor   : line=" << cursor_.line
              << ", col=" << cursor_.column << "\n";
    std::cout << "  Selection: " << selection_.toString() << "\n";
}

// ============================================================
// EditorHistory（Caretaker）实现
// ============================================================
EditorHistory::EditorHistory(TextEditor& editor, size_t maxHistory)
    : editor_(editor)
    , maxHistory_(maxHistory)
{
}

void EditorHistory::save(const std::string& description) {
    // 每次新操作后，清空重做栈（与主流编辑器行为一致）
    redoStack_.clear();

    // 保存当前状态的快照
    undoStack_.push_back(editor_.createMemento(description));

    // 超出最大历史记录数时，丢弃最早的记录
    if (undoStack_.size() > maxHistory_) {
        undoStack_.erase(undoStack_.begin());
    }
}

bool EditorHistory::undo() {
    if (undoStack_.empty()) {
        std::cout << "  [Nothing to undo]\n";
        return false;
    }

    // 先保存当前状态到重做栈，然后恢复上一个状态
    redoStack_.push_back(editor_.createMemento("before-undo"));

    auto& memento = undoStack_.back();
    std::cout << "  [Undo -> \"" << memento->getDescription()
              << "\" at " << memento->getTimestamp() << "]\n";
    editor_.restoreFromMemento(*memento);
    undoStack_.pop_back();

    return true;
}

bool EditorHistory::redo() {
    if (redoStack_.empty()) {
        std::cout << "  [Nothing to redo]\n";
        return false;
    }

    // 先保存当前状态到撤销栈，然后恢复重做栈顶的状态
    undoStack_.push_back(editor_.createMemento("before-redo"));

    auto& memento = redoStack_.back();
    std::cout << "  [Redo -> restoring state at " << memento->getTimestamp() << "]\n";
    editor_.restoreFromMemento(*memento);
    redoStack_.pop_back();

    return true;
}

void EditorHistory::showHistory() const {
    std::cout << "\n  === Edit History ===\n";
    std::cout << "  Undo stack (" << undoStack_.size() << " entries):\n";
    for (size_t i = 0; i < undoStack_.size(); ++i) {
        std::cout << "    [" << i << "] " << undoStack_[i]->getDescription()
                  << " (" << undoStack_[i]->getTimestamp() << ")\n";
    }
    std::cout << "  Redo stack (" << redoStack_.size() << " entries):\n";
    for (size_t i = 0; i < redoStack_.size(); ++i) {
        std::cout << "    [" << i << "] (at " << redoStack_[i]->getTimestamp() << ")\n";
    }
    std::cout << "  ====================\n";
}

// ============================================================
// main() —— 演示文本编辑器的撤销/重做功能
// ============================================================
int main() {
    std::cout << "========================================\n";
    std::cout << "  Memento Pattern - Text Editor Demo\n";
    std::cout << "========================================\n\n";

    TextEditor editor;
    EditorHistory history(editor);

    // --- Step 1: 输入标题 ---
    std::cout << "[Step 1] Type title\n";
    editor.type("Design Patterns");
    history.save("type-title");
    editor.display();

    // --- Step 2: 换行并输入正文 ---
    std::cout << "\n[Step 2] New line + type body\n";
    editor.newLine();
    editor.type("Memento captures and restores object state.");
    history.save("type-body");
    editor.display();

    // --- Step 3: 选中部分文本 ---
    std::cout << "\n[Step 3] Select text range\n";
    editor.select(1, 0, 1, 7);
    history.save("select-text");
    editor.display();

    // --- Step 4: 继续编辑 ---
    std::cout << "\n[Step 4] Append more text\n";
    editor.clearSelection();
    editor.type(" It belongs to behavioral patterns.");
    history.save("append-text");
    editor.display();

    // 显示完整历史
    history.showHistory();

    // --- Step 5: 撤销操作 ---
    std::cout << "\n[Step 5] Undo (x2)\n";
    history.undo();
    editor.display();
    std::cout << "\n";
    history.undo();
    editor.display();

    // --- Step 6: 重做操作 ---
    std::cout << "\n[Step 6] Redo (x1)\n";
    history.redo();
    editor.display();

    // 显示最终历史
    history.showHistory();

    // --- Step 7: 在撤销后进行新编辑（重做栈被清空） ---
    std::cout << "\n[Step 7] New edit after undo (redo stack cleared)\n";
    editor.type(" -- EDITED");
    history.save("new-edit-after-undo");
    editor.display();
    history.showHistory();

    // --- Step 8: 连续撤销直到栈空 ---
    std::cout << "\n[Step 8] Undo all remaining\n";
    while (history.undoCount() > 0) {
        history.undo();
    }
    editor.display();

    // 尝试再撤销
    std::cout << "\n[Step 9] Try undo on empty stack\n";
    history.undo();

    std::cout << "\n========================================\n";
    std::cout << "  Demo complete.\n";
    std::cout << "========================================\n";

    return 0;
}
