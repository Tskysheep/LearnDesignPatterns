#include "Proxy.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

// ============================================================
// 真实对象实现
// ============================================================

HighResolutionImage::HighResolutionImage(const std::string& filename)
    : filename_(filename) {
    // 构造时立即加载 —— 这就是代价高昂的操作
    loadFromDisk();
}

void HighResolutionImage::loadFromDisk() {
    std::cout << "    [加载] 正在从磁盘加载高分辨率图片: " << filename_ << std::endl;
    // 模拟磁盘 I/O 延迟（实际项目中这可能是几百 MB 的图片）
    std::cout << "    [加载] 读取图片数据..." << std::endl;
    std::cout << "    [加载] 解码图片格式..." << std::endl;
    std::cout << "    [加载] 分配显存..." << std::endl;

    // 模拟不同图片的不同尺寸
    if (filename_.find("panorama") != std::string::npos) {
        width_ = 8000;
        height_ = 3000;
    } else if (filename_.find("portrait") != std::string::npos) {
        width_ = 2000;
        height_ = 3000;
    } else {
        width_ = 3840;
        height_ = 2160;
    }

    loaded_ = true;
    std::cout << "    [加载] 图片加载完成! 尺寸: "
              << width_ << "x" << height_ << std::endl;
}

void HighResolutionImage::display() {
    std::cout << "    [显示] 渲染高分辨率图片: " << filename_
              << " (" << width_ << "x" << height_ << ")" << std::endl;
}

// ============================================================
// 虚拟代理实现 —— 延迟加载
// ============================================================

VirtualImageProxy::VirtualImageProxy(const std::string& filename)
    : filename_(filename) {
    // 构造时不加载图片，只记录文件名
    // 这是虚拟代理的核心：推迟昂贵操作到真正需要时
    std::cout << "    [代理] 创建图片代理: " << filename
              << " (图片尚未加载)" << std::endl;
}

void VirtualImageProxy::display() {
    // 首次 display 时才加载真实图片
    if (!realImage_) {
        std::cout << "    [代理] 首次显示，触发加载..." << std::endl;
        realImage_ = std::make_unique<HighResolutionImage>(filename_);
    }
    realImage_->display();
}

int VirtualImageProxy::getWidth() const {
    ensureLoaded();
    return realImage_->getWidth();
}

int VirtualImageProxy::getHeight() const {
    ensureLoaded();
    return realImage_->getHeight();
}

void VirtualImageProxy::ensureLoaded() const {
    if (!realImage_) {
        std::cout << "    [代理] 需要图片信息，触发加载..." << std::endl;
        realImage_ = std::make_unique<HighResolutionImage>(filename_);
    }
}

// ============================================================
// 访问控制代理实现
// ============================================================

std::string roleToString(UserRole role) {
    switch (role) {
        case UserRole::Guest:   return "游客";
        case UserRole::Member:  return "会员";
        case UserRole::Premium: return "高级会员";
        default: return "未知";
    }
}

AccessControlImageProxy::AccessControlImageProxy(
    const std::string& filename, UserRole userRole)
    : filename_(filename), userRole_(userRole) {
    std::cout << "    [权限代理] 创建权限控制代理: " << filename
              << " (用户角色: " << roleToString(userRole) << ")" << std::endl;
}

void AccessControlImageProxy::display() {
    switch (userRole_) {
        case UserRole::Guest:
            // 游客只能看占位符
            std::cout << "    [权限代理] 用户角色: " << roleToString(userRole_)
                      << " -> 显示缩略占位符: [" << filename_ << " 的模糊预览]"
                      << std::endl;
            std::cout << "    [权限代理] 提示: 请升级会员以查看高清图片" << std::endl;
            break;

        case UserRole::Member:
            // 会员可以看标准质量
            std::cout << "    [权限代理] 用户角色: " << roleToString(userRole_)
                      << " -> 加载标准质量图片" << std::endl;
            if (!realImage_) {
                realImage_ = std::make_unique<HighResolutionImage>(filename_);
            }
            std::cout << "    [权限代理] 显示标准质量 (缩放到 50%)" << std::endl;
            break;

        case UserRole::Premium:
            // 高级会员可以看原始质量
            std::cout << "    [权限代理] 用户角色: " << roleToString(userRole_)
                      << " -> 加载原始质量图片" << std::endl;
            if (!realImage_) {
                realImage_ = std::make_unique<HighResolutionImage>(filename_);
            }
            realImage_->display();
            break;
    }
}

int AccessControlImageProxy::getWidth() const {
    // 所有角色都可以获取元数据
    return 3840;
}

int AccessControlImageProxy::getHeight() const {
    return 2160;
}

// ============================================================
// 缓存代理实现
// ============================================================

// 静态缓存池初始化
std::unordered_map<std::string, std::shared_ptr<HighResolutionImage>>
    CachingImageProxy::cache_;

CachingImageProxy::CachingImageProxy(const std::string& filename)
    : filename_(filename) {}

void CachingImageProxy::display() {
    auto it = cache_.find(filename_);
    if (it != cache_.end()) {
        std::cout << "    [缓存代理] 缓存命中: " << filename_ << std::endl;
        it->second->display();
    } else {
        std::cout << "    [缓存代理] 缓存未命中，加载并缓存: "
                  << filename_ << std::endl;
        auto image = std::make_shared<HighResolutionImage>(filename_);
        cache_[filename_] = image;
        image->display();
    }
}

int CachingImageProxy::getWidth() const {
    auto it = cache_.find(filename_);
    if (it != cache_.end()) {
        return it->second->getWidth();
    }
    return 0;  // 未加载时返回 0
}

int CachingImageProxy::getHeight() const {
    auto it = cache_.find(filename_);
    if (it != cache_.end()) {
        return it->second->getHeight();
    }
    return 0;
}

size_t CachingImageProxy::getCacheSize() {
    return cache_.size();
}

void CachingImageProxy::clearCache() {
    cache_.clear();
    std::cout << "    [缓存代理] 缓存已清空" << std::endl;
}

// ============================================================
// 客户端代码
// ============================================================
int main() {
    std::cout << "============================================" << std::endl;
    std::cout << "   Proxy 模式演示 - 图片加载代理" << std::endl;
    std::cout << "============================================" << std::endl;

    // ---- 场景1：虚拟代理（延迟加载） ----
    std::cout << "\n【场景1】虚拟代理 —— 延迟加载大图片" << std::endl;
    std::cout << "  创建 3 个图片代理（此时不加载任何图片）：\n" << std::endl;

    // 创建代理时不触发加载
    std::vector<std::unique_ptr<Image>> gallery;
    gallery.push_back(std::make_unique<VirtualImageProxy>("sunset_panorama.jpg"));
    gallery.push_back(std::make_unique<VirtualImageProxy>("family_portrait.jpg"));
    gallery.push_back(std::make_unique<VirtualImageProxy>("city_night.jpg"));

    std::cout << "\n  用户浏览相册，只点击了第 1 张：\n" << std::endl;
    // 只有被显示的图片才会被加载
    gallery[0]->display();

    std::cout << "\n  再次显示第 1 张（已缓存，不重复加载）：\n" << std::endl;
    gallery[0]->display();

    std::cout << "\n  用户又点击了第 3 张：\n" << std::endl;
    gallery[2]->display();

    std::cout << "\n  (第 2 张图片从未被加载 —— 节省了资源！)" << std::endl;

    // ---- 场景2：访问控制代理 ----
    std::cout << "\n\n【场景2】访问控制代理 —— 根据用户权限控制访问" << std::endl;

    std::string photoFile = "premium_wallpaper.jpg";

    std::cout << "\n  --- 游客尝试查看 ---\n" << std::endl;
    {
        AccessControlImageProxy guestProxy(photoFile, UserRole::Guest);
        guestProxy.display();
    }

    std::cout << "\n  --- 会员尝试查看 ---\n" << std::endl;
    {
        AccessControlImageProxy memberProxy(photoFile, UserRole::Member);
        memberProxy.display();
    }

    std::cout << "\n  --- 高级会员尝试查看 ---\n" << std::endl;
    {
        AccessControlImageProxy premiumProxy(photoFile, UserRole::Premium);
        premiumProxy.display();
    }

    // ---- 场景3：缓存代理 ----
    std::cout << "\n\n【场景3】缓存代理 —— 避免重复加载" << std::endl;
    std::cout << "  当前缓存大小: " << CachingImageProxy::getCacheSize() << std::endl;

    std::cout << "\n  第一次加载 landscape.jpg：\n" << std::endl;
    {
        CachingImageProxy proxy1("landscape.jpg");
        proxy1.display();
    }

    std::cout << "\n  第二次加载 landscape.jpg（命中缓存）：\n" << std::endl;
    {
        CachingImageProxy proxy2("landscape.jpg");
        proxy2.display();
    }

    std::cout << "\n  加载另一张 ocean_panorama.jpg：\n" << std::endl;
    {
        CachingImageProxy proxy3("ocean_panorama.jpg");
        proxy3.display();
    }

    std::cout << "\n  当前缓存大小: " << CachingImageProxy::getCacheSize() << std::endl;
    CachingImageProxy::clearCache();

    // ---- 场景4：多态使用（对客户端透明） ----
    std::cout << "\n\n【场景4】多态使用 —— 代理对客户端透明" << std::endl;
    std::cout << "  客户端代码完全不知道自己用的是代理还是真实对象：\n" << std::endl;

    // 客户端通过 Image 接口操作，不关心是代理还是真实对象
    auto showImage = [](Image& img) {
        std::cout << "  文件名: " << img.getFilename() << std::endl;
        img.display();
        std::cout << std::endl;
    };

    VirtualImageProxy proxyImg("transparent_test.jpg");
    showImage(proxyImg);

    std::cout << "\n============================================" << std::endl;
    std::cout << "  Proxy 模式要点总结：" << std::endl;
    std::cout << "  1. 虚拟代理：延迟创建昂贵对象，按需加载" << std::endl;
    std::cout << "  2. 访问控制代理：在不修改真实对象的前提下增加权限检查" << std::endl;
    std::cout << "  3. 缓存代理：缓存昂贵操作的结果" << std::endl;
    std::cout << "  4. 代理与真实对象实现相同接口，对客户端透明" << std::endl;
    std::cout << "============================================" << std::endl;

    return 0;
}
