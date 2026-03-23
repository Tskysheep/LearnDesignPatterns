#pragma once
#include <string>
#include <vector>
#include <memory>
#include <sstream>

// ============================================================
// 建造者模式 (Builder Pattern)
// 场景：SQL 查询构建器 —— 通过链式调用逐步构建复杂的 SQL 查询语句
//       将 SQL 的构建过程与最终表示分离
// ============================================================

// --- 产品：SQL 查询对象 ---
// 封装一条完整的 SQL 查询及其参数
class SqlQuery {
public:
    void setType(const std::string& type);
    void setColumns(const std::vector<std::string>& columns);
    void setTable(const std::string& table);
    void setWhereClause(const std::string& clause);
    void setJoinClause(const std::string& clause);
    void setOrderByClause(const std::string& clause);
    void setGroupByClause(const std::string& clause);
    void setHavingClause(const std::string& clause);
    void setLimit(int limit);
    void setOffset(int offset);

    // 生成最终 SQL 字符串
    std::string toSql() const;

    // 获取查询摘要信息
    std::string describe() const;

private:
    std::string type_ = "SELECT";
    std::vector<std::string> columns_;
    std::string table_;
    std::string whereClause_;
    std::string joinClause_;
    std::string orderByClause_;
    std::string groupByClause_;
    std::string havingClause_;
    int limit_ = -1;
    int offset_ = -1;
};

// --- 抽象建造者：查询构建器接口 ---
// 定义构建 SQL 查询各部分的步骤
class QueryBuilder {
public:
    virtual ~QueryBuilder() = default;

    // 链式调用方法 —— 每个方法返回 *this 的引用
    virtual QueryBuilder& select(const std::vector<std::string>& columns) = 0;
    virtual QueryBuilder& from(const std::string& table) = 0;
    virtual QueryBuilder& where(const std::string& condition) = 0;
    virtual QueryBuilder& andWhere(const std::string& condition) = 0;
    virtual QueryBuilder& orWhere(const std::string& condition) = 0;
    virtual QueryBuilder& join(const std::string& table, const std::string& on) = 0;
    virtual QueryBuilder& leftJoin(const std::string& table, const std::string& on) = 0;
    virtual QueryBuilder& orderBy(const std::string& column, const std::string& direction = "ASC") = 0;
    virtual QueryBuilder& groupBy(const std::string& column) = 0;
    virtual QueryBuilder& having(const std::string& condition) = 0;
    virtual QueryBuilder& limit(int count) = 0;
    virtual QueryBuilder& offset(int count) = 0;

    // 构建最终产品
    virtual std::unique_ptr<SqlQuery> build() = 0;

    // 重置建造者以复用
    virtual void reset() = 0;
};

// --- 具体建造者：标准 SQL 构建器 ---
// 构建符合 ANSI SQL 标准的查询语句
class StandardSqlBuilder : public QueryBuilder {
public:
    StandardSqlBuilder();

    QueryBuilder& select(const std::vector<std::string>& columns) override;
    QueryBuilder& from(const std::string& table) override;
    QueryBuilder& where(const std::string& condition) override;
    QueryBuilder& andWhere(const std::string& condition) override;
    QueryBuilder& orWhere(const std::string& condition) override;
    QueryBuilder& join(const std::string& table, const std::string& on) override;
    QueryBuilder& leftJoin(const std::string& table, const std::string& on) override;
    QueryBuilder& orderBy(const std::string& column, const std::string& direction) override;
    QueryBuilder& groupBy(const std::string& column) override;
    QueryBuilder& having(const std::string& condition) override;
    QueryBuilder& limit(int count) override;
    QueryBuilder& offset(int count) override;

    std::unique_ptr<SqlQuery> build() override;
    void reset() override;

private:
    std::unique_ptr<SqlQuery> query_;
    std::vector<std::string> columns_;
    std::string table_;
    std::vector<std::string> whereConditions_;
    std::vector<std::string> joinClauses_;
    std::vector<std::pair<std::string, std::string>> orderByClauses_;
    std::vector<std::string> groupByClauses_;
    std::string havingClause_;
    int limit_ = -1;
    int offset_ = -1;
};

// --- 具体建造者：MySQL 方言构建器 ---
// 生成 MySQL 特有语法（如 LIMIT offset, count）
class MySqlBuilder : public QueryBuilder {
public:
    MySqlBuilder();

    QueryBuilder& select(const std::vector<std::string>& columns) override;
    QueryBuilder& from(const std::string& table) override;
    QueryBuilder& where(const std::string& condition) override;
    QueryBuilder& andWhere(const std::string& condition) override;
    QueryBuilder& orWhere(const std::string& condition) override;
    QueryBuilder& join(const std::string& table, const std::string& on) override;
    QueryBuilder& leftJoin(const std::string& table, const std::string& on) override;
    QueryBuilder& orderBy(const std::string& column, const std::string& direction) override;
    QueryBuilder& groupBy(const std::string& column) override;
    QueryBuilder& having(const std::string& condition) override;
    QueryBuilder& limit(int count) override;
    QueryBuilder& offset(int count) override;

    std::unique_ptr<SqlQuery> build() override;
    void reset() override;

private:
    std::unique_ptr<SqlQuery> query_;
    std::vector<std::string> columns_;
    std::string table_;
    std::vector<std::string> whereConditions_;
    std::vector<std::string> joinClauses_;
    std::vector<std::pair<std::string, std::string>> orderByClauses_;
    std::vector<std::string> groupByClauses_;
    std::string havingClause_;
    int limit_ = -1;
    int offset_ = -1;
};

// --- Director：预定义常用查询模式 ---
// 封装常见的查询构建步骤序列，客户端无需了解构建细节
class QueryDirector {
public:
    explicit QueryDirector(QueryBuilder& builder);

    // 构建分页列表查询
    std::unique_ptr<SqlQuery> buildPaginatedList(
        const std::string& table,
        const std::vector<std::string>& columns,
        int page, int pageSize);

    // 构建带条件的统计查询
    std::unique_ptr<SqlQuery> buildAggregation(
        const std::string& table,
        const std::string& aggregateColumn,
        const std::string& groupColumn,
        const std::string& condition = "");

    // 构建关联查询
    std::unique_ptr<SqlQuery> buildJoinQuery(
        const std::string& mainTable,
        const std::string& joinTable,
        const std::string& joinCondition,
        const std::vector<std::string>& columns);

private:
    QueryBuilder& builder_;
};
