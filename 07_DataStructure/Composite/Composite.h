#ifndef COMPOSITE_H
#define COMPOSITE_H

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

// ============================================================
// 组合模式（Composite Pattern）
// 场景：文件系统树 —— 文件和目录共享同一接口，
//       目录可以包含子文件或子目录，形成树形递归结构。
// ============================================================

// 前向声明
class FileSystemVisitor;

// 抽象组件：文件系统节点
// 设计要点：叶子节点和容器节点的统一抽象接口
class FileSystemNode {
public:
    explicit FileSystemNode(const std::string& name) : name_(name) {}
    virtual ~FileSystemNode() = default;

    // 获取节点名称
    const std::string& getName() const { return name_; }

    // 获取节点大小（字节）—— 文件返回自身大小，目录返回所有子节点大小之和
    virtual size_t getSize() const = 0;

    // 显示节点信息（带缩进层级）
    virtual void display(int depth = 0) const = 0;

    // 搜索：按名称模糊匹配，返回所有匹配的路径
    virtual void search(const std::string& keyword,
                        const std::string& currentPath,
                        std::vector<std::string>& results) const = 0;

    // 子节点管理 —— 默认实现抛异常，仅目录重写
    virtual void add(std::shared_ptr<FileSystemNode> node);
    virtual void remove(const std::string& name);
    virtual std::shared_ptr<FileSystemNode> getChild(const std::string& name) const;

    // 判断是否为目录
    virtual bool isDirectory() const { return false; }

protected:
    std::string name_;
};

// 叶子节点：文件
class File : public FileSystemNode {
public:
    File(const std::string& name, size_t size, const std::string& extension);

    size_t getSize() const override;
    void display(int depth = 0) const override;
    void search(const std::string& keyword,
                const std::string& currentPath,
                std::vector<std::string>& results) const override;

    const std::string& getExtension() const { return extension_; }

private:
    size_t size_;          // 文件大小（字节）
    std::string extension_; // 文件扩展名
};

// 容器节点：目录
// 设计要点：持有子节点集合，递归地将操作委托给子节点
class Directory : public FileSystemNode {
public:
    explicit Directory(const std::string& name);

    size_t getSize() const override;
    void display(int depth = 0) const override;
    void search(const std::string& keyword,
                const std::string& currentPath,
                std::vector<std::string>& results) const override;

    // 重写子节点管理方法
    void add(std::shared_ptr<FileSystemNode> node) override;
    void remove(const std::string& name) override;
    std::shared_ptr<FileSystemNode> getChild(const std::string& name) const override;

    bool isDirectory() const override { return true; }

    // 获取子节点数量
    size_t getChildCount() const { return children_.size(); }

    // 统计目录下所有文件数量（递归）
    size_t countFiles() const;

    // 按扩展名统计文件
    void countByExtension(std::map<std::string, int>& stats) const;

private:
    // 使用 shared_ptr 管理子节点生命周期，支持共享引用
    std::vector<std::shared_ptr<FileSystemNode>> children_;
};

#endif // COMPOSITE_H
