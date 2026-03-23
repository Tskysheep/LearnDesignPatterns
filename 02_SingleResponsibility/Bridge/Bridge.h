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
