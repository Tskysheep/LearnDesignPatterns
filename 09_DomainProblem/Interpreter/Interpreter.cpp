#include "Interpreter.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cassert>

// ============================================================================
// Context 实现
// ============================================================================

void Context::setVariable(const std::string& name, double value) {
    variables_[name] = value;
}

double Context::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        throw std::runtime_error("Undefined variable: " + name);
    }
    return it->second;
}

bool Context::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

std::string Context::dump() const {
    std::ostringstream oss;
    oss << "{ ";
    bool first = true;
    for (const auto& [name, value] : variables_) {
        if (!first) oss << ", ";
        oss << name << " = " << value;
        first = false;
    }
    oss << " }";
    return oss.str();
}

// ============================================================================
// 终结符表达式实现
// ============================================================================

// --- NumberExpression ---

NumberExpression::NumberExpression(double value) : value_(value) {}

double NumberExpression::interpret(const Context& /*context*/) const {
    // 终结符表达式：直接返回自身数值，不依赖上下文
    return value_;
}

std::string NumberExpression::toString() const {
    std::ostringstream oss;
    // 如果是整数则不显示小数点
    if (value_ == static_cast<int>(value_)) {
        oss << static_cast<int>(value_);
    } else {
        oss << value_;
    }
    return oss.str();
}

// --- VariableExpression ---

VariableExpression::VariableExpression(const std::string& name) : name_(name) {}

double VariableExpression::interpret(const Context& context) const {
    // 终结符表达式：从上下文中查找变量值
    // 若变量未定义，Context::getVariable 会抛出异常
    return context.getVariable(name_);
}

std::string VariableExpression::toString() const {
    return name_;
}

// ============================================================================
// 非终结符表达式实现
//   每个非终结符表达式递归地对子表达式求值，然后组合结果。
//   这就是解释器模式的核心——递归地 interpret 整棵 AST。
// ============================================================================

// --- AddExpression ---

AddExpression::AddExpression(ExprPtr left, ExprPtr right)
    : left_(std::move(left)), right_(std::move(right)) {}

double AddExpression::interpret(const Context& context) const {
    return left_->interpret(context) + right_->interpret(context);
}

std::string AddExpression::toString() const {
    return "(" + left_->toString() + " + " + right_->toString() + ")";
}

// --- SubtractExpression ---

SubtractExpression::SubtractExpression(ExprPtr left, ExprPtr right)
    : left_(std::move(left)), right_(std::move(right)) {}

double SubtractExpression::interpret(const Context& context) const {
    return left_->interpret(context) - right_->interpret(context);
}

std::string SubtractExpression::toString() const {
    return "(" + left_->toString() + " - " + right_->toString() + ")";
}

// --- MultiplyExpression ---

MultiplyExpression::MultiplyExpression(ExprPtr left, ExprPtr right)
    : left_(std::move(left)), right_(std::move(right)) {}

double MultiplyExpression::interpret(const Context& context) const {
    return left_->interpret(context) * right_->interpret(context);
}

std::string MultiplyExpression::toString() const {
    return "(" + left_->toString() + " * " + right_->toString() + ")";
}

// --- DivideExpression ---

DivideExpression::DivideExpression(ExprPtr left, ExprPtr right)
    : left_(std::move(left)), right_(std::move(right)) {}

double DivideExpression::interpret(const Context& context) const {
    double divisor = right_->interpret(context);
    if (std::abs(divisor) < 1e-12) {
        throw std::runtime_error("Division by zero");
    }
    return left_->interpret(context) / divisor;
}

std::string DivideExpression::toString() const {
    return "(" + left_->toString() + " / " + right_->toString() + ")";
}

// --- NegateExpression ---

NegateExpression::NegateExpression(ExprPtr operand)
    : operand_(std::move(operand)) {}

double NegateExpression::interpret(const Context& context) const {
    return -(operand_->interpret(context));
}

std::string NegateExpression::toString() const {
    return "(-" + operand_->toString() + ")";
}

// ============================================================================
// Tokenizer 实现
//   将原始字符串切分为一个个 Token，是语法分析的前置步骤。
// ============================================================================

Tokenizer::Tokenizer(const std::string& input) : input_(input) {}

char Tokenizer::peek() const {
    return pos_ < input_.size() ? input_[pos_] : '\0';
}

char Tokenizer::advance() {
    return input_[pos_++];
}

void Tokenizer::skipWhitespace() {
    while (pos_ < input_.size() && std::isspace(input_[pos_])) {
        ++pos_;
    }
}

Token Tokenizer::readNumber() {
    size_t start = pos_;
    while (pos_ < input_.size() && (std::isdigit(input_[pos_]) || input_[pos_] == '.')) {
        ++pos_;
    }
    std::string text = input_.substr(start, pos_ - start);
    return Token{TokenType::Number, text, std::stod(text)};
}

Token Tokenizer::readIdentifier() {
    size_t start = pos_;
    while (pos_ < input_.size() && (std::isalnum(input_[pos_]) || input_[pos_] == '_')) {
        ++pos_;
    }
    std::string text = input_.substr(start, pos_ - start);
    return Token{TokenType::Identifier, text, 0.0};
}

std::vector<Token> Tokenizer::tokenize() {
    std::vector<Token> tokens;
    while (pos_ < input_.size()) {
        skipWhitespace();
        if (pos_ >= input_.size()) break;

        char c = peek();
        if (std::isdigit(c) || (c == '.' && pos_ + 1 < input_.size() && std::isdigit(input_[pos_ + 1]))) {
            tokens.push_back(readNumber());
        } else if (std::isalpha(c) || c == '_') {
            tokens.push_back(readIdentifier());
        } else {
            switch (c) {
                case '+': tokens.push_back({TokenType::Plus, "+"}); advance(); break;
                case '-': tokens.push_back({TokenType::Minus, "-"}); advance(); break;
                case '*': tokens.push_back({TokenType::Star, "*"}); advance(); break;
                case '/': tokens.push_back({TokenType::Slash, "/"}); advance(); break;
                case '(': tokens.push_back({TokenType::LParen, "("}); advance(); break;
                case ')': tokens.push_back({TokenType::RParen, ")"}); advance(); break;
                default:
                    throw std::runtime_error(
                        std::string("Unexpected character: '") + c + "' at position " + std::to_string(pos_));
            }
        }
    }
    tokens.push_back({TokenType::End, ""});
    return tokens;
}

// ============================================================================
// Parser 实现
//   递归下降解析器：按运算符优先级逐层解析，构建 AST。
//   关键设计：每一层文法规则对应一个解析方法，
//   低优先级运算符在外层，高优先级在内层。
// ============================================================================

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

const Token& Parser::current() const {
    return tokens_[pos_];
}

const Token& Parser::advance() {
    return tokens_[pos_++];
}

bool Parser::match(TokenType type) {
    if (current().type == type) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(TokenType type, const std::string& errorMsg) {
    if (!match(type)) {
        throw std::runtime_error(errorMsg + " (got '" + current().text + "')");
    }
}

ExprPtr Parser::parse() {
    auto result = parseExpression();
    if (current().type != TokenType::End) {
        throw std::runtime_error("Unexpected token after expression: '" + current().text + "'");
    }
    return result;
}

// expression = term (('+' | '-') term)*
// 加减法层：最低优先级的二元运算符
ExprPtr Parser::parseExpression() {
    auto left = parseTerm();

    while (current().type == TokenType::Plus || current().type == TokenType::Minus) {
        TokenType op = current().type;
        advance();
        auto right = parseTerm();

        if (op == TokenType::Plus) {
            left = std::make_unique<AddExpression>(std::move(left), std::move(right));
        } else {
            left = std::make_unique<SubtractExpression>(std::move(left), std::move(right));
        }
    }

    return left;
}

// term = unary (('*' | '/') unary)*
// 乘除法层：高于加减的优先级
ExprPtr Parser::parseTerm() {
    auto left = parseUnary();

    while (current().type == TokenType::Star || current().type == TokenType::Slash) {
        TokenType op = current().type;
        advance();
        auto right = parseUnary();

        if (op == TokenType::Star) {
            left = std::make_unique<MultiplyExpression>(std::move(left), std::move(right));
        } else {
            left = std::make_unique<DivideExpression>(std::move(left), std::move(right));
        }
    }

    return left;
}

// unary = '-' unary | primary
// 一元负号：右结合，优先级高于乘除
ExprPtr Parser::parseUnary() {
    if (match(TokenType::Minus)) {
        auto operand = parseUnary();
        return std::make_unique<NegateExpression>(std::move(operand));
    }
    return parsePrimary();
}

// primary = NUMBER | IDENTIFIER | '(' expression ')'
// 基本表达式：最高优先级（数字、变量、括号子表达式）
ExprPtr Parser::parsePrimary() {
    // 数字字面量
    if (current().type == TokenType::Number) {
        double val = current().numValue;
        advance();
        return std::make_unique<NumberExpression>(val);
    }

    // 变量引用
    if (current().type == TokenType::Identifier) {
        std::string name = current().text;
        advance();
        return std::make_unique<VariableExpression>(name);
    }

    // 括号子表达式
    if (match(TokenType::LParen)) {
        auto expr = parseExpression();
        expect(TokenType::RParen, "Expected closing ')'");
        return expr;
    }

    throw std::runtime_error("Unexpected token: '" + current().text + "'");
}

// ============================================================================
// ExpressionEvaluator 实现
// ============================================================================

void ExpressionEvaluator::setVariable(const std::string& name, double value) {
    context_.setVariable(name, value);
}

double ExpressionEvaluator::evaluate(const std::string& expression) {
    auto ast = parseToAST(expression);
    return ast->interpret(context_);
}

ExprPtr ExpressionEvaluator::parseToAST(const std::string& expression) {
    Tokenizer tokenizer(expression);
    auto tokens = tokenizer.tokenize();
    Parser parser(tokens);
    return parser.parse();
}

double ExpressionEvaluator::evaluateAST(const Expression& ast) const {
    return ast.interpret(context_);
}

// ============================================================================
// 辅助函数
// ============================================================================

static void printSeparator(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " " << title << "\n";
    std::cout << std::string(60, '=') << "\n";
}

static void evaluateAndPrint(ExpressionEvaluator& eval, const std::string& expr) {
    try {
        auto ast = eval.parseToAST(expr);
        double result = eval.evaluateAST(*ast);
        std::cout << "  Expression : " << expr << "\n";
        std::cout << "  AST        : " << ast->toString() << "\n";
        std::cout << "  Result     : " << result << "\n\n";
    } catch (const std::exception& e) {
        std::cout << "  Expression : " << expr << "\n";
        std::cout << "  Error      : " << e.what() << "\n\n";
    }
}

// ============================================================================
// main() —— 演示解释器模式的完整流程
// ============================================================================

int main() {
    std::cout << "=== Interpreter Pattern Demo: Mathematical Expression Evaluator ===\n";

    // --------------------------------------------------
    // 演示 1：基本算术运算
    // --------------------------------------------------
    printSeparator("Demo 1: Basic Arithmetic");
    std::cout << "Evaluating simple arithmetic expressions:\n\n";

    ExpressionEvaluator eval;
    evaluateAndPrint(eval, "3 + 5");
    evaluateAndPrint(eval, "10 - 4");
    evaluateAndPrint(eval, "6 * 7");
    evaluateAndPrint(eval, "20 / 4");

    // --------------------------------------------------
    // 演示 2：运算符优先级和括号
    //   解释器模式的文法规则天然编码了优先级：
    //   乘除在 parseTerm() 层解析，加减在 parseExpression() 层，
    //   括号在 parsePrimary() 层触发递归回到 parseExpression()。
    // --------------------------------------------------
    printSeparator("Demo 2: Operator Precedence & Parentheses");
    std::cout << "Demonstrating correct operator precedence:\n\n";

    evaluateAndPrint(eval, "3 + 5 * 2");          // = 13, 非 16
    evaluateAndPrint(eval, "(3 + 5) * 2");         // = 16
    evaluateAndPrint(eval, "3 + 5 * (2 - 1)");    // = 8
    evaluateAndPrint(eval, "10 - 2 * 3 + 1");     // = 5
    evaluateAndPrint(eval, "(10 - 2) * (3 + 1)"); // = 32

    // --------------------------------------------------
    // 演示 3：带变量的表达式
    //   变量由 Context 管理。VariableExpression 在 interpret() 时
    //   从 Context 中查找变量值。同一棵 AST 可以在不同 Context 下
    //   求出不同的结果——这正是解释器模式灵活性的体现。
    // --------------------------------------------------
    printSeparator("Demo 3: Variable Expressions");
    std::cout << "Setting variables: x = 10, y = 3, z = 2\n\n";

    eval.setVariable("x", 10);
    eval.setVariable("y", 3);
    eval.setVariable("z", 2);

    evaluateAndPrint(eval, "x + y");
    evaluateAndPrint(eval, "x * y - z");
    evaluateAndPrint(eval, "x / (y + z)");
    evaluateAndPrint(eval, "(x - y) * (x + z)");

    // --------------------------------------------------
    // 演示 4：同一 AST 在不同上下文中求值
    //   构建一次 AST，改变变量值后重新求值。
    //   这展示了 AST 与 Context 分离的优势。
    // --------------------------------------------------
    printSeparator("Demo 4: Same AST, Different Contexts");

    auto ast = eval.parseToAST("x * x + y * y");
    std::cout << "AST: " << ast->toString() << "\n\n";

    // 场景 A: x=3, y=4 => 25
    ExpressionEvaluator evalA;
    evalA.setVariable("x", 3);
    evalA.setVariable("y", 4);
    std::cout << "  Context A: x=3, y=4  =>  " << evalA.evaluateAST(*ast) << "\n";

    // 场景 B: x=5, y=12 => 169
    ExpressionEvaluator evalB;
    evalB.setVariable("x", 5);
    evalB.setVariable("y", 12);
    std::cout << "  Context B: x=5, y=12 =>  " << evalB.evaluateAST(*ast) << "\n";

    // 场景 C: x=1, y=1 => 2
    ExpressionEvaluator evalC;
    evalC.setVariable("x", 1);
    evalC.setVariable("y", 1);
    std::cout << "  Context C: x=1, y=1  =>  " << evalC.evaluateAST(*ast) << "\n";

    // --------------------------------------------------
    // 演示 5：一元负号
    // --------------------------------------------------
    printSeparator("Demo 5: Unary Negation");

    evaluateAndPrint(eval, "-5");
    evaluateAndPrint(eval, "-x + y");
    evaluateAndPrint(eval, "-(x + y)");
    evaluateAndPrint(eval, "--x");  // 双重否定

    // --------------------------------------------------
    // 演示 6：复杂嵌套表达式
    // --------------------------------------------------
    printSeparator("Demo 6: Complex Nested Expressions");

    eval.setVariable("a", 2);
    eval.setVariable("b", 5);
    eval.setVariable("c", 3);

    evaluateAndPrint(eval, "a * (b + c) - (b - a) * c");
    evaluateAndPrint(eval, "(a + b) * (b + c) / (a + c)");
    evaluateAndPrint(eval, "a * a + b * b + c * c");

    // --------------------------------------------------
    // 演示 7：错误处理
    //   展示解释器对非法输入的处理能力
    // --------------------------------------------------
    printSeparator("Demo 7: Error Handling");
    std::cout << "Testing error conditions:\n\n";

    evaluateAndPrint(eval, "5 / 0");             // 除零
    evaluateAndPrint(eval, "x + undefined_var");  // 未定义变量
    evaluateAndPrint(eval, "3 + + 5");           // 语法错误

    // --------------------------------------------------
    // 演示 8：手动构建 AST（不经过 Parser）
    //   直接用表达式节点类构造 AST，展示模式的本质：
    //   解释器模式的核心是 Expression 继承体系和 interpret()，
    //   Parser 只是构建 AST 的一种手段。
    // --------------------------------------------------
    printSeparator("Demo 8: Manually Building AST");
    std::cout << "Building AST for '(3 + x) * (y - 1)' by hand:\n\n";

    // 手动构建：(3 + x) * (y - 1)
    auto manualAST = std::make_unique<MultiplyExpression>(
        std::make_unique<AddExpression>(
            std::make_unique<NumberExpression>(3),
            std::make_unique<VariableExpression>("x")
        ),
        std::make_unique<SubtractExpression>(
            std::make_unique<VariableExpression>("y"),
            std::make_unique<NumberExpression>(1)
        )
    );

    Context ctx;
    ctx.setVariable("x", 7);
    ctx.setVariable("y", 5);

    std::cout << "  AST      : " << manualAST->toString() << "\n";
    std::cout << "  Context  : " << ctx.dump() << "\n";
    std::cout << "  Result   : " << manualAST->interpret(ctx) << "\n";
    std::cout << "  Expected : (3 + 7) * (5 - 1) = 10 * 4 = 40\n";

    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << " All demos completed.\n";
    std::cout << std::string(60, '=') << "\n";

    return 0;
}
