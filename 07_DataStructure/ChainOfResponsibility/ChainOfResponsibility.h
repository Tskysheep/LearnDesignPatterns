#ifndef CHAIN_OF_RESPONSIBILITY_H
#define CHAIN_OF_RESPONSIBILITY_H

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <ctime>

// ============================================================
// 责任链模式（Chain of Responsibility Pattern）
// 场景：技术支持工单系统 —— 不同级别的支持人员（L1、L2、经理、总监）
//       根据工单严重程度逐级处理，无��处理时传递给上一级。
// ============================================================

// 工单严重程度
enum class Severity {
    Minor = 1,     // 小问题：密码重置、FAQ
    Moderate = 2,  // 中等问题：软件配置、权限申请
    Major = 3,     // 重大问题：系统故障、数据异常
    Critical = 4   // 紧急问题：生产宕机、安全漏洞
};

// 工单类别
enum class TicketCategory {
    Account,       // 账户问题
    Software,      // 软件问题
    Hardware,      // 硬件问题
    Network,       // 网络问题
    Security,      // 安全问题
    DataRecovery   // 数据恢复
};

// 工单实体
struct SupportTicket {
    int id;
    std::string customerName;
    std::string description;
    Severity severity;
    TicketCategory category;
    std::time_t createdAt;

    // 处理记录
    std::vector<std::string> handlingLog;

    SupportTicket(int id, const std::string& customerName,
                  const std::string& description,
                  Severity severity, TicketCategory category);

    std::string getSeverityString() const;
    std::string getCategoryString() const;
    void addLog(const std::string& handler, const std::string& action);
};

// ============================================================
// 抽象处理者（Handler）
// 设计要点：
//   1. 持有指向下一个处理者的指针（链式结构）
//   2. 定义处理请求的模板方法：先尝试自己处理，不能处理则转发
// ============================================================
class SupportHandler {
public:
    explicit SupportHandler(const std::string& name, const std::string& level);
    virtual ~SupportHandler() = default;

    // 设置下一个处理者，返回下一个处理者的引用（支持链式调用）
    SupportHandler& setNext(std::shared_ptr<SupportHandler> next);

    // 获取下一个处理者
    std::shared_ptr<SupportHandler> getNext() const { return next_; }

    // 处理工单的模板方法
    // 设计要点：非虚接口（NVI），固定"尝试处理 -> 转发"的流程
    void handle(SupportTicket& ticket);

    const std::string& getName() const { return name_; }
    const std::string& getLevel() const { return level_; }

protected:
    // 子类实现：判断是否能处理该工单
    virtual bool canHandle(const SupportTicket& ticket) const = 0;

    // 子类实现：实际处理逻辑
    virtual void processTicket(SupportTicket& ticket) = 0;

private:
    std::string name_;
    std::string level_;
    std::shared_ptr<SupportHandler> next_;  // 链中的下一个处理者
};

// ============================================================
// 具体处理者：一级技术支持（Level 1）
// 处理 Minor 级别的常见问题
// ============================================================
class Level1Support : public SupportHandler {
public:
    explicit Level1Support(const std::string& name);

protected:
    bool canHandle(const SupportTicket& ticket) const override;
    void processTicket(SupportTicket& ticket) override;
};

// ============================================================
// 具体处理者：二级技术支持（Level 2）
// 处理 Moderate 级别的技术问题
// ============================================================
class Level2Support : public SupportHandler {
public:
    explicit Level2Support(const std::string& name);

protected:
    bool canHandle(const SupportTicket& ticket) const override;
    void processTicket(SupportTicket& ticket) override;
};

// ============================================================
// 具体处理者：技术经理（Manager）
// 处理 Major 级别的重大问题
// ============================================================
class ManagerSupport : public SupportHandler {
public:
    explicit ManagerSupport(const std::string& name);

protected:
    bool canHandle(const SupportTicket& ticket) const override;
    void processTicket(SupportTicket& ticket) override;
};

// ============================================================
// 具体处理者：技术总监（Director）
// 处理 Critical 级别的紧急问题 —— 责任链的最后兜底
// ============================================================
class DirectorSupport : public SupportHandler {
public:
    explicit DirectorSupport(const std::string& name);

protected:
    bool canHandle(const SupportTicket& ticket) const override;
    void processTicket(SupportTicket& ticket) override;
};

// ============================================================
// 辅助：工单系统门面（Facade），负责构建链和提交工单
// ============================================================
class SupportSystem {
public:
    SupportSystem();

    void submitTicket(SupportTicket& ticket);
    void printChain() const;

private:
    std::shared_ptr<SupportHandler> chainHead_;
    std::vector<std::shared_ptr<SupportHandler>> handlers_;
};

#endif // CHAIN_OF_RESPONSIBILITY_H
