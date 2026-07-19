# Blog and OJ

一个将个人博客与在线判题（Online Judge，OJ）结合在一起的前后端分离项目。用户可以注册、登录、发布和编辑博客，也可以浏览题目、提交代码，并实时查看判题结果与历史提交记录。

## 项目简介

项目由 Vue 前端和两个 Spring Boot 后端服务组成：

- `web`：提供登录注册、博客管理、题目浏览、代码编辑与提交、判题记录等页面。
- `backend`：负责用户认证、博客与题目数据、提交记录、WebSocket 推送等核心业务。
- `evaluatesystem`：从 RabbitMQ 接收判题任务，通过 Docker 容器隔离运行用户代码，再将结果返回给主服务。

主要功能包括：

- 用户注册、登录、JWT 身份认证与头像上传
- 博客的创建、编辑、删除、浏览与图片上传
- Markdown 内容展示与代码编辑
- OJ 题目列表、题目详情和测试用例管理
- Java 代码在线提交与 Docker 沙箱判题
- RabbitMQ 异步分发判题任务
- WebSocket 实时推送判题结果
- 个人提交记录与答题详情查询

## 项目结构

```text
Blog-and-OJ/
├── backendcloud/                         # Maven 后端聚合工程
│   ├── backend/                          # 主业务服务（端口 3000）
│   │   ├── src/
│   │   │   ├── main/
│   │   │   │   ├── java/com/kob/backend/
│   │   │   │   │   ├── aspect/          # AOP 日志
│   │   │   │   │   ├── config/          # Security、CORS、WebSocket、RabbitMQ 配置
│   │   │   │   │   ├── consumer/        # WebSocket 服务
│   │   │   │   │   ├── controller/
│   │   │   │   │   │   ├── oj/          # 题目、答案、记录、判题接口
│   │   │   │   │   │   └── user/        # 用户账号与博客接口
│   │   │   │   │   ├── mapper/          # MyBatis-Plus Mapper
│   │   │   │   │   ├── pojo/            # 数据实体
│   │   │   │   │   ├── producer/        # RabbitMQ 判题任务生产者
│   │   │   │   │   ├── service/
│   │   │   │   │   │   ├── impl/        # 业务实现
│   │   │   │   │   │   ├── oj/          # OJ 业务接口
│   │   │   │   │   │   └── user/        # 用户与博客业务接口
│   │   │   │   │   ├── utils/            # JWT 等工具类
│   │   │   │   │   └── BackendApplication.java
│   │   │   │   └── resources/
│   │   │   │       ├── static/           # 后端静态资源
│   │   │   │       ├── templates/        # Thymeleaf 模板
│   │   │   │       └── application.properties
│   │   │   └── test/                     # 主业务服务测试
│   │   └── pom.xml
│   ├── evaluatesystem/                    # 独立判题服务（端口 3001）
│   │   ├── src/main/
│   │   │   ├── java/com/evaluatesystem/
│   │   │   │   ├── config/               # RabbitMQ 配置
│   │   │   │   ├── service/
│   │   │   │   │   └── utils/            # 判题队列、Docker 执行与进程工具
│   │   │   │   └── EvaluateSystemApplication.java
│   │   │   └── resources/
│   │   │       └── application.properties
│   │   └── pom.xml
│   └── pom.xml                            # 后端父级 Maven 配置
├── web/                                   # Vue 3 前端项目
│   ├── public/                            # HTML 模板与站点图标
│   ├── src/
│   │   ├── api/                           # 博客图片接口
│   │   ├── assets/
│   │   │   ├── images/                    # 页面图片资源
│   │   ├── components/                    # 导航栏、编辑器、判题弹窗等组件
│   │   ├── router/                        # Vue Router 路由
│   │   ├── store/                         # Vuex 状态管理
│   │   ├── utils/                         # Markdown、图片与判题工具
│   │   ├── views/
│   │   │   ├── answer/                    # 答案详情页面
│   │   │   ├── error/                     # 404 页面
│   │   │   ├── pk/                        # 博客首页
│   │   │   ├── ranklist/                  # 题目列表与详情页面
│   │   │   ├── record/                    # 提交记录页面
│   │   │   └── user/                      # 登录、注册、博客管理与用户设置
│   │   ├── App.vue
│   │   └── main.js                        # 前端入口
│   ├── tests/unit/                        # 前端单元测试
│   ├── package.json                       # npm 依赖与脚本
│   └── vue.config.js                      # Vue CLI 配置
├── docs/superpowers/                      # 设计说明与实施计划
└── README.md
```

## 技术栈

### 前端

| 技术 | 用途 |
| --- | --- |
| Vue 3 | 构建前端页面与组件 |
| Vue CLI 5 | 开发、构建与工程管理 |
| Vue Router 4 | 前端路由管理 |
| Vuex 4 | 用户与题目等全局状态管理 |
| Bootstrap 5 | 页面布局与样式 |
| jQuery | 部分 HTTP 请求与页面交互 |
| Ace Editor | 在线代码编辑器 |
| Node.js Test Runner | 前端单元测试 |

### 后端与基础设施

| 技术 | 用途 |
| --- | --- |
| Java 8 | 后端开发与用户代码判题版本 |
| Spring Boot 2.6.13 | 构建主业务服务和判题服务 |
| Spring Security + JWT | 登录认证与接口鉴权 |
| MyBatis-Plus | 数据访问与 CRUD |
| MySQL 8 | 用户、博客、题目和提交记录存储 |
| RabbitMQ | 异步传递判题任务与判题结果 |
| WebSocket | 向前端实时推送判题结果 |
| Docker / docker-java | 隔离编译和运行用户代码 |
| MinIO | 头像与博客图片对象存储 |
| Maven | Java 依赖管理与项目构建 |

## 运行环境

启动前建议准备以下环境：

- Node.js 16 或更高版本、npm 8 或更高版本
- JDK 8
- Maven 3.8 或更高版本
- MySQL 8
- RabbitMQ
- Docker Engine 或 Docker Desktop
- MinIO（需要使用头像和博客图片上传功能时）


## 后端启动方法

先启动 Docker，然后分别打开两个终端启动主业务服务和判题服务。

### 方法一：使用 IntelliJ IDEA

1. 使用 IDEA 打开 `backendcloud` 目录，等待 Maven 依赖加载完成。
2. 运行主业务入口：

   ```text
   backendcloud/backend/src/main/java/com/kob/backend/BackendApplication.java
   ```

3. 运行判题服务入口：

   ```text
   backendcloud/evaluatesystem/src/main/java/com/evaluatesystem/EvaluateSystemApplication.java
   ```

### 方法二：使用 Maven 命令行

在项目根目录执行：

```bash
# 终端 1：启动主业务服务（http://127.0.0.1:3000）
mvn -f backendcloud/backend/pom.xml \
  -Dspring-boot.run.skip=false \
  -Dspring-boot.run.main-class=com.kob.backend.BackendApplication \
  spring-boot:run
```

```bash
# 终端 2：启动判题服务（http://127.0.0.1:3001）
mvn -f backendcloud/evaluatesystem/pom.xml \
  -Dspring-boot.run.skip=false \
  -Dspring-boot.run.main-class=com.evaluatesystem.EvaluateSystemApplication \
  spring-boot:run
```

这里显式指定了 `skip=false` 和启动类，以覆盖父级 `pom.xml` 中 Spring Boot Maven 插件的现有配置。

## 前端启动方法

确认主业务服务已在 `127.0.0.1:3000` 启动，然后在新的终端中执行：

```bash
cd web
npm install
npm run serve
```

启动成功后访问：

```text
http://localhost:8080
```

前端 API 和 WebSocket 地址为 `127.0.0.1:3000`，后端允许的前端来源为 `http://localhost:8080`。

## 常用命令

在 `web` 目录下执行：

```bash
# 启动开发服务器
npm run serve

# 运行前端单元测试
npm run test:unit

# 检查代码规范
npm run lint

# 构建生产环境文件
npm run build
```

在项目根目录执行后端测试：

```bash
mvn -f backendcloud/pom.xml test
```

## 服务端口

| 服务 | 默认地址 | 说明 |
| --- | --- | --- |
| Vue 前端 | `http://localhost:8080` | Web 页面 |
| 主业务服务 | `http://127.0.0.1:3000` | REST API 与 WebSocket |
| 判题服务 | `http://127.0.0.1:3001` | 消费并执行判题任务 |
| RabbitMQ | `localhost:5672` | 默认 AMQP 端口，可在配置中修改 |
| MySQL | `localhost:3306` | 默认数据库端口，可在配置中修改 |
| MinIO | `localhost:9000` | 常用 API 端口，以实际配置为准 |

## 启动顺序

```text
MySQL / RabbitMQ / Docker / MinIO
                ↓
主业务服务 backend（3000） + 判题服务 evaluatesystem（3001）
                ↓
Vue 前端 web（8080）
```
