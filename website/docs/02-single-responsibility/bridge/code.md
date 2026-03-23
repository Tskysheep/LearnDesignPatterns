---
sidebar_position: 2
title: 源代码
description: Bridge 模式的完整实现代码
---

# 源代码

## 头文件（Bridge.h）

```cpp title="Bridge.h" showLineNumbers
#ifndef BRIDGE_H
#define BRIDGE_H

#include <memory>
#include <string>
#include <vector>

// ============================================================
// 桥接模式 - 跨平台消息系统
// 核心思想：将"消息类型"和"发送平台"这两个独立变化的维度分离
// 每个维度各自演化，互不干扰（单一职责）
// ============================================================

// ============================================================
// 实现维度：消息发送平台（Implementor）
// 定义平台相关的底层发送操作
// ============================================================
class MessagePlatform {
public:
    virtual ~MessagePlatform() = default;

    // 平台名称
    virtual std::string platformName() const = 0;

    // 底层发送能力——不同平台有不同的实现
    virtual bool sendText(const std::string& recipient,
                          const std::string& title,
                          const std::string& body) = 0;

    virtual bool sendBinary(const std::string& recipient,
                            const std::string& title,
                            const std::vector<uint8_t>& data,
                            const std::string& mimeType) = 0;

    // 平台特有的状态查询
    virtual size_t maxPayloadBytes() const = 0;
};

// 具体实现：电子邮件平台
class EmailPlatform : public MessagePlatform {
public:
    explicit EmailPlatform(const std::string& smtpServer);
    std::string platformName() const override;
    bool sendText(const std::string& recipient,
                  const std::string& title,
                  const std::string& body) override;
    bool sendBinary(const std::string& recipient,
                    const std::string& title,
                    const std::vector<uint8_t>& data,
                    const std::string& mimeType) override;
    size_t maxPayloadBytes() const override;

private:
    std::string smtpServer_;
};

// 具体实现：短信平台
class SmsPlatform : public MessagePlatform {
public:
    explicit SmsPlatform(const std::string& gateway);
    std::string platformName() const override;
    bool sendText(const std::string& recipient,
                  const std::string& title,
                  const std::string& body) override;
    bool sendBinary(const std::string& recipient,
                    const std::string& title,
                    const std::vector<uint8_t>& data,
                    const std::string& mimeType) override;
    size_t maxPayloadBytes() const override;

private:
    std::string gateway_;
};

// 具体实现：推送通知平台
class PushNotificationPlatform : public MessagePlatform {
public:
    explicit PushNotificationPlatform(const std::string& appId);
    std::string platformName() const override;
    bool sendText(const std::string& recipient,
                  const std::string& title,
                  const std::string& body) override;
    bool sendBinary(const std::string& recipient,
                    const std::string& title,
                    const std::vector<uint8_t>& data,
                    const std::string& mimeType) override;
    size_t maxPayloadBytes() const override;

private:
    std::string appId_;
};

// ============================================================
// 抽象维度：消息类型（Abstraction）
// 定义高层的消息发送逻辑，将底层操作委托给平台
// 桥接的关键：持有 MessagePlatform 的共享指针
// ============================================================
class Message {
public:
    explicit Message(std::shared_ptr<MessagePlatform> platform);
    virtual ~Message() = default;

    // 发送消息——子类实现具体的消息组装逻辑
    virtual bool send(const std::string& recipient) = 0;

    // 消息类型描述
    virtual std::string messageType() const = 0;

    // 允许运行时切换平台——体现桥接模式的灵活性
    void setPlatform(std::shared_ptr<MessagePlatform> platform);
    std::string getPlatformName() const;

protected:
    // 子类通过此指针调用平台的底层能力
    std::shared_ptr<MessagePlatform> platform_;
};

// 细化抽象：文本消息
class TextMessage : public Message {
public:
    TextMessage(std::shared_ptr<MessagePlatform> platform,
                const std::string& title,
                const std::string& content);
    bool send(const std::string& recipient) override;
    std::string messageType() const override;

private:
    std::string title_;
    std::string content_;
};

// 细化抽象：图片消息
class ImageMessage : public Message {
public:
    ImageMessage(std::shared_ptr<MessagePlatform> platform,
                 const std::string& caption,
                 const std::string& imagePath,
                 size_t imageSize);
    bool send(const std::string& recipient) override;
    std::string messageType() const override;

private:
    std::string caption_;
    std::string imagePath_;
    size_t imageSize_;
    // 模拟图片数据
    std::vector<uint8_t> simulateImageData() const;
};

// 细化抽象：视频消息
class VideoMessage : public Message {
public:
    VideoMessage(std::shared_ptr<MessagePlatform> platform,
                 const std::string& title,
                 const std::string& videoPath,
                 size_t videoSize,
                 int durationSeconds);
    bool send(const std::string& recipient) override;
    std::string messageType() const override;

private:
    std::string title_;
    std::string videoPath_;
    size_t videoSize_;
    int durationSeconds_;
    std::vector<uint8_t> simulateVideoData() const;
};

#endif // BRIDGE_H
```

## 实现文件（Bridge.cpp）

```cpp title="Bridge.cpp" showLineNumbers
#include "Bridge.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

// ============================================================
// EmailPlatform 实现
// ============================================================

EmailPlatform::EmailPlatform(const std::string& smtpServer)
    : smtpServer_(smtpServer) {}

std::string EmailPlatform::platformName() const {
    return "Email(" + smtpServer_ + ")";
}

bool EmailPlatform::sendText(const std::string& recipient,
                              const std::string& title,
                              const std::string& body) {
    std::cout << "    [Email] 通过 SMTP 服务器 " << smtpServer_ << " 发送邮件\n"
              << "    [Email] 收件人: " << recipient << "\n"
              << "    [Email] 主题: " << title << "\n"
              << "    [Email] 正文: " << body << "\n"
              << "    [Email] 发送成功 (支持富文本格式)\n";
    return true;
}

bool EmailPlatform::sendBinary(const std::string& recipient,
                                const std::string& title,
                                const std::vector<uint8_t>& data,
                                const std::string& mimeType) {
    std::cout << "    [Email] 通过 SMTP 服务器 " << smtpServer_ << " 发送附件邮件\n"
              << "    [Email] 收件人: " << recipient << "\n"
              << "    [Email] 主题: " << title << "\n"
              << "    [Email] 附件类型: " << mimeType
              << " (" << data.size() << " 字节)\n"
              << "    [Email] 发送成功 (附件作为 Base64 编码)\n";
    return true;
}

size_t EmailPlatform::maxPayloadBytes() const {
    return 25 * 1024 * 1024;  // 25MB 附件限制
}

// ============================================================
// SmsPlatform 实现
// ============================================================

SmsPlatform::SmsPlatform(const std::string& gateway)
    : gateway_(gateway) {}

std::string SmsPlatform::platformName() const {
    return "SMS(" + gateway_ + ")";
}

bool SmsPlatform::sendText(const std::string& recipient,
                            const std::string& title,
                            const std::string& body) {
    // 短信有字数限制，需要截断
    std::string smsContent = title + ": " + body;
    if (smsContent.size() > 140) {
        smsContent = smsContent.substr(0, 137) + "...";
    }
    std::cout << "    [SMS] 通过网关 " << gateway_ << " 发送短信\n"
              << "    [SMS] 接收号码: " << recipient << "\n"
              << "    [SMS] 内容 (" << smsContent.size() << " 字符): "
              << smsContent << "\n"
              << "    [SMS] 发送成功\n";
    return true;
}

bool SmsPlatform::sendBinary(const std::string& recipient,
                              const std::string& title,
                              const std::vector<uint8_t>& data,
                              const std::string& mimeType) {
    // 短信不支持大型二进制数据，发送下载链接代替
    std::cout << "    [SMS] 短信不支持直接发送 " << mimeType << " ("
              << data.size() << " 字节)\n"
              << "    [SMS] 接收号码: " << recipient << "\n"
              << "    [SMS] 转为发送下载链接: \"" << title
              << " - 点击查看: https://cdn.example.com/msg/abc123\"\n"
              << "    [SMS] 发送成功 (降级为链接)\n";
    return true;
}

size_t SmsPlatform::maxPayloadBytes() const {
    return 160;  // SMS 标准字符限制
}

// ============================================================
// PushNotificationPlatform 实现
// ============================================================

PushNotificationPlatform::PushNotificationPlatform(const std::string& appId)
    : appId_(appId) {}

std::string PushNotificationPlatform::platformName() const {
    return "Push(" + appId_ + ")";
}

bool PushNotificationPlatform::sendText(const std::string& recipient,
                                          const std::string& title,
                                          const std::string& body) {
    // 推送通知标题和正文有长度限制
    std::string pushTitle = title.substr(0, 50);
    std::string pushBody = body.size() > 200 ? body.substr(0, 197) + "..." : body;

    std::cout << "    [Push] 通过 APNs/FCM 推送 (AppId: " << appId_ << ")\n"
              << "    [Push] 设备令牌: " << recipient << "\n"
              << "    [Push] 标题: " << pushTitle << "\n"
              << "    [Push] 正文: " << pushBody << "\n"
              << "    [Push] 发送成功 (含角标+声音提醒)\n";
    return true;
}

bool PushNotificationPlatform::sendBinary(const std::string& recipient,
                                            const std::string& title,
                                            const std::vector<uint8_t>& data,
                                            const std::string& mimeType) {
    // 推送通知携带缩略图 + 实际内容在应用内查看
    std::cout << "    [Push] 通过 APNs/FCM 推送富媒体通知 (AppId: " << appId_ << ")\n"
              << "    [Push] 设备令牌: " << recipient << "\n"
              << "    [Push] 标题: " << title << "\n"
              << "    [Push] 媒体类型: " << mimeType
              << " (" << data.size() << " 字节)\n"
              << "    [Push] 发送成功 (含缩略图预览，完整内容需打开应用)\n";
    return true;
}

size_t PushNotificationPlatform::maxPayloadBytes() const {
    return 4 * 1024;  // APNs 4KB 载荷限制
}

// ============================================================
// Message 抽象类实现
// ============================================================

Message::Message(std::shared_ptr<MessagePlatform> platform)
    : platform_(std::move(platform)) {}

void Message::setPlatform(std::shared_ptr<MessagePlatform> platform) {
    platform_ = std::move(platform);
}

std::string Message::getPlatformName() const {
    return platform_->platformName();
}

// ============================================================
// TextMessage 实现
// ============================================================

TextMessage::TextMessage(std::shared_ptr<MessagePlatform> platform,
                         const std::string& title,
                         const std::string& content)
    : Message(std::move(platform)), title_(title), content_(content) {}

bool TextMessage::send(const std::string& recipient) {
    std::cout << "  [TextMessage] 准备发送文本消息...\n";
    // 文本消息直接使用平台的文本发送能力
    return platform_->sendText(recipient, title_, content_);
}

std::string TextMessage::messageType() const {
    return "TextMessage";
}

// ============================================================
// ImageMessage 实现
// ============================================================

ImageMessage::ImageMessage(std::shared_ptr<MessagePlatform> platform,
                           const std::string& caption,
                           const std::string& imagePath,
                           size_t imageSize)
    : Message(std::move(platform)),
      caption_(caption),
      imagePath_(imagePath),
      imageSize_(imageSize) {}

std::vector<uint8_t> ImageMessage::simulateImageData() const {
    // 模拟图片数据
    std::vector<uint8_t> data(std::min(imageSize_, size_t(1024)));
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<uint8_t>(i % 256);
    }
    return data;
}

bool ImageMessage::send(const std::string& recipient) {
    std::cout << "  [ImageMessage] 准备发送图片消息...\n"
              << "  [ImageMessage] 图片: " << imagePath_
              << " (" << imageSize_ << " 字节)\n";

    // 检查平台的有效载荷限制
    if (imageSize_ > platform_->maxPayloadBytes()) {
        std::cout << "  [ImageMessage] 警告: 图片大小 (" << imageSize_
                  << " 字节) 超过平台限制 ("
                  << platform_->maxPayloadBytes() << " 字节)，将进行压缩\n";
    }

    auto imageData = simulateImageData();
    return platform_->sendBinary(recipient, caption_, imageData, "image/jpeg");
}

std::string ImageMessage::messageType() const {
    return "ImageMessage";
}

// ============================================================
// VideoMessage 实现
// ============================================================

VideoMessage::VideoMessage(std::shared_ptr<MessagePlatform> platform,
                           const std::string& title,
                           const std::string& videoPath,
                           size_t videoSize,
                           int durationSeconds)
    : Message(std::move(platform)),
      title_(title),
      videoPath_(videoPath),
      videoSize_(videoSize),
      durationSeconds_(durationSeconds) {}

std::vector<uint8_t> VideoMessage::simulateVideoData() const {
    std::vector<uint8_t> data(std::min(videoSize_, size_t(2048)));
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = static_cast<uint8_t>((i * 7) % 256);
    }
    return data;
}

bool VideoMessage::send(const std::string& recipient) {
    int minutes = durationSeconds_ / 60;
    int seconds = durationSeconds_ % 60;

    std::cout << "  [VideoMessage] 准备发送视频消息...\n"
              << "  [VideoMessage] 视频: " << videoPath_
              << " (" << videoSize_ << " 字节, "
              << minutes << "分" << seconds << "秒)\n";

    if (videoSize_ > platform_->maxPayloadBytes()) {
        std::cout << "  [VideoMessage] 警告: 视频大小超过平台限制，"
                  << "将上传至云端并发送链接\n";
    }

    auto videoData = simulateVideoData();
    std::string fullTitle = title_ + " [" + std::to_string(minutes) + ":"
                            + (seconds < 10 ? "0" : "") + std::to_string(seconds) + "]";
    return platform_->sendBinary(recipient, fullTitle, videoData, "video/mp4");
}

std::string VideoMessage::messageType() const {
    return "VideoMessage";
}

// ============================================================
// 演示程序
// ============================================================

int main() {
    std::cout << "========================================\n";
    std::cout << "   桥接模式 - 跨平台消息系统演示\n";
    std::cout << "========================================\n\n";

    // 创建三个平台实例（实现维度）
    auto email = std::make_shared<EmailPlatform>("smtp.company.com");
    auto sms = std::make_shared<SmsPlatform>("twilio-gw");
    auto push = std::make_shared<PushNotificationPlatform>("com.company.app");

    // 场景1：同一条文本消息通过不同平台发送
    // 展示桥接模式的核心价值——抽象与实现独立变化
    std::cout << "--- 场景1: 同一条文本消息 x 三种平台 ---\n\n";
    {
        auto msg = std::make_unique<TextMessage>(
            email, "系统通知", "您的订单 #10086 已发货，预计明天送达。");

        std::cout << "[" << msg->messageType() << " via "
                  << msg->getPlatformName() << "]\n";
        msg->send("user@example.com");
        std::cout << "\n";

        // 运行时切换平台——无需创建新的消息对象
        msg->setPlatform(sms);
        std::cout << "[" << msg->messageType() << " via "
                  << msg->getPlatformName() << "]\n";
        msg->send("+8613800138000");
        std::cout << "\n";

        msg->setPlatform(push);
        std::cout << "[" << msg->messageType() << " via "
                  << msg->getPlatformName() << "]\n";
        msg->send("device-token-abc123");
        std::cout << "\n";
    }

    // 场景2：不同类型的消息通过邮件发送
    // 展示消息类型维度的扩展
    std::cout << "--- 场景2: 三种消息类型 x 邮件平台 ---\n\n";
    {
        auto textMsg = std::make_unique<TextMessage>(
            email, "周报", "本周完成了桥接模式的实现，代码已提交。");

        auto imageMsg = std::make_unique<ImageMessage>(
            email, "设计稿截图", "/images/design_v2.jpg", 2 * 1024 * 1024);

        auto videoMsg = std::make_unique<VideoMessage>(
            email, "产品演示", "/videos/demo.mp4", 15 * 1024 * 1024, 185);

        std::cout << "[" << textMsg->messageType() << " via "
                  << textMsg->getPlatformName() << "]\n";
        textMsg->send("team@company.com");
        std::cout << "\n";

        std::cout << "[" << imageMsg->messageType() << " via "
                  << imageMsg->getPlatformName() << "]\n";
        imageMsg->send("designer@company.com");
        std::cout << "\n";

        std::cout << "[" << videoMsg->messageType() << " via "
                  << videoMsg->getPlatformName() << "]\n";
        videoMsg->send("pm@company.com");
        std::cout << "\n";
    }

    // 场景3：展示平台限制导致的行为差异
    // 同一条视频消息在不同平台上的表现截然不同
    std::cout << "--- 场景3: 大视频消息在不同平台上的表现 ---\n\n";
    {
        auto video = std::make_unique<VideoMessage>(
            email, "会议录像", "/videos/meeting.mp4",
            50 * 1024 * 1024, 3600);  // 50MB, 1小时

        std::cout << "[via Email - 大附件]\n";
        video->send("archive@company.com");
        std::cout << "\n";

        video->setPlatform(sms);
        std::cout << "[via SMS - 降级为链接]\n";
        video->send("+8613900139000");
        std::cout << "\n";

        video->setPlatform(push);
        std::cout << "[via Push - 预览+应用内查看]\n";
        video->send("device-token-xyz789");
        std::cout << "\n";
    }

    // 场景4：批量消息分发
    // 展示桥接模式在实际业务中的应用——多消息 x 多平台的矩阵
    std::cout << "--- 场景4: 批量消息分发 ---\n\n";
    {
        // 不同平台的接收者列表
        struct Recipient {
            std::string name;
            std::shared_ptr<MessagePlatform> platform;
            std::string address;
        };

        std::vector<Recipient> recipients = {
            {"张三", email, "zhangsan@example.com"},
            {"李四", sms, "+8613700137000"},
            {"王五", push, "device-token-wangwu"},
        };

        auto announcement = std::make_unique<TextMessage>(
            email,  // 初始平台，会被切换
            "紧急通知",
            "服务器将于今晚 22:00 进行维护，预计持续2小时。");

        for (const auto& r : recipients) {
            announcement->setPlatform(r.platform);
            std::cout << "[发送给 " << r.name << " via "
                      << r.platform->platformName() << "]\n";
            announcement->send(r.address);
            std::cout << "\n";
        }
    }

    std::cout << "========================================\n";
    std::cout << "   演示结束\n";
    std::cout << "========================================\n";

    return 0;
}
```

## 构建方式

```cmake title="CMakeLists.txt"
cmake_minimum_required(VERSION 3.17)
add_executable(Pattern_Bridge Bridge.cpp Bridge.h)
target_compile_features(Pattern_Bridge PRIVATE cxx_std_17)
```

:::tip 编译运行
```bash
cd build
cmake --build . --target Pattern_Bridge
./Pattern_Bridge
```
:::
