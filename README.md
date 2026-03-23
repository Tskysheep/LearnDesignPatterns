# LearnDesignPatterns

C++ 设计模式教学示例 — GoF 23 种设计模式，基于"封装变化"分类法。

## 在线浏览

**[>>>>>>>>>在线浏览<<<<<<<<<](https://tskysheep.github.io/LearnDesignPatterns/)**

## 项目简介

23 个独立可编译的 C++17 示例，采用李建忠老师的"封装变化"视角对设计模式进行分类，每个模式包含完整的教学文档（Mermaid 类图 / 时序图）、可运行的示例代码，以及在开源项目中的真实应用。

| 分类 | 模式数 | 包含模式 |
|------|:------:|----------|
| 组件协作 | 3 | Template Method、Strategy、Observer |
| 单一职责 | 2 | Decorator、Bridge |
| 对象创建 | 4 | Factory Method、Abstract Factory、Prototype、Builder |
| 对象性能 | 2 | Singleton、Flyweight |
| 接口隔离 | 4 | Facade、Proxy、Mediator、Adapter |
| 状态变化 | 2 | Memento、State |
| 数据结构 | 3 | Composite、Iterator、Chain of Responsibility |
| 行为变化 | 2 | Command、Visitor |
| 领域问题 | 1 | Interpreter |

## 构建

```bash
cmake -B build
cmake --build build
```

运行单个模式：

```bash
./build/01_ComponentCollaboration/Strategy/Pattern_Strategy
```

## 项目结构

```
01_ComponentCollaboration/   # 组件协作 (3 个模式)
02_SingleResponsibility/     # 单一职责 (2 个模式)
03_ObjectCreation/           # 对象创建 (4 个模式)
04_ObjectPerformance/        # 对象性能 (2 个模式)
05_InterfaceIsolation/       # 接口隔离 (4 个模式)
06_StateChange/              # 状态变化 (2 个模式)
07_DataStructure/            # 数据结构 (3 个模式)
08_BehavioralChange/         # 行为变化 (2 个模式)
09_DomainProblem/            # 领域问题 (1 个模式)
website/                     # Docusaurus 文档站点
```

每个模式目录包含：
- `*.h` — 类声明
- `*.cpp` — 实现 + main() 演示
- `*.md` — 教学文档（含 Mermaid 类图 / 时序图）
- `CMakeLists.txt` — 独立构建配置

## License

[MIT](LICENSE)
