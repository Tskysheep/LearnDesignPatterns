#include "ChainOfResponsibility.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// ============================================================
// SupportTicket 实现
// ============================================================

SupportTicket::SupportTicket(int id, const std::string& customerName,
                             const std::string& description,
                             Severity severity, TicketCategory category)
    : id(id), customerName(customerName), description(description),
      severity(severity), category(category), createdAt(std::time(nullptr)) {}

std::string SupportTicket::getSeverityString() const {
    switch (severity) {
        case Severity::Minor:    return "Minor";
        case Severity::Moderate: return "Moderate";
        case Severity::Major:    return "Major";
        case Severity::Critical: return "Critical";
    }
    return "Unknown";
}

std::string SupportTicket::getCategoryString() const {
    switch (category) {
        case TicketCategory::Account:      return "Account";
        case TicketCategory::Software:     return "Software";
        case TicketCategory::Hardware:     return "Hardware";
        case TicketCategory::Network:      return "Network";
        case TicketCategory::Security:     return "Security";
        case TicketCategory::DataRecovery: return "DataRecovery";
    }
    return "Unknown";
}

void SupportTicket::addLog(const std::string& handler, const std::string& action) {
    std::ostringstream oss;
    oss << "[" << handler << "] " << action;
    handlingLog.push_back(oss.str());
}

// ============================================================
// SupportHandler 基类实现
// ============================================================

SupportHandler::SupportHandler(const std::string& name, const std::string& level)
    : name_(name), level_(level), next_(nullptr) {}

SupportHandler& SupportHandler::setNext(std::shared_ptr<SupportHandler> next) {
    next_ = std::move(next);
    return *next_;
}

// 模板方法：定义责任链的转发逻辑
// 设计要点：先尝试自己处理，处理不了则转发给链中的下一个处理者
void SupportHandler::handle(SupportTicket& ticket) {
    if (canHandle(ticket)) {
        ticket.addLog(level_ + " - " + name_, "Accepted ticket");
        processTicket(ticket);
    } else {
        ticket.addLog(level_ + " - " + name_, "Cannot handle, forwarding...");
        if (next_) {
            // 传递给链中的下一个处理者
            next_->handle(ticket);
        } else {
            // 到达链尾仍无人处理 —— 记录未解决状态
            ticket.addLog("SYSTEM", "No handler available! Ticket escalated to external support.");
        }
    }
}

// ============================================================
// Level1Support 实现 —— 处理简单问题
// ============================================================

Level1Support::Level1Support(const std::string& name)
    : SupportHandler(name, "L1") {}

bool Level1Support::canHandle(const SupportTicket& ticket) const {
    // L1 只处理 Minor 级别的问题
    return ticket.severity == Severity::Minor;
}

void Level1Support::processTicket(SupportTicket& ticket) {
    std::string resolution;
    switch (ticket.category) {
        case TicketCategory::Account:
            resolution = "Reset password and verified account access.";
            break;
        case TicketCategory::Software:
            resolution = "Guided user through basic troubleshooting steps.";
            break;
        default:
            resolution = "Provided FAQ documentation and basic guidance.";
            break;
    }
    ticket.addLog("L1 - " + getName(), "RESOLVED: " + resolution);
}

// ============================================================
// Level2Support 实现 —— 处理中等技术问题
// ============================================================

Level2Support::Level2Support(const std::string& name)
    : SupportHandler(name, "L2") {}

bool Level2Support::canHandle(const SupportTicket& ticket) const {
    // L2 处理 Moderate 级别，或 Minor 级别的硬件/网络问题（L1 无法解决时上升）
    return ticket.severity == Severity::Moderate;
}

void Level2Support::processTicket(SupportTicket& ticket) {
    std::string resolution;
    switch (ticket.category) {
        case TicketCategory::Software:
            resolution = "Reconfigured application settings and applied patches.";
            break;
        case TicketCategory::Network:
            resolution = "Diagnosed network issue, updated firewall rules.";
            break;
        case TicketCategory::Hardware:
            resolution = "Ran hardware diagnostics, scheduled replacement.";
            break;
        default:
            resolution = "Performed technical investigation and applied fix.";
            break;
    }
    ticket.addLog("L2 - " + getName(), "RESOLVED: " + resolution);
}

// ============================================================
// ManagerSupport 实现 —— 处理重大问题
// ============================================================

ManagerSupport::ManagerSupport(const std::string& name)
    : SupportHandler(name, "Manager") {}

bool ManagerSupport::canHandle(const SupportTicket& ticket) const {
    // 经理处理 Major 级别的问题
    return ticket.severity == Severity::Major;
}

void ManagerSupport::processTicket(SupportTicket& ticket) {
    std::string resolution;
    switch (ticket.category) {
        case TicketCategory::Security:
            resolution = "Initiated security audit, patched vulnerability, notified stakeholders.";
            break;
        case TicketCategory::DataRecovery:
            resolution = "Coordinated with DBA team, recovered data from backup.";
            break;
        default:
            resolution = "Assembled cross-team task force, implemented hotfix.";
            break;
    }
    ticket.addLog("Manager - " + getName(), "RESOLVED: " + resolution);
}

// ============================================================
// DirectorSupport 实现 —— 最高级别兜底处理
// ============================================================

DirectorSupport::DirectorSupport(const std::string& name)
    : SupportHandler(name, "Director") {}

bool DirectorSupport::canHandle(const SupportTicket& ticket) const {
    // 总监处理所有到达此级别的问题（兜底）
    // 设计要点：责任链的最后一环通常是"万能处理者"，确保请求不会丢失
    return true;
}

void DirectorSupport::processTicket(SupportTicket& ticket) {
    std::string resolution;
    if (ticket.severity == Severity::Critical) {
        switch (ticket.category) {
            case TicketCategory::Security:
                resolution = "Activated incident response team. Full system lockdown "
                             "and forensic analysis initiated. Board notified.";
                break;
            case TicketCategory::Network:
                resolution = "Engaged ISP and infrastructure team. Activated DR site. "
                             "Customer communication sent.";
                break;
            default:
                resolution = "Declared P1 incident. All hands on deck. "
                             "Hourly status updates to executive team.";
                break;
        }
    } else {
        resolution = "Executive review completed. Assigned dedicated resources.";
    }
    ticket.addLog("Director - " + getName(), "RESOLVED: " + resolution);
}

// ============================================================
// SupportSystem 实现 —— 构建并管理责任链
// ============================================================

SupportSystem::SupportSystem() {
    // 构建责任链：L1 -> L2 -> Manager -> Director
    auto l1 = std::make_shared<Level1Support>("Alice");
    auto l2 = std::make_shared<Level2Support>("Bob");
    auto mgr = std::make_shared<ManagerSupport>("Charlie");
    auto dir = std::make_shared<DirectorSupport>("Diana");

    // 链式设置（setNext 返回下一个处理者的引用）
    l1->setNext(l2);
    l2->setNext(mgr);
    mgr->setNext(dir);

    chainHead_ = l1;
    handlers_ = {l1, l2, mgr, dir};
}

void SupportSystem::submitTicket(SupportTicket& ticket) {
    std::cout << "  Submitting ticket #" << ticket.id
              << " [" << ticket.getSeverityString() << " / "
              << ticket.getCategoryString() << "]" << std::endl;
    std::cout << "  Customer: " << ticket.customerName << std::endl;
    std::cout << "  Issue: " << ticket.description << std::endl;
    std::cout << std::endl;

    // 从链头开始处理
    chainHead_->handle(ticket);

    // 打印处理日志
    std::cout << "  Processing log:" << std::endl;
    for (const auto& log : ticket.handlingLog) {
        std::cout << "    -> " << log << std::endl;
    }
    std::cout << std::endl;
}

void SupportSystem::printChain() const {
    std::cout << "  Support chain: ";
    auto current = chainHead_;
    while (current) {
        std::cout << current->getLevel() << "(" << current->getName() << ")";
        current = current->getNext();
        if (current) std::cout << " -> ";
    }
    std::cout << std::endl;
}

// ============================================================
// main() —— 演示责任链模式
// ============================================================
int main() {
    std::cout << "================================================" << std::endl;
    std::cout << "   Chain of Responsibility - Support Ticket System" << std::endl;
    std::cout << "================================================" << std::endl;

    SupportSystem system;

    // 显示责任链结构
    std::cout << "\n[0] Support Chain Structure:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    system.printChain();

    // --- 工单 1：Minor 级别，L1 直接处理 ---
    std::cout << "\n[1] Minor Ticket - Password Reset:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    SupportTicket ticket1(1001, "John Smith",
        "Cannot log in, forgot password",
        Severity::Minor, TicketCategory::Account);
    system.submitTicket(ticket1);

    // --- 工单 2：Moderate 级别，L1 无法处理，转发给 L2 ---
    std::cout << "[2] Moderate Ticket - Software Configuration:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    SupportTicket ticket2(1002, "Jane Doe",
        "ERP module showing incorrect currency conversion rates",
        Severity::Moderate, TicketCategory::Software);
    system.submitTicket(ticket2);

    // --- 工单 3：Major 级别，经过 L1、L2，到达 Manager ---
    std::cout << "[3] Major Ticket - Data Recovery:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    SupportTicket ticket3(1003, "Acme Corp",
        "Accidentally deleted 3 months of transaction records",
        Severity::Major, TicketCategory::DataRecovery);
    system.submitTicket(ticket3);

    // --- 工单 4：Critical 级别，直通 Director ---
    std::cout << "[4] Critical Ticket - Production Down:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    SupportTicket ticket4(1004, "MegaTech Inc",
        "Production cluster completely unresponsive, 10K users affected",
        Severity::Critical, TicketCategory::Network);
    system.submitTicket(ticket4);

    // --- 工单 5：Critical 安全事件 ---
    std::cout << "[5] Critical Ticket - Security Breach:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    SupportTicket ticket5(1005, "SecureBank Ltd",
        "Unauthorized access detected on admin portal, potential data breach",
        Severity::Critical, TicketCategory::Security);
    system.submitTicket(ticket5);

    // --- 工单 6：Minor 级别的软件问题 ---
    std::cout << "[6] Minor Ticket - Basic Software Issue:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    SupportTicket ticket6(1006, "StartupXYZ",
        "How to export report as PDF",
        Severity::Minor, TicketCategory::Software);
    system.submitTicket(ticket6);

    // --- 统计汇总 ---
    std::cout << "================================================" << std::endl;
    std::cout << "   Ticket Processing Summary" << std::endl;
    std::cout << "================================================" << std::endl;

    struct TicketSummary {
        int id;
        std::string severity;
        std::string resolvedBy;
        int stepsToResolve;
    };

    std::vector<SupportTicket*> allTickets = {
        &ticket1, &ticket2, &ticket3, &ticket4, &ticket5, &ticket6
    };

    std::cout << std::left
              << std::setw(8) << "  ID"
              << std::setw(12) << "Severity"
              << std::setw(14) << "Category"
              << std::setw(8) << "Steps"
              << "Resolved By" << std::endl;
    std::cout << "  " << std::string(60, '-') << std::endl;

    for (auto* t : allTickets) {
        // 找到最终处理者（最后一条 RESOLVED 日志）
        std::string resolvedBy = "N/A";
        for (const auto& log : t->handlingLog) {
            if (log.find("RESOLVED") != std::string::npos) {
                resolvedBy = log.substr(1, log.find(']') - 1);
            }
        }

        std::cout << "  " << std::left
                  << std::setw(8) << t->id
                  << std::setw(12) << t->getSeverityString()
                  << std::setw(14) << t->getCategoryString()
                  << std::setw(8) << t->handlingLog.size()
                  << resolvedBy << std::endl;
    }

    std::cout << "\n================================================" << std::endl;
    std::cout << "   Chain of Responsibility Demo Complete" << std::endl;
    std::cout << "================================================" << std::endl;

    return 0;
}
