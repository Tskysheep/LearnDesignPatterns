#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <functional>

// ============================================================
// 访问者模式（Visitor Pattern）— 文档元素处理系统
// 核心思想：在不修改元素类的前提下，定义作用于元素的新操作
// 双重分派（Double Dispatch）是实现的关键机制
// ============================================================

// 前置声明
class TextElement;
class ImageElement;
class TableElement;

// ---- 访问者接口（Visitor Interface） ----
// 为每种元素类型定义一个 visit 重载
// 新增操作 = 新增一个 Visitor 子类，无需修改任何 Element 类
class DocumentVisitor {
public:
    virtual ~DocumentVisitor() = default;
    virtual void visit(TextElement& element) = 0;
    virtual void visit(ImageElement& element) = 0;
    virtual void visit(TableElement& element) = 0;
};

// ---- 元素接口（Element Interface） ----
// accept() 方法是双重分派的第二步
class DocumentElement {
public:
    virtual ~DocumentElement() = default;
    virtual void accept(DocumentVisitor& visitor) = 0;
    virtual std::string typeName() const = 0;
};

// ---- 具体元素（Concrete Elements） ----

// 文本元素
class TextElement : public DocumentElement {
public:
    TextElement(const std::string& text, const std::string& fontName = "Arial",
                int fontSize = 12, bool bold = false, bool italic = false);

    void accept(DocumentVisitor& visitor) override;
    std::string typeName() const override;

    // 文本属性访问
    const std::string& getText() const;
    void setText(const std::string& text);
    const std::string& getFontName() const;
    int getFontSize() const;
    bool isBold() const;
    bool isItalic() const;

private:
    std::string text_;
    std::string fontName_;
    int fontSize_;
    bool bold_;
    bool italic_;
};

// 图片元素
class ImageElement : public DocumentElement {
public:
    ImageElement(const std::string& filePath, int width, int height,
                 const std::string& altText = "");

    void accept(DocumentVisitor& visitor) override;
    std::string typeName() const override;

    const std::string& getFilePath() const;
    int getWidth() const;
    int getHeight() const;
    const std::string& getAltText() const;
    std::string getFormat() const; // 从文件路径推断格式

private:
    std::string filePath_;
    int width_;
    int height_;
    std::string altText_;
};

// 表格元素
class TableElement : public DocumentElement {
public:
    TableElement(int rows, int cols, const std::string& caption = "");

    void accept(DocumentVisitor& visitor) override;
    std::string typeName() const override;

    void setCell(int row, int col, const std::string& value);
    std::string getCell(int row, int col) const;
    void setHeader(int col, const std::string& header);
    std::string getHeader(int col) const;

    int getRows() const;
    int getCols() const;
    const std::string& getCaption() const;

private:
    int rows_;
    int cols_;
    std::string caption_;
    std::vector<std::string> headers_;
    std::vector<std::vector<std::string>> data_;
};

// ---- 具体访问者（Concrete Visitors） ----

// 1. HTML 渲染访问者 — 将文档元素渲染为 HTML
class HtmlRenderVisitor : public DocumentVisitor {
public:
    void visit(TextElement& element) override;
    void visit(ImageElement& element) override;
    void visit(TableElement& element) override;

    std::string getResult() const;
    void clear();

private:
    std::ostringstream output_;
};

// 2. Markdown 导出访问者 — 将文档元素导出为 Markdown
class MarkdownExportVisitor : public DocumentVisitor {
public:
    void visit(TextElement& element) override;
    void visit(ImageElement& element) override;
    void visit(TableElement& element) override;

    std::string getResult() const;
    void clear();

private:
    std::ostringstream output_;
};

// 3. 纯文本提取访问者 — 提取所有可索引的文本内容
class PlainTextExtractVisitor : public DocumentVisitor {
public:
    void visit(TextElement& element) override;
    void visit(ImageElement& element) override;
    void visit(TableElement& element) override;

    std::string getResult() const;
    void clear();

private:
    std::ostringstream output_;
};

// 4. 文档统计访问者 — 统计文档的各种指标
class DocumentStatsVisitor : public DocumentVisitor {
public:
    void visit(TextElement& element) override;
    void visit(ImageElement& element) override;
    void visit(TableElement& element) override;

    void printStats() const;

    int getTextCount() const;
    int getImageCount() const;
    int getTableCount() const;
    int getTotalWords() const;
    int getTotalCells() const;

private:
    int textCount_ = 0;
    int imageCount_ = 0;
    int tableCount_ = 0;
    int totalWords_ = 0;
    int totalCells_ = 0;
    long long totalImagePixels_ = 0;
};

// 5. 拼写检查访问者 — 检查文本元素中的拼写错误
class SpellCheckVisitor : public DocumentVisitor {
public:
    SpellCheckVisitor();

    void visit(TextElement& element) override;
    void visit(ImageElement& element) override;
    void visit(TableElement& element) override;

    void printReport() const;
    int getErrorCount() const;

private:
    // 简化的拼写检查：维护一个已知单词集合
    std::vector<std::string> knownWords_;
    std::vector<std::pair<std::string, std::string>> errors_; // {context, misspelled_word}

    std::vector<std::string> splitWords(const std::string& text) const;
    bool isKnownWord(const std::string& word) const;
};

// ---- 文档容器 — 管理一组元素 ----
class Document {
public:
    explicit Document(const std::string& title);

    void addElement(std::unique_ptr<DocumentElement> element);

    // 对所有元素应用访问者
    void accept(DocumentVisitor& visitor);

    const std::string& getTitle() const;
    size_t elementCount() const;

private:
    std::string title_;
    std::vector<std::unique_ptr<DocumentElement>> elements_;
};
