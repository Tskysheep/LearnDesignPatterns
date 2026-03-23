#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <chrono>
#include <thread>

// ============================================================
// Proxy 模式 - 大图片虚拟代理 + 访问控制代理
// 核心思想：为其他对象提供一个替身或占位符，
//          以控制对这个对象的访问
// ============================================================

// -------------------- 抽象接口 --------------------

// 图片接口 —— Subject 角色
// 设计要点：代理和真实对象实现相同的接口，对客户端透明
class Image {
public:
    virtual ~Image() = default;
    virtual void display() = 0;
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
    virtual std::string getFilename() const = 0;
};

// -------------------- 真实对象 --------------------

// 高分辨率图片 —— RealSubject 角色
// 模拟从磁盘加载大型图片的过程（耗时操作）
class HighResolutionImage : public Image {
public:
    explicit HighResolutionImage(const std::string& filename);

    void display() override;
    int getWidth() const override { return width_; }
    int getHeight() const override { return height_; }
    std::string getFilename() const override { return filename_; }

private:
    void loadFromDisk();  // 模拟耗时的磁盘加载

    std::string filename_;
    int width_ = 0;
    int height_ = 0;
    bool loaded_ = false;
};

// -------------------- 虚拟代理 --------------------

// 虚拟代理 —— 延迟加载大图片
// 设计要点：在真正需要显示图片之前，不创建昂贵的真实对象
//          只在调用 display() 时才触发实际加载
class VirtualImageProxy : public Image {
public:
    explicit VirtualImageProxy(const std::string& filename);

    void display() override;
    int getWidth() const override;
    int getHeight() const override;
    std::string getFilename() const override { return filename_; }

private:
    // 确保真实图片已加载（惰性初始化）
    void ensureLoaded() const;

    std::string filename_;
    // mutable：在 const 方法中也能延迟初始化
    mutable std::unique_ptr<HighResolutionImage> realImage_;
};

// -------------------- 访问控制代理 --------------------

// 用户角色枚举
enum class UserRole {
    Guest,      // 游客 —— 只能查看缩略图
    Member,     // 会员 —— 可以查看标准图
    Premium     // 高级会员 —— 可以查看原图
};

std::string roleToString(UserRole role);

// 访问控制代理 —— 根据用户权限控制图片访问
// 设计要点：在不修改真实对象的前提下，增加权限检查逻辑
class AccessControlImageProxy : public Image {
public:
    AccessControlImageProxy(const std::string& filename, UserRole userRole);

    void display() override;
    int getWidth() const override;
    int getHeight() const override;
    std::string getFilename() const override { return filename_; }

    void setUserRole(UserRole role) { userRole_ = role; }

private:
    std::string filename_;
    UserRole userRole_;
    std::unique_ptr<HighResolutionImage> realImage_;
};

// -------------------- 缓存代理（附加演示） --------------------

// 缓存代理 —— 缓存已加载的图片，避免重复加载
// 设计要点：维护一个缓存池，相同文件名的图片只加载一次
class CachingImageProxy : public Image {
public:
    explicit CachingImageProxy(const std::string& filename);

    void display() override;
    int getWidth() const override;
    int getHeight() const override;
    std::string getFilename() const override { return filename_; }

    // 查看缓存状态
    static size_t getCacheSize();
    static void clearCache();

private:
    std::string filename_;

    // 静态缓存池 —— 所有 CachingImageProxy 共享
    static std::unordered_map<std::string, std::shared_ptr<HighResolutionImage>> cache_;
};
