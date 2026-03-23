# C++ 设计模式教学项目

## 一、设计模式概论

### 什么是设计模式

设计模式（Design Pattern）是在软件设计中反复出现的问题的**经过验证的解决方案**。它们不是可以直接复制粘贴的代码，而是描述了在特定上下文中解决通用设计问题的模板。

GoF（Gang of Four）在 1994 年出版的《Design Patterns: Elements of Reusable Object-Oriented Software》中总结了 23 种经典设计模式，至今仍是面向对象设计的基石。

### 为什么学习设计模式

1. **提升设计能力**：理解优秀软件背后的设计思想，而非仅停留在语法层面
2. **建立共同语言**：团队沟通时使用 "Observer"、"Strategy" 等术语，减少沟通成本
3. **应对变化**：设计模式的核心价值在于**封装变化**，让软件在需求变化时保持稳定
4. **理解框架**：主流框架（Qt、Boost、STL）大量使用设计模式，学习模式有助于理解框架设计

### SOLID 原则简述

设计模式是 SOLID 原则的具体实践：

| 原则 | 全称 | 核心思想 |
|------|------|----------|
| **S** | 单一职责原则 (SRP) | 一个类只应有一个引起变化的原因 |
| **O** | 开放封闭原则 (OCP) | 对扩展开放，对修改封闭 |
| **L** | 里氏替换原则 (LSP) | 子类必须能够替换其基类 |
| **I** | 接口隔离原则 (ISP) | 不应强迫客户依赖它不使用的接口 |
| **D** | 依赖倒置原则 (DIP) | 高层模块不应依赖低层模块，二者都应依赖抽象 |

---

## 二、"封装变化"分类法

本项目采用李建忠老师提出的**"封装变化"分类法**，从设计模式要封装的变化维度来组织 23 种模式。相比 GoF 原书按创建型/结构型/行为型的三分法，这种分类方式更能揭示模式解决问题的本质。

### 分类总览

| 分类 | 核心思想 | 包含模式 |
|------|----------|----------|
| **组件协作** | 封装组件之间的协作关系，通过晚绑定实现框架与应用的松耦合 | Template Method, Strategy, Observer |
| **单一职责** | 封装对象在某个维度上的变化，避免类膨胀 | Decorator, Bridge |
| **对象创建** | 封装对象创建的变化，绕开 `new` 带来的紧耦合 | Factory Method, Abstract Factory, Prototype, Builder |
| **对象性能** | 封装对象的创建成本与共享策略 | Singleton, Flyweight |
| **接口隔离** | 封装接口的变化，在不同组件之间建立间接层 | Facade, Proxy, Mediator, Adapter |
| **状态变化** | 封装对象状态的变化，在状态改变时保持结构稳定 | Memento, State |
| **数据结构** | 封装数据结构的变化，使算法与结构解耦 | Composite, Iterator, Chain of Responsibility |
| **行为变化** | 封装对象行为的变化，在运行时动态改变行为 | Command, Visitor |
| **领域问题** | 封装特定领域的变化，为特定领域提供表达力 | Interpreter |

---

## 三、23 种设计模式一览

### 1. 组件协作 (Component Collaboration)

| 模式 | 一句话描述 | 文档链接 |
|------|-----------|----------|
| Template Method | 定义算法骨架，将某些步骤延迟到子类 | [TemplateMethod.md](01_ComponentCollaboration/TemplateMethod/TemplateMethod.md) |
| Strategy | 定义一系列算法，使它们可以互相替换 | [Strategy.md](01_ComponentCollaboration/Strategy/Strategy.md) |
| Observer | 定义对象间一对多的依赖关系，状态变化时自动通知 | [Observer.md](01_ComponentCollaboration/Observer/Observer.md) |

### 2. 单一职责 (Single Responsibility)

| 模式 | 一句话描述 | 文档链接 |
|------|-----------|----------|
| Decorator | 动态地给对象添加额外职责 | [Decorator.md](02_SingleResponsibility/Decorator/Decorator.md) |
| Bridge | 将抽象部分与实现部分分离 | [Bridge.md](02_SingleResponsibility/Bridge/Bridge.md) |

### 3. 对象创建 (Object Creation)

| 模式 | 一句话描述 | 文档链接 |
|------|-----------|----------|
| Factory Method | 定义创建对象的接口，让子类决定实例化哪个类 | [FactoryMethod.md](03_ObjectCreation/FactoryMethod/FactoryMethod.md) |
| Abstract Factory | 创建一系列相关对象，无需指定具体类 | [AbstractFactory.md](03_ObjectCreation/AbstractFactory/AbstractFactory.md) |
| Prototype | 通过拷贝原型来创建新对象 | [Prototype.md](03_ObjectCreation/Prototype/Prototype.md) |
| Builder | 将复杂对象的构建与表示分离 | [Builder.md](03_ObjectCreation/Builder/Builder.md) |

### 4. 对象性能 (Object Performance)

| 模式 | 一句话描述 | 文档链接 |
|------|-----------|----------|
| Singleton | 确保一个类只有一个实例 | [Singleton.md](04_ObjectPerformance/Singleton/Singleton.md) |
| Flyweight | 运用共享技术有效支持大量细粒度对象 | [Flyweight.md](04_ObjectPerformance/Flyweight/Flyweight.md) |

### 5. 接口隔离 (Interface Isolation)

| 模式 | 一句话描述 | 文档链接 |
|------|-----------|----------|
| Facade | 为子系统提供一个统一的高层接口 | [Facade.md](05_InterfaceIsolation/Facade/Facade.md) |
| Proxy | 为其他对象提供一种代理以控制访问 | [Proxy.md](05_InterfaceIsolation/Proxy/Proxy.md) |
| Mediator | 用一个中介对象封装一系列对象交互 | [Mediator.md](05_InterfaceIsolation/Mediator/Mediator.md) |
| Adapter | 将一个类的接口转换成客户希望的另一个接口 | [Adapter.md](05_InterfaceIsolation/Adapter/Adapter.md) |

### 6. 状态变化 (State Change)

| 模式 | 一句话描述 | 文档链接 |
|------|-----------|----------|
| Memento | 在不破坏封装的前提下捕获并保存对象内部状态 | [Memento.md](06_StateChange/Memento/Memento.md) |
| State | 允许对象在内部状态改变时改变它的行为 | [State.md](06_StateChange/State/State.md) |

### 7. 数据结构 (Data Structure)

| 模式 | 一句话描述 | 文档链接 |
|------|-----------|----------|
| Composite | 将对象组合成树形结构以表示"部分-整体"层次 | [Composite.md](07_DataStructure/Composite/Composite.md) |
| Iterator | 顺序访问聚合对象中的元素而不暴露其底层表示 | [Iterator.md](07_DataStructure/Iterator/Iterator.md) |
| Chain of Responsibility | 将请求沿着处理者链传递，直到被处理 | [ChainOfResponsibility.md](07_DataStructure/ChainOfResponsibility/ChainOfResponsibility.md) |

### 8. 行为变化 (Behavioral Change)

| 模式 | 一句话描述 | 文档链接 |
|------|-----------|----------|
| Command | 将请求封装为对象，从而可以参数化、排队和撤销 | [Command.md](08_BehavioralChange/Command/Command.md) |
| Visitor | 在不修改类结构的前提下定义新操作 | [Visitor.md](08_BehavioralChange/Visitor/Visitor.md) |

### 9. 领域问题 (Domain Problem)

| 模式 | 一句话描述 | 文档链接 |
|------|-----------|----------|
| Interpreter | 给定一门语言，定义其文法表示和解释器 | [Interpreter.md](09_DomainProblem/Interpreter/Interpreter.md) |

---

## 四、开源项目中的设计模式应用

| 模式 | Qt | Boost | STL/标准库 | LLVM | 其他 |
|------|-----|-------|-----------|------|------|
| Template Method | QWidget::paintEvent | — | — | Pass::runOnFunction | gtest TEST_F |
| Strategy | QSortFilterProxyModel | boost::function | std::sort 比较器 | — | — |
| Observer | 信号与槽 (Signal/Slot) | Boost.Signals2 | — | — | RxCpp |
| Decorator | QScrollArea | boost::iostreams | — | — | Java I/O Streams |
| Bridge | QPlatformWindow | Boost.Pimpl | — | — | — |
| Factory Method | QStyleFactory::create | — | — | llvm::PassRegistry | — |
| Abstract Factory | QStyleFactory | — | — | — | — |
| Prototype | QGraphicsItem::clone | — | — | — | — |
| Builder | — | Boost.Spirit | — | IRBuilder | — |
| Singleton | QApplication | boost::serialization::singleton | — | ManagedStatic | — |
| Flyweight | 隐式共享 (QSharedData) | Boost.Flyweight | — | StringRef/Twine | — |
| Facade | QFileDialog | boost::filesystem | std::filesystem | — | — |
| Proxy | QNetworkProxy | shared_ptr (引用计数) | std::shared_ptr | — | — |
| Mediator | QDialogButtonBox | — | — | — | — |
| Adapter | QAbstractItemModel | boost::adaptors | std::stack/queue | — | — |
| Memento | QUndoCommand | Boost.Serialization | — | — | — |
| State | QStateMachine | Boost.MSM/SML | — | — | — |
| Composite | QObject 树 | — | — | LLVM IR (Value) | — |
| Iterator | QListIterator | Boost.Range | std::iterator | LLVM iterator | — |
| Chain of Resp. | QEvent 事件传播 | — | — | PassManager | — |
| Command | QUndoCommand | — | — | — | — |
| Visitor | — | Boost.Variant (visitor) | std::visit | LLVM InstVisitor | — |
| Interpreter | — | Boost.Spirit | — | — | ANTLR |

---

## 五、推荐学习路线

### 入门路线（先学这 5 个）
1. **Strategy** — 最直观理解"封装变化"
2. **Observer** — 理解松耦合通知机制
3. **Factory Method** — 理解对象创建的多态
4. **Singleton** — 最简单但也最容易误用
5. **Decorator** — 理解组合优于继承

### 进阶路线
6. Template Method → 7. Bridge → 8. Abstract Factory → 9. Builder → 10. Adapter

### 高级路线
11. Composite → 12. Command → 13. State → 14. Proxy → 15. Facade

### 深入路线
16. Chain of Responsibility → 17. Mediator → 18. Flyweight → 19. Memento → 20. Visitor → 21. Prototype → 22. Iterator → 23. Interpreter

### 学习建议
- **先理解问题，再学模式**：每个模式的文档都从"问题背景"开始，先想想你会怎么解决
- **动手编译运行**：每个示例都是独立可执行的，编译运行后对照代码理解
- **对比相似模式**：Strategy vs State、Decorator vs Proxy、Factory Method vs Abstract Factory
- **阅读开源代码**：结合上面的表格，去真实项目中寻找模式的影子

---

## 六、构建与运行

```bash
# 构建全部
mkdir build && cd build
cmake ..
cmake --build .

# 运行某个模式的示例
./Pattern_Strategy
./Pattern_Observer
# ... 其他模式同理
```

### 环境要求
- CMake >= 3.16
- 支持 C++17 的编译器（GCC 7+, Clang 5+, MSVC 2017+）
