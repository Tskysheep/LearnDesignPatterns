#include "TemplateMethod.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>

// =============================================================================
// DataMiner 基类实现
// =============================================================================

// 模板方法：算法骨架固定不变
// 注意：子类不能覆写此方法（语义上应为 final，但此处通过设计约定保证）
void DataMiner::mine(const std::string& source) {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  数据挖掘流水线启动" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // Step 1: 打开数据源
    openSource(source);

    // Step 2: 提取原始数据
    std::string rawData = extractRawData();

    // Step 3: 解析为结构化数据
    std::vector<DataRecord> records = parseData(rawData);
    std::cout << "[流水线] 解析完成，共 " << records.size() << " 条记录" << std::endl;

    // Step 3.5: 钩子方法控制是否执行去重（模板方法中的可选步骤）
    if (shouldDeduplicate()) {
        records = deduplicateData(records);
        std::cout << "[流水线] 去重完成，剩余 " << records.size() << " 条记录" << std::endl;
    }

    // Step 4: 分析数据
    AnalysisResult result = analyzeData(records);

    // Step 5: 生成报告
    generateReport(result);

    // Step 6: 关闭数据源
    closeSource();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  数据挖掘流水线完成" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

// 默认的数据分析实现：计算基本统计量
AnalysisResult DataMiner::analyzeData(const std::vector<DataRecord>& records) {
    std::cout << "[分析] 执行默认统计分析..." << std::endl;

    AnalysisResult result;
    result.totalRecords = static_cast<int>(records.size());

    std::vector<double> values;
    for (const auto& record : records) {
        std::string val = record.get("value");
        if (!val.empty()) {
            try {
                values.push_back(std::stod(val));
            } catch (...) {
                // 忽略无法转换的值
            }
        }
    }

    if (!values.empty()) {
        result.averageValue = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        result.maxValue = *std::max_element(values.begin(), values.end());
        result.minValue = *std::min_element(values.begin(), values.end());
    }

    result.summary = "基本统计分析完成，数据源: " + sourceName_;
    return result;
}

// 默认的报告生成
void DataMiner::generateReport(const AnalysisResult& result) {
    std::cout << "\n--- 分析报告 ---" << std::endl;
    std::cout << "数据源: " << sourceName_ << std::endl;
    std::cout << "总记录数: " << result.totalRecords << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "平均值: " << result.averageValue << std::endl;
    std::cout << "最大值: " << result.maxValue << std::endl;
    std::cout << "最小值: " << result.minValue << std::endl;
    std::cout << "摘要: " << result.summary << std::endl;
    std::cout << "--- 报告结束 ---\n" << std::endl;
}

// 默认的去重逻辑：按 "id" 字段去重
std::vector<DataRecord> DataMiner::deduplicateData(const std::vector<DataRecord>& records) {
    std::cout << "[去重] 按 ID 字段去重..." << std::endl;
    std::map<std::string, DataRecord> unique;
    for (const auto& record : records) {
        std::string id = record.get("id");
        if (!id.empty()) {
            unique[id] = record;
        }
    }
    std::vector<DataRecord> result;
    result.reserve(unique.size());
    for (auto& [key, val] : unique) {
        result.push_back(std::move(val));
    }
    return result;
}

// =============================================================================
// CsvDataMiner 实现
// =============================================================================

CsvDataMiner::CsvDataMiner(char delimiter) : delimiter_(delimiter) {}

void CsvDataMiner::openSource(const std::string& source) {
    filePath_ = source;
    sourceName_ = "CSV 文件: " + source;
    std::cout << "[CSV] 打开文件: " << source << std::endl;
    // 模拟打开文件
    std::cout << "[CSV] 文件打开成功，分隔符: '" << delimiter_ << "'" << std::endl;
}

std::string CsvDataMiner::extractRawData() {
    std::cout << "[CSV] 读取文件内容..." << std::endl;
    // 模拟 CSV 文件内容（包含重复记录以演示去重）
    return "id,name,value\n"
           "1,产品A,150.5\n"
           "2,产品B,230.0\n"
           "3,产品C,89.9\n"
           "1,产品A,150.5\n"  // 重复记录
           "4,产品D,310.2\n"
           "5,产品E,175.8\n";
}

std::vector<DataRecord> CsvDataMiner::parseData(const std::string& rawData) {
    std::cout << "[CSV] 解析 CSV 数据..." << std::endl;
    std::vector<DataRecord> records;
    std::istringstream stream(rawData);
    std::string line;

    // 读取表头
    std::vector<std::string> headers;
    if (std::getline(stream, line)) {
        std::istringstream headerStream(line);
        std::string header;
        while (std::getline(headerStream, header, delimiter_)) {
            headers.push_back(header);
        }
    }

    // 解析数据行
    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        DataRecord record;
        std::istringstream lineStream(line);
        std::string field;
        size_t idx = 0;
        while (std::getline(lineStream, field, delimiter_) && idx < headers.size()) {
            record.fields[headers[idx]] = field;
            ++idx;
        }
        records.push_back(std::move(record));
    }

    return records;
}

void CsvDataMiner::closeSource() {
    std::cout << "[CSV] 关闭文件: " << filePath_ << std::endl;
}

// =============================================================================
// JsonDataMiner 实现
// =============================================================================

void JsonDataMiner::openSource(const std::string& source) {
    jsonEndpoint_ = source;
    sourceName_ = "JSON 数据源: " + source;
    std::cout << "[JSON] 连接到 API 端点: " << source << std::endl;
    std::cout << "[JSON] 连接建立成功" << std::endl;
}

std::string JsonDataMiner::extractRawData() {
    std::cout << "[JSON] 从 API 拉取 JSON 数据..." << std::endl;
    // 模拟 JSON 响应
    return R"([
        {"id":"101","name":"订单-北京","value":"5200.00"},
        {"id":"102","name":"订单-上海","value":"8100.50"},
        {"id":"103","name":"订单-广州","value":"3750.25"},
        {"id":"104","name":"订单-深圳","value":"6400.00"}
    ])";
}

std::vector<DataRecord> JsonDataMiner::parseData(const std::string& rawData) {
    std::cout << "[JSON] 解析 JSON 数据..." << std::endl;
    // 简化的 JSON 解析（生产环境应使用 nlohmann/json 等库）
    std::vector<DataRecord> records;
    std::istringstream stream(rawData);
    std::string line;

    while (std::getline(stream, line)) {
        DataRecord record;
        // 简易提取 key-value 对
        auto extractField = [&](const std::string& key) -> std::string {
            std::string search = "\"" + key + "\":\"";
            auto pos = line.find(search);
            if (pos == std::string::npos) return "";
            pos += search.size();
            auto end = line.find("\"", pos);
            return (end != std::string::npos) ? line.substr(pos, end - pos) : "";
        };

        std::string id = extractField("id");
        if (!id.empty()) {
            record.fields["id"] = id;
            record.fields["name"] = extractField("name");
            record.fields["value"] = extractField("value");
            records.push_back(std::move(record));
        }
    }

    return records;
}

void JsonDataMiner::closeSource() {
    std::cout << "[JSON] 断开 API 连接: " << jsonEndpoint_ << std::endl;
}

// 覆写报告生成：JSON 风格输出
void JsonDataMiner::generateReport(const AnalysisResult& result) {
    std::cout << "\n--- JSON 分析报告 ---" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "{" << std::endl;
    std::cout << "  \"source\": \"" << sourceName_ << "\"," << std::endl;
    std::cout << "  \"totalRecords\": " << result.totalRecords << "," << std::endl;
    std::cout << "  \"averageValue\": " << result.averageValue << "," << std::endl;
    std::cout << "  \"maxValue\": " << result.maxValue << "," << std::endl;
    std::cout << "  \"minValue\": " << result.minValue << "," << std::endl;
    std::cout << "  \"summary\": \"" << result.summary << "\"" << std::endl;
    std::cout << "}" << std::endl;
    std::cout << "--- 报告结束 ---\n" << std::endl;
}

// =============================================================================
// DatabaseMiner 实现
// =============================================================================

DatabaseMiner::DatabaseMiner(const std::string& dbType) : dbType_(dbType) {}

void DatabaseMiner::openSource(const std::string& source) {
    connectionString_ = source;
    sourceName_ = dbType_ + " 数据库: " + source;
    std::cout << "[DB] 连接 " << dbType_ << " 数据库: " << source << std::endl;
    std::cout << "[DB] 数据库连接池初始化完成 (连接数: 5)" << std::endl;
}

std::string DatabaseMiner::extractRawData() {
    std::cout << "[DB] 执行 SQL 查询: SELECT * FROM sales_data..." << std::endl;
    // 模拟数据库查询结果（包含重复行以演示去重）
    return "1|华东区|4500.00\n"
           "2|华南区|3800.50\n"
           "3|华北区|5200.75\n"
           "2|华南区|3800.50\n"  // 重复记录
           "4|西南区|2900.00\n"
           "5|西北区|2100.30\n"
           "3|华北区|5200.75\n"; // 重复记录
}

std::vector<DataRecord> DatabaseMiner::parseData(const std::string& rawData) {
    std::cout << "[DB] 解析查询结果集..." << std::endl;
    std::vector<DataRecord> records;
    std::istringstream stream(rawData);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        DataRecord record;
        std::istringstream lineStream(line);
        std::string field;

        std::vector<std::string> fields;
        while (std::getline(lineStream, field, '|')) {
            fields.push_back(field);
        }

        if (fields.size() >= 3) {
            record.fields["id"] = fields[0];
            record.fields["name"] = fields[1];
            record.fields["value"] = fields[2];
            records.push_back(std::move(record));
        }
    }

    return records;
}

void DatabaseMiner::closeSource() {
    std::cout << "[DB] 关闭数据库连接: " << connectionString_ << std::endl;
    std::cout << "[DB] 连接池已释放" << std::endl;
}

// 覆写分析方法：加入数据库特有的统计信息
AnalysisResult DatabaseMiner::analyzeData(const std::vector<DataRecord>& records) {
    std::cout << "[DB 分析] 执行数据库增强型分析..." << std::endl;

    // 先调用基类的默认分析
    AnalysisResult result = DataMiner::analyzeData(records);

    // 增加数据库特有的分析信息
    double range = result.maxValue - result.minValue;
    result.summary = "数据库增强分析完成，数据源: " + sourceName_
                   + "，数据范围: " + std::to_string(range);

    std::cout << "[DB 分析] 数据分布范围: " << std::fixed << std::setprecision(2)
              << range << std::endl;

    return result;
}

// =============================================================================
// 主函数：演示模板方法模式
// =============================================================================
int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "    模板方法模式 (Template Method Pattern) 演示" << std::endl;
    std::cout << "    场景：数据挖掘流水线" << std::endl;
    std::cout << "==================================================" << std::endl;

    // 演示1：CSV 数据挖掘
    // 所有 DataMiner 子类共享相同的 mine() 流程骨架，
    // 但各自实现了不同的 open/extract/parse/close 步骤
    std::cout << "\n【演示 1】CSV 数据挖掘" << std::endl;
    auto csvMiner = std::make_unique<CsvDataMiner>();
    csvMiner->mine("sales_2024_Q1.csv");

    // 演示2：JSON API 数据挖掘
    std::cout << "\n【演示 2】JSON API 数据挖掘" << std::endl;
    auto jsonMiner = std::make_unique<JsonDataMiner>();
    jsonMiner->mine("https://api.example.com/orders");

    // 演示3：数据库数据挖掘
    std::cout << "\n【演示 3】PostgreSQL 数据库数据挖掘" << std::endl;
    auto dbMiner = std::make_unique<DatabaseMiner>("PostgreSQL");
    dbMiner->mine("host=db.example.com dbname=analytics");

    // 关键总结
    std::cout << "==================================================" << std::endl;
    std::cout << "模板方法模式要点总结：" << std::endl;
    std::cout << "1. mine() 方法定义了固定的算法骨架" << std::endl;
    std::cout << "2. 子类覆写具体步骤，但不改变流程顺序" << std::endl;
    std::cout << "3. 钩子方法 shouldDeduplicate() 允许子类控制可选步骤" << std::endl;
    std::cout << "4. 基类提供默认实现，子类按需覆写" << std::endl;
    std::cout << "==================================================" << std::endl;

    return 0;
}
