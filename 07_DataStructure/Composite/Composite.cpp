#include "Composite.h"
#include <iostream>
#include <algorithm>
#include <map>
#include <iomanip>
#include <sstream>
#include <stdexcept>

// ============================================================
// FileSystemNode 基类 —— 默认的子节点管理实现
// 叶子节点调用这些方法时抛出异常，保证类型安全
// ============================================================

void FileSystemNode::add(std::shared_ptr<FileSystemNode> /*node*/) {
    throw std::runtime_error("Cannot add child to a leaf node: " + name_);
}

void FileSystemNode::remove(const std::string& /*name*/) {
    throw std::runtime_error("Cannot remove child from a leaf node: " + name_);
}

std::shared_ptr<FileSystemNode> FileSystemNode::getChild(const std::string& /*name*/) const {
    throw std::runtime_error("Leaf node has no children: " + name_);
}

// ============================================================
// File（叶子节点）实现
// ============================================================

File::File(const std::string& name, size_t size, const std::string& extension)
    : FileSystemNode(name), size_(size), extension_(extension) {}

size_t File::getSize() const {
    return size_;
}

void File::display(int depth) const {
    // 用缩进表示层级，文件用 "📄" 样式前缀
    std::string indent(depth * 2, ' ');
    std::cout << indent << "[File] " << name_ << "." << extension_
              << " (" << size_ << " bytes)" << std::endl;
}

void File::search(const std::string& keyword,
                  const std::string& currentPath,
                  std::vector<std::string>& results) const {
    std::string fullName = name_ + "." + extension_;
    // 简单的子串匹配搜索
    if (fullName.find(keyword) != std::string::npos) {
        results.push_back(currentPath + "/" + fullName);
    }
}

// ============================================================
// Directory（容器节点）实现
// 设计要点：所有操作递归委托给子节点，体现组合模式的透明性
// ============================================================

Directory::Directory(const std::string& name)
    : FileSystemNode(name) {}

size_t Directory::getSize() const {
    // 递归累加所有子节点大小
    size_t total = 0;
    for (const auto& child : children_) {
        total += child->getSize();
    }
    return total;
}

void Directory::display(int depth) const {
    std::string indent(depth * 2, ' ');
    std::cout << indent << "[Dir]  " << name_ << "/" << std::endl;
    // 递归显示所有子节点
    for (const auto& child : children_) {
        child->display(depth + 1);
    }
}

void Directory::search(const std::string& keyword,
                       const std::string& currentPath,
                       std::vector<std::string>& results) const {
    std::string path = currentPath + "/" + name_;
    for (const auto& child : children_) {
        child->search(keyword, path, results);
    }
}

void Directory::add(std::shared_ptr<FileSystemNode> node) {
    // 防止重名
    for (const auto& child : children_) {
        if (child->getName() == node->getName()) {
            throw std::runtime_error("Node already exists: " + node->getName());
        }
    }
    children_.push_back(std::move(node));
}

void Directory::remove(const std::string& name) {
    auto it = std::remove_if(children_.begin(), children_.end(),
        [&name](const std::shared_ptr<FileSystemNode>& node) {
            return node->getName() == name;
        });
    if (it == children_.end()) {
        throw std::runtime_error("Node not found: " + name);
    }
    children_.erase(it, children_.end());
}

std::shared_ptr<FileSystemNode> Directory::getChild(const std::string& name) const {
    for (const auto& child : children_) {
        if (child->getName() == name) {
            return child;
        }
    }
    return nullptr;
}

size_t Directory::countFiles() const {
    size_t count = 0;
    for (const auto& child : children_) {
        if (child->isDirectory()) {
            count += dynamic_cast<const Directory*>(child.get())->countFiles();
        } else {
            ++count;
        }
    }
    return count;
}

void Directory::countByExtension(std::map<std::string, int>& stats) const {
    for (const auto& child : children_) {
        if (child->isDirectory()) {
            dynamic_cast<const Directory*>(child.get())->countByExtension(stats);
        } else {
            auto* file = dynamic_cast<const File*>(child.get());
            if (file) {
                stats[file->getExtension()]++;
            }
        }
    }
}

// ============================================================
// 辅助函数：格式化文件大小
// ============================================================
static std::string formatSize(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    while (size >= 1024.0 && unitIndex < 3) {
        size /= 1024.0;
        ++unitIndex;
    }
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << size << " " << units[unitIndex];
    return oss.str();
}

// ============================================================
// main() —— 演示组合模式
// ============================================================
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Composite Pattern - File System Tree" << std::endl;
    std::cout << "========================================" << std::endl;

    // --- 构建文件系统树 ---
    auto root = std::make_shared<Directory>("project");

    // src 目录
    auto src = std::make_shared<Directory>("src");
    src->add(std::make_shared<File>("main", 2048, "cpp"));
    src->add(std::make_shared<File>("utils", 1536, "cpp"));
    src->add(std::make_shared<File>("utils", 512, "h"));

    // src/core 子目录
    auto core = std::make_shared<Directory>("core");
    core->add(std::make_shared<File>("engine", 4096, "cpp"));
    core->add(std::make_shared<File>("engine", 1024, "h"));
    core->add(std::make_shared<File>("config", 768, "json"));
    src->add(core);

    // docs 目录
    auto docs = std::make_shared<Directory>("docs");
    docs->add(std::make_shared<File>("README", 3072, "md"));
    docs->add(std::make_shared<File>("API", 5120, "md"));
    docs->add(std::make_shared<File>("architecture", 2048, "md"));

    // build 目录
    auto build = std::make_shared<Directory>("build");
    build->add(std::make_shared<File>("output", 10240, "exe"));
    build->add(std::make_shared<File>("debug", 8192, "log"));

    // 根目录文件
    root->add(src);
    root->add(docs);
    root->add(build);
    root->add(std::make_shared<File>("CMakeLists", 256, "txt"));
    root->add(std::make_shared<File>("LICENSE", 1100, "txt"));

    // --- 1. 显示文件系统树 ---
    std::cout << "\n[1] File System Tree Structure:" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    root->display();

    // --- 2. 统一接口计算大小 ---
    // 组合模式的核心优势：客户端无需区分文件和目录，统一调用 getSize()
    std::cout << "\n[2] Size Calculation (unified interface):" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout << "  Total project size: " << formatSize(root->getSize()) << std::endl;
    std::cout << "  src/ size:          " << formatSize(src->getSize()) << std::endl;
    std::cout << "  docs/ size:         " << formatSize(docs->getSize()) << std::endl;
    std::cout << "  build/ size:        " << formatSize(build->getSize()) << std::endl;

    // --- 3. 搜索功能 ---
    std::cout << "\n[3] Search Results:" << std::endl;
    std::cout << "--------------------------------" << std::endl;

    std::vector<std::string> results;
    root->search("engine", "", results);
    std::cout << "  Search 'engine':" << std::endl;
    for (const auto& path : results) {
        std::cout << "    " << path << std::endl;
    }

    results.clear();
    root->search(".md", "", results);
    std::cout << "  Search '.md':" << std::endl;
    for (const auto& path : results) {
        std::cout << "    " << path << std::endl;
    }

    // --- 4. 统计信息 ---
    std::cout << "\n[4] Statistics:" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    std::cout << "  Total files: " << root->countFiles() << std::endl;

    std::map<std::string, int> extStats;
    root->countByExtension(extStats);
    std::cout << "  Files by extension:" << std::endl;
    for (const auto& [ext, count] : extStats) {
        std::cout << "    ." << ext << ": " << count << " file(s)" << std::endl;
    }

    // --- 5. 动态修改树结构 ---
    std::cout << "\n[5] Dynamic Modification:" << std::endl;
    std::cout << "--------------------------------" << std::endl;

    // 添加新文件
    auto tests = std::make_shared<Directory>("tests");
    tests->add(std::make_shared<File>("test_engine", 1024, "cpp"));
    tests->add(std::make_shared<File>("test_utils", 768, "cpp"));
    root->add(tests);
    std::cout << "  Added tests/ directory with 2 test files." << std::endl;

    // 删除 build 目录
    root->remove("build");
    std::cout << "  Removed build/ directory." << std::endl;

    std::cout << "\n  Updated tree:" << std::endl;
    root->display();
    std::cout << "\n  New total size: " << formatSize(root->getSize()) << std::endl;
    std::cout << "  New file count: " << root->countFiles() << std::endl;

    // --- 6. 异常处理演示 ---
    std::cout << "\n[6] Error Handling:" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    try {
        auto file = std::make_shared<File>("test", 100, "txt");
        file->add(std::make_shared<File>("child", 50, "txt"));
    } catch (const std::runtime_error& e) {
        std::cout << "  Caught: " << e.what() << std::endl;
    }

    std::cout << "\n========================================" << std::endl;
    std::cout << "   Composite Pattern Demo Complete" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
