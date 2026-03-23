---
sidebar_position: 1
title: 设计模式概论
description: 什么是设计模式、为什么学习、SOLID 原则
---

# C++ 设计模式教学

## 什么是设计模式

设计模式（Design Pattern）是在软件设计中反复出现的问题的**经过验证的解决方案**。它们不是可以直接复制粘贴的代码，而是描述了在特定上下文中解决通用设计问题的模板。

GoF（Gang of Four）在 1994 年出版的《Design Patterns: Elements of Reusable Object-Oriented Software》中总结了 23 种经典设计模式，至今仍是面向对象设计的基石。

## 为什么学习设计模式

1. **提升设计能力**：理解优秀软件背后的设计思想，而非仅停留在语法层面
2. **建立共同语言**：团队沟通时使用 "Observer"、"Strategy" 等术语，减少沟通成本
3. **应对变化**：设计模式的核心价值在于**封装变化**，让软件在需求变化时保持稳定
4. **理解框架**：主流框架（Qt、Boost、STL）大量使用设计模式，学习模式有助于理解框架设计

## SOLID 原则

设计模式是 SOLID 原则的具体实践：

| 原则 | 全称 | 核心思想 |
|------|------|----------|
| **S** | 单一职责原则 (SRP) | 一个类只应有一个引起变化的原因 |
| **O** | 开放封闭原则 (OCP) | 对扩展开放，对修改封闭 |
| **L** | 里氏替换原则 (LSP) | 子类必须能够替换其基类 |
| **I** | 接口隔离原则 (ISP) | 不应强迫客户依赖它不使用的接口 |
| **D** | 依赖倒置原则 (DIP) | 高层模块不应依赖低层模块，二者都应依赖抽象 |

## "封装变化"分类法

本项目采用李建忠老师提出的**"封装变化"分类法**，从设计模式要封装的变化维度来组织 23 种模式。

:::info 为什么用这种分类法
相比 GoF 原书按创建型/结构型/行为型的三分法，"封装变化"分类方式更能揭示模式解决问题的本质——每个模式都在封装某个维度的变化。
:::

| 分类 | 核心思想 | 包含模式 |
|------|----------|----------|
| [组件协作](/docs/category/组件协作) | 封装组件之间的协作关系 | Template Method, Strategy, Observer |
| [单一职责](/docs/category/单一职责) | 封装对象在某个维度上的变化 | Decorator, Bridge |
| [对象创建](/docs/category/对象创建) | 封装对象创建的变化 | Factory Method, Abstract Factory, Prototype, Builder |
| [对象性能](/docs/category/对象性能) | 封装对象的创建成本与共享策略 | Singleton, Flyweight |
| [接口隔离](/docs/category/接口隔离) | 封装接口的变化 | Facade, Proxy, Mediator, Adapter |
| [状态变化](/docs/category/状态变化) | 封装对象状态的变化 | Memento, State |
| [数据结构](/docs/category/数据结构) | 封装数据结构的变化 | Composite, Iterator, Chain of Responsibility |
| [行为变化](/docs/category/行为变化) | 封装对象行为的变化 | Command, Visitor |
| [领域问题](/docs/category/领域问题) | 封装特定领域的变化 | Interpreter |

## 构建与运行

```bash
# 构建全部
mkdir build && cd build
cmake ..
cmake --build .

# 运行某个模式的示例
./Pattern_Strategy
./Pattern_Observer
```

:::tip 环境要求
- CMake >= 3.16
- 支持 C++17 的编译器（GCC 7+, Clang 5+, MSVC 2017+）
:::
