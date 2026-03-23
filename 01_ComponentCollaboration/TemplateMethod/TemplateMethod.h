#ifndef TEMPLATE_METHOD_H
#define TEMPLATE_METHOD_H

#include <string>
#include <vector>
#include <memory>
#include <map>

// =============================================================================
// 模板方法模式 (Template Method Pattern) - 数据挖掘场景
// =============================================================================
// 核心思想：定义一个算法的骨架（数据挖掘流水线），将某些步骤的具体实现延迟到
// 子类中。模板方法使得子类可以在不改变算法结构的情况下，重新定义算法的某些步骤。
// =============================================================================

// 数据记录：从各种数据源中提取出的统一数据结构
struct DataRecord {
    std::map<std::string, std::string> fields;

    std::string get(const std::string& key) const {
        auto it = fields.find(key);
        return (it != fields.end()) ? it->second : "";
    }
};

// 分析结果
struct AnalysisResult {
    int totalRecords = 0;
    double averageValue = 0.0;
    double maxValue = 0.0;
    double minValue = 0.0;
    std::string summary;
};

// =============================================================================
// 抽象基类：DataMiner
// 定义了数据挖掘的模板方法 mine()，其中固定了算法骨架：
//   open → extractRawData → parseData → analyzeData → generateReport → close
// 子类只需要覆写其中的可变步骤（虚函数），无需改变整体流程。
// =============================================================================
class DataMiner {
public:
    virtual ~DataMiner() = default;

    // 模板方法：定义不可变的算法骨架
    // 声明为 final，防止子类篡改流程顺序（这是模式的关键约束）
    void mine(const std::string& source);

protected:
    // ----- 以下为可变步骤，子类按需覆写 -----

    // 打开数据源（纯虚函数，必须实现）
    virtual void openSource(const std::string& source) = 0;

    // 提取原始数据（纯虚函数，必须实现）
    virtual std::string extractRawData() = 0;

    // 解析原始数据为结构化记录（纯虚函数，必须实现）
    virtual std::vector<DataRecord> parseData(const std::string& rawData) = 0;

    // 关闭数据源（纯虚函数，必须实现）
    virtual void closeSource() = 0;

    // ----- 以下为钩子方法，提供默认实现，子类可选覆写 -----

    // 分析数据（提供默认的统计分析实现）
    virtual AnalysisResult analyzeData(const std::vector<DataRecord>& records);

    // 生成报告（提供默认的文本报告实现）
    virtual void generateReport(const AnalysisResult& result);

    // 钩子方法：是否需要去重（子类可以覆写以控制流程分支）
    virtual bool shouldDeduplicate() const { return false; }

    // 数据去重
    virtual std::vector<DataRecord> deduplicateData(const std::vector<DataRecord>& records);

    std::string sourceName_;
};

// =============================================================================
// 具体子类：CsvDataMiner - 从 CSV 文件挖掘数据
// =============================================================================
class CsvDataMiner : public DataMiner {
public:
    explicit CsvDataMiner(char delimiter = ',');

protected:
    void openSource(const std::string& source) override;
    std::string extractRawData() override;
    std::vector<DataRecord> parseData(const std::string& rawData) override;
    void closeSource() override;

    // CSV 需要去重（覆写钩子方法）
    bool shouldDeduplicate() const override { return true; }

private:
    char delimiter_;
    std::string filePath_;
};

// =============================================================================
// 具体子类：JsonDataMiner - 从 JSON 数据源挖掘数据
// =============================================================================
class JsonDataMiner : public DataMiner {
protected:
    void openSource(const std::string& source) override;
    std::string extractRawData() override;
    std::vector<DataRecord> parseData(const std::string& rawData) override;
    void closeSource() override;

    // 覆写报告生成，输出 JSON 风格的报告
    void generateReport(const AnalysisResult& result) override;

private:
    std::string jsonEndpoint_;
};

// =============================================================================
// 具体子类：DatabaseMiner - 从数据库挖掘数据
// =============================================================================
class DatabaseMiner : public DataMiner {
public:
    explicit DatabaseMiner(const std::string& dbType = "PostgreSQL");

protected:
    void openSource(const std::string& source) override;
    std::string extractRawData() override;
    std::vector<DataRecord> parseData(const std::string& rawData) override;
    void closeSource() override;

    // 数据库数据也需要去重
    bool shouldDeduplicate() const override { return true; }

    // 覆写分析方法，加入数据库特有的索引统计
    AnalysisResult analyzeData(const std::vector<DataRecord>& records) override;

private:
    std::string dbType_;
    std::string connectionString_;
};

#endif // TEMPLATE_METHOD_H
