---
sidebar_position: 2
title: 源代码
description: Builder 模式的完整实现代码
---

# 源代码

## 头文件（Builder.h）

```cpp title="Builder.h" showLineNumbers
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
```

## 实现文件（Builder.cpp）

```cpp title="Builder.cpp" showLineNumbers
#include "Builder.h"
#include <iostream>
#include <algorithm>

// ============================================================
// 建造者模式 —— SQL 查询构建器实现
// ============================================================

// ===================== SqlQuery 产品实现 =====================
void SqlQuery::setType(const std::string& type) { type_ = type; }
void SqlQuery::setColumns(const std::vector<std::string>& columns) { columns_ = columns; }
void SqlQuery::setTable(const std::string& table) { table_ = table; }
void SqlQuery::setWhereClause(const std::string& clause) { whereClause_ = clause; }
void SqlQuery::setJoinClause(const std::string& clause) { joinClause_ = clause; }
void SqlQuery::setOrderByClause(const std::string& clause) { orderByClause_ = clause; }
void SqlQuery::setGroupByClause(const std::string& clause) { groupByClause_ = clause; }
void SqlQuery::setHavingClause(const std::string& clause) { havingClause_ = clause; }
void SqlQuery::setLimit(int limit) { limit_ = limit; }
void SqlQuery::setOffset(int offset) { offset_ = offset; }

std::string SqlQuery::toSql() const {
    std::ostringstream sql;
    sql << type_ << " ";

    // 列
    if (columns_.empty()) {
        sql << "*";
    } else {
        for (size_t i = 0; i < columns_.size(); ++i) {
            sql << columns_[i];
            if (i + 1 < columns_.size()) sql << ", ";
        }
    }

    // FROM
    if (!table_.empty()) {
        sql << "\nFROM " << table_;
    }

    // JOIN
    if (!joinClause_.empty()) {
        sql << "\n" << joinClause_;
    }

    // WHERE
    if (!whereClause_.empty()) {
        sql << "\nWHERE " << whereClause_;
    }

    // GROUP BY
    if (!groupByClause_.empty()) {
        sql << "\nGROUP BY " << groupByClause_;
    }

    // HAVING
    if (!havingClause_.empty()) {
        sql << "\nHAVING " << havingClause_;
    }

    // ORDER BY
    if (!orderByClause_.empty()) {
        sql << "\nORDER BY " << orderByClause_;
    }

    // LIMIT / OFFSET
    if (limit_ >= 0) {
        sql << "\nLIMIT " << limit_;
    }
    if (offset_ >= 0) {
        sql << " OFFSET " << offset_;
    }

    sql << ";";
    return sql.str();
}

std::string SqlQuery::describe() const {
    std::ostringstream desc;
    desc << "Query type: " << type_
         << ", Table: " << table_
         << ", Columns: " << columns_.size();
    if (limit_ >= 0) desc << ", Limit: " << limit_;
    return desc.str();
}

// ===================== StandardSqlBuilder 实现 =====================
StandardSqlBuilder::StandardSqlBuilder() {
    reset();
}

void StandardSqlBuilder::reset() {
    query_ = std::make_unique<SqlQuery>();
    columns_.clear();
    table_.clear();
    whereConditions_.clear();
    joinClauses_.clear();
    orderByClauses_.clear();
    groupByClauses_.clear();
    havingClause_.clear();
    limit_ = -1;
    offset_ = -1;
}

QueryBuilder& StandardSqlBuilder::select(const std::vector<std::string>& columns) {
    columns_ = columns;
    return *this;
}

QueryBuilder& StandardSqlBuilder::from(const std::string& table) {
    table_ = table;
    return *this;
}

QueryBuilder& StandardSqlBuilder::where(const std::string& condition) {
    whereConditions_.clear();
    whereConditions_.push_back(condition);
    return *this;
}

QueryBuilder& StandardSqlBuilder::andWhere(const std::string& condition) {
    whereConditions_.push_back("AND " + condition);
    return *this;
}

QueryBuilder& StandardSqlBuilder::orWhere(const std::string& condition) {
    whereConditions_.push_back("OR " + condition);
    return *this;
}

QueryBuilder& StandardSqlBuilder::join(const std::string& table, const std::string& on) {
    joinClauses_.push_back("JOIN " + table + " ON " + on);
    return *this;
}

QueryBuilder& StandardSqlBuilder::leftJoin(const std::string& table, const std::string& on) {
    joinClauses_.push_back("LEFT JOIN " + table + " ON " + on);
    return *this;
}

QueryBuilder& StandardSqlBuilder::orderBy(const std::string& column, const std::string& direction) {
    orderByClauses_.emplace_back(column, direction);
    return *this;
}

QueryBuilder& StandardSqlBuilder::groupBy(const std::string& column) {
    groupByClauses_.push_back(column);
    return *this;
}

QueryBuilder& StandardSqlBuilder::having(const std::string& condition) {
    havingClause_ = condition;
    return *this;
}

QueryBuilder& StandardSqlBuilder::limit(int count) {
    limit_ = count;
    return *this;
}

QueryBuilder& StandardSqlBuilder::offset(int count) {
    offset_ = count;
    return *this;
}

// 组装最终产品
std::unique_ptr<SqlQuery> StandardSqlBuilder::build() {
    query_->setColumns(columns_);
    query_->setTable(table_);

    // 组合 WHERE 子句
    if (!whereConditions_.empty()) {
        std::ostringstream where;
        for (size_t i = 0; i < whereConditions_.size(); ++i) {
            where << whereConditions_[i];
            if (i + 1 < whereConditions_.size()) where << " ";
        }
        query_->setWhereClause(where.str());
    }

    // 组合 JOIN 子句
    if (!joinClauses_.empty()) {
        std::ostringstream joins;
        for (size_t i = 0; i < joinClauses_.size(); ++i) {
            joins << joinClauses_[i];
            if (i + 1 < joinClauses_.size()) joins << "\n";
        }
        query_->setJoinClause(joins.str());
    }

    // 组合 ORDER BY 子句
    if (!orderByClauses_.empty()) {
        std::ostringstream order;
        for (size_t i = 0; i < orderByClauses_.size(); ++i) {
            order << orderByClauses_[i].first << " " << orderByClauses_[i].second;
            if (i + 1 < orderByClauses_.size()) order << ", ";
        }
        query_->setOrderByClause(order.str());
    }

    // 组合 GROUP BY 子句
    if (!groupByClauses_.empty()) {
        std::ostringstream group;
        for (size_t i = 0; i < groupByClauses_.size(); ++i) {
            group << groupByClauses_[i];
            if (i + 1 < groupByClauses_.size()) group << ", ";
        }
        query_->setGroupByClause(group.str());
    }

    if (!havingClause_.empty()) {
        query_->setHavingClause(havingClause_);
    }

    query_->setLimit(limit_);
    query_->setOffset(offset_);

    auto result = std::move(query_);
    reset(); // 重置以便复用建造者
    return result;
}

// ===================== MySqlBuilder 实现 =====================
// MySQL 方言：使用反引号引用标识符
MySqlBuilder::MySqlBuilder() { reset(); }

void MySqlBuilder::reset() {
    query_ = std::make_unique<SqlQuery>();
    columns_.clear();
    table_.clear();
    whereConditions_.clear();
    joinClauses_.clear();
    orderByClauses_.clear();
    groupByClauses_.clear();
    havingClause_.clear();
    limit_ = -1;
    offset_ = -1;
}

QueryBuilder& MySqlBuilder::select(const std::vector<std::string>& columns) {
    columns_.clear();
    for (const auto& col : columns) {
        // MySQL 使用反引号包裹列名（避免关键字冲突）
        if (col.find('(') == std::string::npos && col != "*") {
            columns_.push_back("`" + col + "`");
        } else {
            columns_.push_back(col); // 聚合函数不加反引号
        }
    }
    return *this;
}

QueryBuilder& MySqlBuilder::from(const std::string& table) {
    table_ = "`" + table + "`";
    return *this;
}

QueryBuilder& MySqlBuilder::where(const std::string& condition) {
    whereConditions_.clear();
    whereConditions_.push_back(condition);
    return *this;
}

QueryBuilder& MySqlBuilder::andWhere(const std::string& condition) {
    whereConditions_.push_back("AND " + condition);
    return *this;
}

QueryBuilder& MySqlBuilder::orWhere(const std::string& condition) {
    whereConditions_.push_back("OR " + condition);
    return *this;
}

QueryBuilder& MySqlBuilder::join(const std::string& table, const std::string& on) {
    joinClauses_.push_back("JOIN `" + table + "` ON " + on);
    return *this;
}

QueryBuilder& MySqlBuilder::leftJoin(const std::string& table, const std::string& on) {
    joinClauses_.push_back("LEFT JOIN `" + table + "` ON " + on);
    return *this;
}

QueryBuilder& MySqlBuilder::orderBy(const std::string& column, const std::string& direction) {
    orderByClauses_.emplace_back("`" + column + "`", direction);
    return *this;
}

QueryBuilder& MySqlBuilder::groupBy(const std::string& column) {
    groupByClauses_.push_back("`" + column + "`");
    return *this;
}

QueryBuilder& MySqlBuilder::having(const std::string& condition) {
    havingClause_ = condition;
    return *this;
}

QueryBuilder& MySqlBuilder::limit(int count) {
    limit_ = count;
    return *this;
}

QueryBuilder& MySqlBuilder::offset(int count) {
    offset_ = count;
    return *this;
}

std::unique_ptr<SqlQuery> MySqlBuilder::build() {
    query_->setType("SELECT"); // MySQL 方言标记
    query_->setColumns(columns_);
    query_->setTable(table_);

    if (!whereConditions_.empty()) {
        std::ostringstream where;
        for (size_t i = 0; i < whereConditions_.size(); ++i) {
            where << whereConditions_[i];
            if (i + 1 < whereConditions_.size()) where << " ";
        }
        query_->setWhereClause(where.str());
    }

    if (!joinClauses_.empty()) {
        std::ostringstream joins;
        for (size_t i = 0; i < joinClauses_.size(); ++i) {
            joins << joinClauses_[i];
            if (i + 1 < joinClauses_.size()) joins << "\n";
        }
        query_->setJoinClause(joins.str());
    }

    if (!orderByClauses_.empty()) {
        std::ostringstream order;
        for (size_t i = 0; i < orderByClauses_.size(); ++i) {
            order << orderByClauses_[i].first << " " << orderByClauses_[i].second;
            if (i + 1 < orderByClauses_.size()) order << ", ";
        }
        query_->setOrderByClause(order.str());
    }

    if (!groupByClauses_.empty()) {
        std::ostringstream group;
        for (size_t i = 0; i < groupByClauses_.size(); ++i) {
            group << groupByClauses_[i];
            if (i + 1 < groupByClauses_.size()) group << ", ";
        }
        query_->setGroupByClause(group.str());
    }

    if (!havingClause_.empty()) {
        query_->setHavingClause(havingClause_);
    }

    query_->setLimit(limit_);
    query_->setOffset(offset_);

    auto result = std::move(query_);
    reset();
    return result;
}

// ===================== QueryDirector 实现 =====================
QueryDirector::QueryDirector(QueryBuilder& builder) : builder_(builder) {}

// 构建分页列表查询
std::unique_ptr<SqlQuery> QueryDirector::buildPaginatedList(
    const std::string& table,
    const std::vector<std::string>& columns,
    int page, int pageSize)
{
    builder_.reset();
    return builder_
        .select(columns)
        .from(table)
        .orderBy("id", "ASC")
        .limit(pageSize)
        .offset((page - 1) * pageSize)
        .build();
}

// 构建聚合统计查询
std::unique_ptr<SqlQuery> QueryDirector::buildAggregation(
    const std::string& table,
    const std::string& aggregateColumn,
    const std::string& groupColumn,
    const std::string& condition)
{
    builder_.reset();
    builder_
        .select({"COUNT(*) AS total",
                 "AVG(" + aggregateColumn + ") AS avg_value",
                 groupColumn})
        .from(table)
        .groupBy(groupColumn)
        .orderBy("total", "DESC");

    if (!condition.empty()) {
        builder_.where(condition);
    }

    return builder_.build();
}

// 构建关联查询
std::unique_ptr<SqlQuery> QueryDirector::buildJoinQuery(
    const std::string& mainTable,
    const std::string& joinTable,
    const std::string& joinCondition,
    const std::vector<std::string>& columns)
{
    builder_.reset();
    return builder_
        .select(columns)
        .from(mainTable)
        .join(joinTable, joinCondition)
        .build();
}

// ===================== 客户端演示 =====================
int main() {
    std::cout << "========================================\n";
    std::cout << " Builder Pattern Demo\n";
    std::cout << " Scenario: SQL Query Builder\n";
    std::cout << "========================================\n\n";

    // 1. 使用 StandardSqlBuilder 手动构建复杂查询
    std::cout << "--- 1. Manual query building (Standard SQL) ---\n\n";
    StandardSqlBuilder stdBuilder;

    auto query1 = stdBuilder
        .select({"u.id", "u.name", "u.email", "o.total_amount"})
        .from("users u")
        .join("orders o", "u.id = o.user_id")
        .leftJoin("profiles p", "u.id = p.user_id")
        .where("u.status = 'active'")
        .andWhere("o.created_at > '2024-01-01'")
        .orderBy("o.total_amount", "DESC")
        .orderBy("u.name", "ASC")
        .limit(20)
        .offset(0)
        .build();

    std::cout << query1->toSql() << "\n\n";

    // 2. 使用 MySqlBuilder 构建同样逻辑的查询（MySQL 方言）
    std::cout << "--- 2. Same query with MySQL dialect ---\n\n";
    MySqlBuilder mysqlBuilder;

    auto query2 = mysqlBuilder
        .select({"u.id", "u.name", "u.email", "o.total_amount"})
        .from("users")
        .join("orders", "u.id = o.user_id")
        .where("u.status = 'active'")
        .andWhere("o.created_at > '2024-01-01'")
        .orderBy("total_amount", "DESC")
        .limit(20)
        .offset(0)
        .build();

    std::cout << query2->toSql() << "\n\n";

    // 3. 使用 Director 构建预定义的查询模式
    std::cout << "--- 3. Director: Paginated list (page 3, 10 per page) ---\n\n";
    QueryDirector director(stdBuilder);

    auto paginatedQuery = director.buildPaginatedList(
        "products",
        {"id", "name", "price", "stock"},
        3, 10);

    std::cout << paginatedQuery->toSql() << "\n\n";

    // 4. Director 构建聚合查询
    std::cout << "--- 4. Director: Aggregation query ---\n\n";
    auto aggQuery = director.buildAggregation(
        "orders", "total_amount", "category",
        "created_at > '2024-01-01'");

    std::cout << aggQuery->toSql() << "\n\n";

    // 5. Director 构建关联查询
    std::cout << "--- 5. Director: Join query ---\n\n";
    auto joinQuery = director.buildJoinQuery(
        "employees", "departments",
        "employees.dept_id = departments.id",
        {"employees.name", "departments.name AS dept_name", "employees.salary"});

    std::cout << joinQuery->toSql() << "\n\n";

    // 6. 复用建造者：构建完全不同的查询
    std::cout << "--- 6. Builder reuse: different query ---\n\n";
    auto query3 = stdBuilder
        .select({"department", "COUNT(*) AS emp_count", "AVG(salary) AS avg_salary"})
        .from("employees")
        .where("hire_date >= '2020-01-01'")
        .groupBy("department")
        .having("COUNT(*) > 5")
        .orderBy("avg_salary", "DESC")
        .build();

    std::cout << query3->toSql() << "\n\n";

    std::cout << "========================================\n";
    std::cout << "Key insight: The Builder separates the construction\n";
    std::cout << "of a complex SQL query from its representation.\n";
    std::cout << "The same building process can create different\n";
    std::cout << "SQL dialects (Standard vs MySQL). The Director\n";
    std::cout << "encapsulates common query patterns for reuse.\n";
    std::cout << "========================================\n";

    return 0;
}
```

## 构建方式

```cmake title="CMakeLists.txt"
add_executable(Pattern_Builder Builder.cpp Builder.h)
target_compile_features(Pattern_Builder PRIVATE cxx_std_17)
```

:::tip 编译运行
```bash
cd build
cmake --build . --target Pattern_Builder
./Pattern_Builder
```
:::
