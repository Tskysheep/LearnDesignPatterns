---
sidebar_position: 2
title: 推荐学习路线
description: 按难度递进的设计模式学习路线
---

# 推荐学习路线

## 入门路线（先学这 5 个）

1. **[Strategy](./component-collaboration/strategy/)** — 最直观理解"封装变化"
2. **[Observer](./component-collaboration/observer/)** — 理解松耦合通知机制
3. **[Factory Method](./object-creation/factory-method/)** — 理解对象创建的多态
4. **[Singleton](./object-performance/singleton/)** — 最简单但也最容易误用
5. **[Decorator](./single-responsibility/decorator/)** — 理解组合优于继承

## 进阶路线

6. [Template Method](./component-collaboration/template-method/)
7. [Bridge](./single-responsibility/bridge/)
8. [Abstract Factory](./object-creation/abstract-factory/)
9. [Builder](./object-creation/builder/)
10. [Adapter](./interface-isolation/adapter/)

## 高级路线

11. [Composite](./data-structure/composite/)
12. [Command](./behavioral-change/command/)
13. [State](./state-change/state/)
14. [Proxy](./interface-isolation/proxy/)
15. [Facade](./interface-isolation/facade/)

## 深入路线

16. [Chain of Responsibility](./data-structure/chain-of-responsibility/)
17. [Mediator](./interface-isolation/mediator/)
18. [Flyweight](./object-performance/flyweight/)
19. [Memento](./state-change/memento/)
20. [Visitor](./behavioral-change/visitor/)
21. [Prototype](./object-creation/prototype/)
22. [Iterator](./data-structure/iterator/)
23. [Interpreter](./domain-problem/interpreter/)

## 学习建议

:::tip 学习方法
- **先理解问题，再学模式**：每个模式的文档都从"问题背景"开始，先想想你会怎么解决
- **动手编译运行**：每个示例都是独立可执行的，编译运行后对照代码理解
- **对比相似模式**：Strategy vs State、Decorator vs Proxy、Factory Method vs Abstract Factory
- **阅读开源代码**：结合文档中的开源项目应用，去真实项目中寻找模式的影子
:::

## 开源项目中的设计模式

| 模式 | Qt | Boost | STL/标准库 | LLVM |
|------|-----|-------|-----------|------|
| Template Method | QWidget::paintEvent | — | — | Pass::runOnFunction |
| Strategy | QSortFilterProxyModel | boost::function | std::sort 比较器 | — |
| Observer | 信号与槽 | Boost.Signals2 | — | — |
| Decorator | QScrollArea | boost::iostreams | — | — |
| Bridge | QPlatformWindow | Boost.Pimpl | — | — |
| Factory Method | QStyleFactory::create | — | — | llvm::PassRegistry |
| Abstract Factory | QStyleFactory | — | — | — |
| Prototype | QGraphicsItem::clone | — | — | — |
| Builder | — | Boost.Spirit | — | IRBuilder |
| Singleton | QApplication | boost::serialization | — | ManagedStatic |
| Flyweight | QSharedData | Boost.Flyweight | — | StringRef/Twine |
| Facade | QFileDialog | boost::filesystem | std::filesystem | — |
| Proxy | QNetworkProxy | shared_ptr | std::shared_ptr | — |
| Mediator | QDialogButtonBox | — | — | — |
| Adapter | QAbstractItemModel | boost::adaptors | std::stack/queue | — |
| Memento | QUndoCommand | Boost.Serialization | — | — |
| State | QStateMachine | Boost.MSM/SML | — | — |
| Composite | QObject 树 | — | — | LLVM IR (Value) |
| Iterator | QListIterator | Boost.Range | std::iterator | — |
| Chain of Resp. | QEvent 事件传播 | — | — | PassManager |
| Command | QUndoCommand | — | — | — |
| Visitor | — | Boost.Variant | std::visit | InstVisitor |
| Interpreter | — | Boost.Spirit | — | — |
