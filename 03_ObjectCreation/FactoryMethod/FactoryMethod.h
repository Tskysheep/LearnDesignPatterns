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
