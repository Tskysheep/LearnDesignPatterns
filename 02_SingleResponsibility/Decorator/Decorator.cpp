#include "Decorator.h"
#include <iostream>
#include <algorithm>
#include <sstream>

// ============================================================
// FileStream 实现
// ============================================================

FileStream::FileStream(const std::string& filename)
    : filename_(filename) {}

std::string FileStream::read() {
    // 模拟从文件读取数据
    std::cout << "  [FileStream] 从文件 \"" << filename_ << "\" 读取数据\n";
    return buffer_;
}

void FileStream::write(const std::string& data) {
    // 模拟写入文件
    std::cout << "  [FileStream] 写入数据到文件 \"" << filename_ << "\"\n";
    buffer_ = data;
}

std::string FileStream::getDescription() const {
    return "FileStream(" + filename_ + ")";
}

// ============================================================
// MemoryStream 实现
// ============================================================

MemoryStream::MemoryStream() = default;

std::string MemoryStream::read() {
    std::cout << "  [MemoryStream] 从内存读取数据\n";
    return buffer_;
}

void MemoryStream::write(const std::string& data) {
    std::cout << "  [MemoryStream] 写入数据到内存\n";
    buffer_ = data;
}

std::string MemoryStream::getDescription() const {
    return "MemoryStream";
}

// ============================================================
// StreamDecorator 基类实现
// 默认行为：直接转发给被装饰对象
// ============================================================

StreamDecorator::StreamDecorator(std::unique_ptr<DataStream> stream)
    : wrappedStream_(std::move(stream)) {}

std::string StreamDecorator::read() {
    return wrappedStream_->read();
}

void StreamDecorator::write(const std::string& data) {
    wrappedStream_->write(data);
}

std::string StreamDecorator::getDescription() const {
    return wrappedStream_->getDescription();
}

// ============================================================
// EncryptionStream 实现
// 使用简单的凯撒密码来演示加密/解密过程
// ============================================================

EncryptionStream::EncryptionStream(std::unique_ptr<DataStream> stream, int key)
    : StreamDecorator(std::move(stream)), key_(key) {}

std::string EncryptionStream::encrypt(const std::string& data) const {
    std::string result = data;
    for (auto& ch : result) {
        // 只对可打印ASCII字符（33~126）做偏移，保持空格不变
        if (ch >= 33 && ch <= 126) {
            ch = static_cast<char>((ch - 33 + key_) % 94 + 33);
        }
    }
    return result;
}

std::string EncryptionStream::decrypt(const std::string& data) const {
    std::string result = data;
    for (auto& ch : result) {
        if (ch >= 33 && ch <= 126) {
            ch = static_cast<char>((ch - 33 - key_ + 94) % 94 + 33);
        }
    }
    return result;
}

std::string EncryptionStream::read() {
    // 读取时先从内层流读取，然后解密
    std::string data = wrappedStream_->read();
    std::cout << "  [Encryption] 解密数据: \"" << data << "\"\n";
    std::string decrypted = decrypt(data);
    std::cout << "  [Encryption] 解密结果: \"" << decrypted << "\"\n";
    return decrypted;
}

void EncryptionStream::write(const std::string& data) {
    // 写入时先加密，然后传给内层流
    std::cout << "  [Encryption] 加密数据: \"" << data << "\"\n";
    std::string encrypted = encrypt(data);
    std::cout << "  [Encryption] 加密结果: \"" << encrypted << "\"\n";
    wrappedStream_->write(encrypted);
}

std::string EncryptionStream::getDescription() const {
    return "Encryption -> " + wrappedStream_->getDescription();
}

// ============================================================
// CompressionStream 实现
// 使用简单的游程编码（Run-Length Encoding）
// ============================================================

CompressionStream::CompressionStream(std::unique_ptr<DataStream> stream)
    : StreamDecorator(std::move(stream)) {}

std::string CompressionStream::compress(const std::string& data) const {
    if (data.empty()) return data;

    std::ostringstream oss;
    char current = data[0];
    int count = 1;

    for (std::size_t i = 1; i < data.size(); ++i) {
        if (data[i] == current) {
            ++count;
        } else {
            oss << current;
            if (count > 1) oss << count;
            current = data[i];
            count = 1;
        }
    }
    oss << current;
    if (count > 1) oss << count;
    return oss.str();
}

std::string CompressionStream::decompress(const std::string& data) const {
    if (data.empty()) return data;

    std::ostringstream oss;
    for (std::size_t i = 0; i < data.size(); ++i) {
        if (std::isdigit(static_cast<unsigned char>(data[i]))) {
            // 数字表示前一个字符重复的次数
            int count = data[i] - '0';
            if (!oss.str().empty()) {
                char lastChar = oss.str().back();
                // 已经输出了一次，再输出 count-1 次
                for (int j = 1; j < count; ++j) {
                    oss << lastChar;
                }
            }
        } else {
            oss << data[i];
        }
    }
    return oss.str();
}

std::string CompressionStream::read() {
    std::string data = wrappedStream_->read();
    std::cout << "  [Compression] 解压数据: \"" << data << "\"\n";
    std::string decompressed = decompress(data);
    std::cout << "  [Compression] 解压结果: \"" << decompressed << "\"\n";
    return decompressed;
}

void CompressionStream::write(const std::string& data) {
    std::cout << "  [Compression] 压缩数据: \"" << data << "\"\n";
    std::string compressed = compress(data);
    std::cout << "  [Compression] 压缩结果: \"" << compressed << "\"\n";
    wrappedStream_->write(compressed);
}

std::string CompressionStream::getDescription() const {
    return "Compression -> " + wrappedStream_->getDescription();
}

// ============================================================
// BufferedStream 实现
// ============================================================

BufferedStream::BufferedStream(std::unique_ptr<DataStream> stream,
                               std::size_t bufferSize)
    : StreamDecorator(std::move(stream)), bufferSize_(bufferSize) {}

BufferedStream::~BufferedStream() {
    // 析构时自动刷新未写入的数据，避免数据丢失
    if (!pendingBuffer_.empty()) {
        flush();
    }
}

std::string BufferedStream::read() {
    std::cout << "  [Buffered] 通过缓冲区读取数据\n";
    return wrappedStream_->read();
}

void BufferedStream::write(const std::string& data) {
    pendingBuffer_ += data;
    std::cout << "  [Buffered] 缓冲数据 (当前缓冲区: "
              << pendingBuffer_.size() << "/" << bufferSize_ << " 字节)\n";

    // 当缓冲区满时自动刷新
    if (pendingBuffer_.size() >= bufferSize_) {
        flush();
    }
}

void BufferedStream::flush() {
    if (!pendingBuffer_.empty()) {
        std::cout << "  [Buffered] 刷新缓冲区，写入 "
                  << pendingBuffer_.size() << " 字节\n";
        wrappedStream_->write(pendingBuffer_);
        pendingBuffer_.clear();
    }
}

std::string BufferedStream::getDescription() const {
    return "Buffered(" + std::to_string(bufferSize_) + ") -> "
           + wrappedStream_->getDescription();
}

// ============================================================
// 演示程序
// ============================================================

int main() {
    std::cout << "========================================\n";
    std::cout << "   装饰器模式 - 数据流处理演示\n";
    std::cout << "========================================\n\n";

    // 场景1：基础文件流（无装饰）
    std::cout << "--- 场景1: 基础文件流 ---\n";
    {
        auto stream = std::make_unique<FileStream>("data.txt");
        std::cout << "流描述: " << stream->getDescription() << "\n";
        stream->write("Hello World");
        std::string result = stream->read();
        std::cout << "读取结果: \"" << result << "\"\n\n";
    }

    // 场景2：文件流 + 加密装饰
    std::cout << "--- 场景2: 加密文件流 ---\n";
    {
        // 装饰器层层包裹：FileStream被EncryptionStream装饰
        auto stream = std::make_unique<EncryptionStream>(
            std::make_unique<FileStream>("secret.dat"), 5);
        std::cout << "流描述: " << stream->getDescription() << "\n";
        stream->write("Sensitive Data");
        std::string result = stream->read();
        std::cout << "读取结果: \"" << result << "\"\n\n";
    }

    // 场景3：多层装饰叠加——压缩 + 加密 + 文件流
    // 写入顺序：原始数据 -> 压缩 -> 加密 -> 文件
    // 读取顺序：文件 -> 解密 -> 解压 -> 原始数据
    std::cout << "--- 场景3: 压缩 + 加密 叠加装饰 ---\n";
    {
        auto stream = std::make_unique<CompressionStream>(
            std::make_unique<EncryptionStream>(
                std::make_unique<FileStream>("archive.enc"), 3));
        std::cout << "流描述: " << stream->getDescription() << "\n";
        stream->write("aaabbbcccddd");
        std::string result = stream->read();
        std::cout << "读取结果: \"" << result << "\"\n\n";
    }

    // 场景4：缓冲 + 加密 + 内存流
    std::cout << "--- 场景4: 缓冲 + 加密 + 内存流 ---\n";
    {
        // 设置较小的缓冲区大小以便演示
        auto rawStream = std::make_unique<BufferedStream>(
            std::make_unique<EncryptionStream>(
                std::make_unique<MemoryStream>(), 2),
            30);  // 30字节缓冲区
        std::cout << "流描述: " << rawStream->getDescription() << "\n";

        // 多次写入小数据，观察缓冲行为
        rawStream->write("First chunk. ");
        rawStream->write("Second chunk. ");
        rawStream->write("Third chunk.");  // 应触发缓冲区刷新

        // 手动刷新剩余数据
        rawStream->flush();
        std::string result = rawStream->read();
        std::cout << "读取结果: \"" << result << "\"\n\n";
    }

    // 场景5：展示装饰器的灵活性——运行时动态组合
    std::cout << "--- 场景5: 运行时动态组合装饰器 ---\n";
    {
        // 根据"配置"动态决定是否启用各种功能
        bool enableEncryption = true;
        bool enableCompression = true;
        bool enableBuffering = true;

        std::unique_ptr<DataStream> stream =
            std::make_unique<MemoryStream>();

        if (enableBuffering) {
            stream = std::make_unique<BufferedStream>(
                std::move(stream), 20);  // 小缓冲区，写入时自动刷新
        }
        if (enableEncryption) {
            stream = std::make_unique<EncryptionStream>(
                std::move(stream), 3);
        }
        if (enableCompression) {
            stream = std::make_unique<CompressionStream>(
                std::move(stream));
        }

        std::cout << "流描述: " << stream->getDescription() << "\n";
        stream->write("Dynamic pipeline test data!");
        std::string result = stream->read();
        std::cout << "读取结果: \"" << result << "\"\n";
    }

    std::cout << "\n========================================\n";
    std::cout << "   演示结束\n";
    std::cout << "========================================\n";

    return 0;
}
