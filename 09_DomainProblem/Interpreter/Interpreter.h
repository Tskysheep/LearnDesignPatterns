#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <sstream>
#include <vector>
#include <stdexcept>

// ============================================================================
// 解释器模式（Interpreter Pattern）
// 场景：数学表达式求值器
//   解析并计算如 "3 + 5 * (2 - 1)" 的表达式，
//   支持变量绑定（如 "x + y * 2"），
//   构建抽象语法树（AST），由终结符和非终结符表达式节点组成。
// ============================================================================

// --- 上下文：存储变量名到值的映射 ---
// 解释器模式中的 Context 角色，为表达式求值提供外部环境
class Context {
public:
    void setVariable(const std::string& name, double value);
    double getVariable(const std::string& name) const;
    bool hasVariable(const std::string& name) const;
    std::string dump() const;

private:
    std::unordered_map<std::string, double> variables_;
};

// --- 抽象表达式：AST 节点的公共接口 ---
// 所有终结符和非终结符表达式都实现此接口
class Expression {
public:
    virtual ~Expression() = default;

    // 在给定上下文中对表达式求值
    virtual double interpret(const Context& context) const = 0;

    // 返回表达式的字符串表示（用于调试/展示）
    virtual std::string toString() const = 0;
};

using ExprPtr = std::unique_ptr<Expression>;

// ============================================================================
// 终结符表达式（Terminal Expression）
//   对应文法中的终结符，不能再分解为更小的表达式
// ============================================================================

// --- 数字字面量：如 3, 5.5 ---
class NumberExpression : public Expression {
public:
    explicit NumberExpression(double value);
    double interpret(const Context& context) const override;
    std::string toString() const override;

private:
    double value_;
};

// --- 变量引用：如 x, y ---
// 通过 Context 查找变量值，若变量未定义则抛出异常
class VariableExpression : public Expression {
public:
    explicit VariableExpression(const std::string& name);
    double interpret(const Context& context) const override;
    std::string toString() const override;

private:
    std::string name_;
};

// ============================================================================
// 非终结符表达式（Non-terminal Expression）
//   由更小的表达式组合而成，对应文法中的产生式规则
//   每种运算符（+、-、*、/）对应一个非终结符表达式类
// ============================================================================

// --- 加法表达式 ---
class AddExpression : public Expression {
public:
    AddExpression(ExprPtr left, ExprPtr right);
    double interpret(const Context& context) const override;
    std::string toString() const override;

private:
    ExprPtr left_;
    ExprPtr right_;
};

// --- 减法表达式 ---
class SubtractExpression : public Expression {
public:
    SubtractExpression(ExprPtr left, ExprPtr right);
    double interpret(const Context& context) const override;
    std::string toString() const override;

private:
    ExprPtr left_;
    ExprPtr right_;
};

// --- 乘法表达式 ---
class MultiplyExpression : public Expression {
public:
    MultiplyExpression(ExprPtr left, ExprPtr right);
    double interpret(const Context& context) const override;
    std::string toString() const override;

private:
    ExprPtr left_;
    ExprPtr right_;
};

// --- 除法表达式 ---
class DivideExpression : public Expression {
public:
    DivideExpression(ExprPtr left, ExprPtr right);
    double interpret(const Context& context) const override;
    std::string toString() const override;

private:
    ExprPtr left_;
    ExprPtr right_;
};

// --- 取负表达式（一元运算符） ---
class NegateExpression : public Expression {
public:
    explicit NegateExpression(ExprPtr operand);
    double interpret(const Context& context) const override;
    std::string toString() const override;

private:
    ExprPtr operand_;
};

// ============================================================================
// 词法分析器（Tokenizer / Lexer）
//   将输入字符串拆分为 Token 序列，供 Parser 使用。
//   虽然 Tokenizer 不是解释器模式的核心角色，但它是构建实际
//   解释器不可缺少的前置步骤。
// ============================================================================

enum class TokenType {
    Number,     // 数字字面量
    Identifier, // 变量名
    Plus,       // +
    Minus,      // -
    Star,       // *
    Slash,      // /
    LParen,     // (
    RParen,     // )
    End         // 输入结束
};

struct Token {
    TokenType type;
    std::string text;
    double numValue = 0.0;
};

class Tokenizer {
public:
    explicit Tokenizer(const std::string& input);
    std::vector<Token> tokenize();

private:
    std::string input_;
    size_t pos_ = 0;

    char peek() const;
    char advance();
    void skipWhitespace();
    Token readNumber();
    Token readIdentifier();
};

// ============================================================================
// 语法分析器（Parser）
//   采用递归下降法，根据运算符优先级将 Token 序列解析为 AST。
//   文法规则（EBNF 形式）：
//     expression  = term (('+' | '-') term)*
//     term        = unary (('*' | '/') unary)*
//     unary       = '-' unary | primary
//     primary     = NUMBER | IDENTIFIER | '(' expression ')'
//
//   这里的文法层次直接体现了运算符优先级：
//   乘除 > 加减，括号最高优先级。
// ============================================================================

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    // 解析并返回 AST 根节点
    ExprPtr parse();

private:
    std::vector<Token> tokens_;
    size_t pos_ = 0;

    const Token& current() const;
    const Token& advance();
    bool match(TokenType type);
    void expect(TokenType type, const std::string& errorMsg);

    // 递归下降解析各层级
    ExprPtr parseExpression();  // 加减
    ExprPtr parseTerm();        // 乘除
    ExprPtr parseUnary();       // 一元负号
    ExprPtr parsePrimary();     // 数字、变量、括号
};

// ============================================================================
// 表达式求值器（Evaluator）—— 客户端角色
//   封装了 Tokenizer + Parser + Context 的完整流程，
//   为外部提供简洁的表达式求值接口。
// ============================================================================

class ExpressionEvaluator {
public:
    ExpressionEvaluator() = default;

    // 设置变量
    void setVariable(const std::string& name, double value);

    // 解析并求值表达式字符串
    double evaluate(const std::string& expression);

    // 解析表达式并返回 AST（不求值）
    ExprPtr parseToAST(const std::string& expression);

    // 对已有 AST 求值
    double evaluateAST(const Expression& ast) const;

private:
    Context context_;
};

#endif // INTERPRETER_H
