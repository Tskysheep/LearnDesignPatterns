#ifndef DECORATOR_H
#define DECORATOR_H

#include <memory>
#include <string>
#include <vector>

// ============================================================
// 装饰器模式 - 数据流处理
// 核心思想：通过组合而非继承，动态地给对象添加职责
// 每个装饰器只负责一种增强功能（单一职责）
// ============================================================

// 抽象组件：数据流接口
// 定义读写操作的统一接口，装饰器和具体流都实现此接口
class DataStream {
public:
    virtual ~DataStream() = default;
    virtual std::string read() = 0;
    virtual void write(const std::string& data) = 0;
    virtual std::string getDescription() const = 0;
};

// 具体组件：文件数据流
// 最基础的数据���实现，提供真实的读写能力
class FileStream : public DataStream {
public:
    explicit FileStream(const std::string& filename);
    std::string read() override;
    void write(const std::string& data) override;
    std::string getDescription() const override;

private:
    std::string filename_;
    std::string buffer_;  // 模拟文件内容
};

// 具体组件：内存数据流
class MemoryStream : public DataStream {
public:
    MemoryStream();
    std::string read() override;
    void write(const std::string& data) override;
    std::string getDescription() const override;

private:
    std::string buffer_;
};

// 装饰器基类
// 关键设计：持有一个DataStream的智能指针，转发所有调用给被装饰对象
// 这是装饰器模式的核心——装饰器本身也是DataStream，可以被继续装饰
class StreamDecorator : public DataStream {
public:
    explicit StreamDecorator(std::unique_ptr<DataStream> stream);
    std::string read() override;
    void write(const std::string& data) override;
    std::string getDescription() const override;

protected:
    std::unique_ptr<DataStream> wrappedStream_;  // 被装饰的流对象
};

// 具体装饰器：加密流
// 只负责加密/解密这一件事（单一职责）
class EncryptionStream : public StreamDecorator {
public:
    explicit EncryptionStream(std::unique_ptr<DataStream> stream, int key = 3);
    std::string read() override;
    void write(const std::string& data) override;
    std::string getDescription() const override;

private:
    int key_;  // 简单的凯撒密码偏移量
    std::string encrypt(const std::string& data) const;
    std::string decrypt(const std::string& data) const;
};

// 具体装饰器：压缩流
// 只负责压缩/解压这一件事
class CompressionStream : public StreamDecorator {
public:
    explicit CompressionStream(std::unique_ptr<DataStream> stream);
    std::string read() override;
    void write(const std::string& data) override;
    std::string getDescription() const override;

private:
    // 简单的游程编码（RLE）压缩
    std::string compress(const std::string& data) const;
    std::string decompress(const std::string& data) const;
};

// 具体装饰器：缓冲流
// 只负责缓冲管理这一件事，累积到阈值后才真正写入
class BufferedStream : public StreamDecorator {
public:
    explicit BufferedStream(std::unique_ptr<DataStream> stream,
                            std::size_t bufferSize = 64);
    ~BufferedStream() override;
    std::string read() override;
    void write(const std::string& data) override;
    void flush();
    std::string getDescription() const override;

private:
    std::size_t bufferSize_;
    std::string pendingBuffer_;  // 待写入的缓冲数据
};

#endif // DECORATOR_H
