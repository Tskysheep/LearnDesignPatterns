---
sidebar_position: 2
title: 源代码
description: FactoryMethod 模式的完整实现代码
---

# 源代码

## 头文件（FactoryMethod.h）

```cpp title="FactoryMethod.h" showLineNumbers
#pragma once
#include <string>
#include <memory>
#include <vector>
#include <sstream>

// ============================================================
// 工厂方法模式 (Factory Method Pattern)
// 场景：文档导出系统 —— 不同格式（PDF、HTML、Markdown）的导出器
//       通过工厂方法创建，客户端无需知道具体导出器类型
// ============================================================

// --- 产品接口：文档导出器 ---
// 所有导出器的抽象基类，定义统一的导出接口
class DocumentExporter {
public:
    virtual ~DocumentExporter() = default;

    // 设置文档标题
    virtual void setTitle(const std::string& title) = 0;
    // 添加段落内容
    virtual void addParagraph(const std::string& text) = 0;
    // 添加代码块
    virtual void addCodeBlock(const std::string& code, const std::string& language = "") = 0;
    // 生成最终导出内容
    virtual std::string render() const = 0;
    // 获取文件扩展名
    virtual std::string getFileExtension() const = 0;
};

// --- 具体产品：PDF 导出器 ---
class PdfExporter : public DocumentExporter {
public:
    void setTitle(const std::string& title) override;
    void addParagraph(const std::string& text) override;
    void addCodeBlock(const std::string& code, const std::string& language) override;
    std::string render() const override;
    std::string getFileExtension() const override;

private:
    std::string title_;
    // 模拟 PDF 内部指令流
    std::vector<std::string> instructions_;
};

// --- 具体产品：HTML 导出器 ---
class HtmlExporter : public DocumentExporter {
public:
    void setTitle(const std::string& title) override;
    void addParagraph(const std::string& text) override;
    void addCodeBlock(const std::string& code, const std::string& language) override;
    std::string render() const override;
    std::string getFileExtension() const override;

private:
    std::string title_;
    std::vector<std::string> bodyElements_;
};

// --- 具体产品：Markdown 导出器 ---
class MarkdownExporter : public DocumentExporter {
public:
    void setTitle(const std::string& title) override;
    void addParagraph(const std::string& text) override;
    void addCodeBlock(const std::string& code, const std::string& language) override;
    std::string render() const override;
    std::string getFileExtension() const override;

private:
    std::string title_;
    std::vector<std::string> sections_;
};

// --- 创建者抽象类：文档导出服务 ---
// 核心：定义工厂方法 createExporter()，子类决定创建哪种导出器
// 模板方法 exportDocument() 调用工厂方法获取产品，然后执行通用导出流程
class ExportService {
public:
    virtual ~ExportService() = default;

    // 工厂方法：由子类实现，返回具体的导出器
    virtual std::unique_ptr<DocumentExporter> createExporter() const = 0;

    // 模板方法：使用工厂方法创建导出器，完成文档导出
    std::string exportDocument(const std::string& title,
                               const std::vector<std::string>& paragraphs) const;
};

// --- 具体创建者 ---
class PdfExportService : public ExportService {
public:
    std::unique_ptr<DocumentExporter> createExporter() const override;
};

class HtmlExportService : public ExportService {
public:
    std::unique_ptr<DocumentExporter> createExporter() const override;
};

class MarkdownExportService : public ExportService {
public:
    std::unique_ptr<DocumentExporter> createExporter() const override;
};
```

## 实现文件（FactoryMethod.cpp）

```cpp title="FactoryMethod.cpp" showLineNumbers
#include "FactoryMethod.h"
#include <iostream>
#include <algorithm>

// ============================================================
// 工厂方法模式 —— 文档导出系统实现
// ============================================================

// ===================== PDF 导出器实现 =====================
void PdfExporter::setTitle(const std::string& title) {
    title_ = title;
    // 模拟 PDF 指令：设置标题字体和位置
    instructions_.push_back("BT /F1 24 Tf 100 750 Td (" + title + ") Tj ET");
}

void PdfExporter::addParagraph(const std::string& text) {
    instructions_.push_back("BT /F2 12 Tf 50 Td (" + text + ") Tj ET");
}

void PdfExporter::addCodeBlock(const std::string& code, const std::string& language) {
    instructions_.push_back("% Code block [" + language + "]");
    instructions_.push_back("BT /Courier 10 Tf 50 Td (" + code + ") Tj ET");
}

std::string PdfExporter::render() const {
    std::ostringstream oss;
    oss << "%PDF-1.4\n";
    oss << "% Document: " << title_ << "\n";
    for (const auto& instr : instructions_) {
        oss << instr << "\n";
    }
    oss << "%%EOF\n";
    return oss.str();
}

std::string PdfExporter::getFileExtension() const { return ".pdf"; }

// ===================== HTML 导出器实现 =====================
void HtmlExporter::setTitle(const std::string& title) {
    title_ = title;
}

void HtmlExporter::addParagraph(const std::string& text) {
    bodyElements_.push_back("  <p>" + text + "</p>");
}

void HtmlExporter::addCodeBlock(const std::string& code, const std::string& language) {
    bodyElements_.push_back("  <pre><code class=\"language-" + language + "\">" +
                            code + "</code></pre>");
}

std::string HtmlExporter::render() const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n<html>\n<head>\n";
    oss << "  <title>" << title_ << "</title>\n";
    oss << "</head>\n<body>\n";
    oss << "  <h1>" << title_ << "</h1>\n";
    for (const auto& elem : bodyElements_) {
        oss << elem << "\n";
    }
    oss << "</body>\n</html>\n";
    return oss.str();
}

std::string HtmlExporter::getFileExtension() const { return ".html"; }

// ===================== Markdown 导出器实现 =====================
void MarkdownExporter::setTitle(const std::string& title) {
    title_ = title;
    sections_.push_back("# " + title);
}

void MarkdownExporter::addParagraph(const std::string& text) {
    sections_.push_back(text);
}

void MarkdownExporter::addCodeBlock(const std::string& code, const std::string& language) {
    sections_.push_back("```" + language + "\n" + code + "\n```");
}

std::string MarkdownExporter::render() const {
    std::ostringstream oss;
    for (size_t i = 0; i < sections_.size(); ++i) {
        oss << sections_[i];
        if (i + 1 < sections_.size()) oss << "\n\n";
    }
    oss << "\n";
    return oss.str();
}

std::string MarkdownExporter::getFileExtension() const { return ".md"; }

// ===================== ExportService 模板方法 =====================
// 关键点：exportDocument 调用工厂方法 createExporter()，
// 而不是直接 new 具体产品。客户端代码仅依赖抽象接口。
std::string ExportService::exportDocument(
    const std::string& title,
    const std::vector<std::string>& paragraphs) const
{
    // 调用工厂方法 —— 子类决定创建哪种导出器
    auto exporter = createExporter();

    exporter->setTitle(title);
    for (const auto& para : paragraphs) {
        exporter->addParagraph(para);
    }
    exporter->addCodeBlock("int main() { return 0; }", "cpp");

    std::string result = exporter->render();
    std::cout << "[ExportService] Exported to *" << exporter->getFileExtension()
              << " (" << result.size() << " bytes)\n";
    return result;
}

// ===================== 具体工厂方法实现 =====================
std::unique_ptr<DocumentExporter> PdfExportService::createExporter() const {
    return std::make_unique<PdfExporter>();
}

std::unique_ptr<DocumentExporter> HtmlExportService::createExporter() const {
    return std::make_unique<HtmlExporter>();
}

std::unique_ptr<DocumentExporter> MarkdownExportService::createExporter() const {
    return std::make_unique<MarkdownExporter>();
}

// ===================== 客户端代码 =====================
// 演示：客户端通过 ExportService 引用操作，完全不知道具体产品类型
void clientCode(const ExportService& service) {
    std::vector<std::string> content = {
        "This document demonstrates the Factory Method pattern.",
        "Each export format is encapsulated behind a common interface.",
        "The client code never directly instantiates concrete exporters."
    };
    std::string output = service.exportDocument("Design Patterns Report", content);
    std::cout << "--- Output Preview ---\n" << output << "\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << " Factory Method Pattern Demo\n";
    std::cout << " Scenario: Document Export System\n";
    std::cout << "========================================\n\n";

    // 1) PDF 导出
    std::cout << ">>> PDF Export <<<\n";
    PdfExportService pdfService;
    clientCode(pdfService);

    // 2) HTML 导出
    std::cout << ">>> HTML Export <<<\n";
    HtmlExportService htmlService;
    clientCode(htmlService);

    // 3) Markdown 导出
    std::cout << ">>> Markdown Export <<<\n";
    MarkdownExportService mdService;
    clientCode(mdService);

    // 演示工厂方法的扩展性：新增格式只需新增子类，无需修改已有代码
    std::cout << "========================================\n";
    std::cout << "Key insight: Adding a new format (e.g., LaTeX)\n";
    std::cout << "requires only a new Exporter + ExportService subclass.\n";
    std::cout << "No existing code is modified (Open/Closed Principle).\n";
    std::cout << "========================================\n";

    return 0;
}
```

## 构建方式

```cmake title="CMakeLists.txt"
add_executable(Pattern_FactoryMethod FactoryMethod.cpp FactoryMethod.h)
target_compile_features(Pattern_FactoryMethod PRIVATE cxx_std_17)
```

:::tip 编译运行
```bash
cd build
cmake --build . --target Pattern_FactoryMethod
./Pattern_FactoryMethod
```
:::
