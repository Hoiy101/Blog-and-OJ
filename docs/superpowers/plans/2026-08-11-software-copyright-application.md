# Blog and OJ V1.0 Software Copyright Application Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 生成一套个人申请人补齐身份信息后即可用于 Blog and OJ V1.0 软件著作权登记的 DOCX、PDF和核查清单。

**Architecture:** 使用一个结构化元数据文件作为所有材料的事实来源，以独立脚本完成仓库代码盘点、敏感信息过滤和60页代码鉴别材料生成。说明书、申请信息表和提交指南采用 DOCX 模板化生成，再统一渲染为 PDF/PNG进行视觉核验。

**Tech Stack:** Python 3、python-docx、Pygments、LibreOffice、Poppler、现有 Vue 3/Spring Boot 源码、Codex 文档与 PDF 工作区运行时。

## Global Constraints

- 软件全称统一为 `Blog and OJ`，版本号统一为 `V1.0`。
- 自然人申请、独立开发、原始取得；个人身份信息只保留空白字段。
- 开发完成日期为2026年8月10日，首次发表日期为2026年2月23日。
- 首次发表方式为 GitHub 网络发布，地址为 `https://github.com/Hoiy101/Blog-and-OJ`，地点为中国。
- 不写入密码、访问密钥、令牌、个人证件号码或真实线上连接信息。
- 源程序鉴别材料使用普通交存方式，共60页，每页至少50个实际代码行。
- 不修改现有业务代码，不覆盖用户未提交的 `GetServiceImpl.java` 修改。

---

## File Map

- Create: `copyright-application/application-data.yaml` — 全部非敏感登记事实的唯一来源。
- Create: `copyright-application/scripts/inventory_source.py` — 盘点自有源码、行数和敏感模式。
- Create: `copyright-application/scripts/build_application_package.py` — 生成信息表、说明书、指南和待填清单。
- Create: `copyright-application/scripts/build_source_deposit.py` — 选择连续源码并生成60页鉴别材料。
- Create: `copyright-application/scripts/verify_package.py` — 校验字段一致性、页数、代码行数和敏感信息。
- Create: `copyright-application/01-软件著作权登记信息表.docx` — 申请字段草稿。
- Create: `copyright-application/02-Blog and OJ V1.0软件操作说明书.docx` — 操作说明书源文件。
- Create: `copyright-application/02-Blog and OJ V1.0软件操作说明书.pdf` — 操作说明书提交预览版。
- Create: `copyright-application/03-Blog and OJ V1.0源程序鉴别材料.docx` — 源程序材料源文件。
- Create: `copyright-application/03-Blog and OJ V1.0源程序鉴别材料.pdf` — 源程序材料提交预览版。
- Create: `copyright-application/04-申请材料清单与提交指南.docx` — 提交说明。
- Create: `copyright-application/05-待填写信息清单.md` — 个人线下补填项。
- Create: `copyright-application/qa/` — PDF页面渲染图、源文件清单和验证报告。

### Task 1: 建立统一登记数据和源码清单

**Files:**
- Create: `copyright-application/application-data.yaml`
- Create: `copyright-application/scripts/inventory_source.py`
- Create: `copyright-application/qa/source-inventory.json`

**Interfaces:**
- Consumes: 当前仓库中的 `web/src`、`backendcloud/backend/src/main/java`、`backendcloud/evaluatesystem/src/main/java`。
- Produces: UTF-8 YAML登记数据；JSON源码清单，每项包含 `path`、`language`、`line_count`、`sha256`、`sensitive_matches`。

- [ ] **Step 1: 写入统一登记数据**

  `application-data.yaml` 明确写入已确认字段，并将 `applicant.name`、`id_number`、`address`、`postal_code`、`phone`、`email` 设为空字符串；功能模块按 README 和源码目录列出。

- [ ] **Step 2: 实现源码盘点脚本**

  使用 `pathlib.Path.rglob` 仅收集 `.java`、`.js`、`.mjs`、`.vue` 文件，排除 `node_modules`、`target`、`dist`、测试快照和配置文件；对 `password`、`secret`、`access_key`、`token`、JDBC URL 和私钥头执行不区分大小写扫描。

- [ ] **Step 3: 生成并检查源码清单**

  Run: `python3 copyright-application/scripts/inventory_source.py`

  Expected: 退出码0，生成 `qa/source-inventory.json`；每个路径位于允许目录内，敏感匹配项明确列出而不输出匹配值。

### Task 2: 生成申请信息表、清单和待填写项

**Files:**
- Create: `copyright-application/scripts/build_application_package.py`
- Create: `copyright-application/01-软件著作权登记信息表.docx`
- Create: `copyright-application/04-申请材料清单与提交指南.docx`
- Create: `copyright-application/05-待填写信息清单.md`

**Interfaces:**
- Consumes: `application-data.yaml`。
- Produces: 三份内容一致、无个人隐私的申请辅助材料。

- [ ] **Step 1: 实现公共文档样式**

  在生成脚本中定义 A4 页边距、中文正文和标题字体、表格边框、页眉页脚、段前段后距以及空白字段渲染规则；空字段统一显示 `________________`。

- [ ] **Step 2: 生成登记信息表**

  按“软件基本信息、开发信息、发表信息、著作权人信息、权利范围、软件技术特点”组织字段；软件用途描述为个人内容创作与在线编程练习的综合平台。

- [ ] **Step 3: 生成提交指南和待填清单**

  指南写明自然人身份证明、在线填报、签字/确认、鉴别材料上传和补正提醒；待填清单只记录字段名和填写说明，不记录真实值。

- [ ] **Step 4: 执行生成器**

  Run: `python3 copyright-application/scripts/build_application_package.py --documents basic`

  Expected: 生成 `01`、`04`、`05` 三个文件，且全文检索不到空字段以外的身份证号或手机号。

### Task 3: 获取真实界面并生成软件操作说明书

**Files:**
- Create: `copyright-application/assets/screenshots/*.png`
- Modify: `copyright-application/scripts/build_application_package.py`
- Create: `copyright-application/02-Blog and OJ V1.0软件操作说明书.docx`

**Interfaces:**
- Consumes: 当前可运行的网站页面；无法运行时消费仓库内可验证的页面源代码和现有图片资源。
- Produces: 带图注的真实界面截图和完整操作说明书。

- [ ] **Step 1: 按文档技能加载运行时并检查服务可用性**

  调用工作区依赖加载器获取 Python、LibreOffice、Poppler 和文档脚本路径；检查 `http://localhost:8080` 与后端服务。不得为截图修改业务功能或连接生产数据。

- [ ] **Step 2: 采集可用的真实界面**

  优先采集登录注册、博客列表、博客编辑、题目列表、题目详情、代码编辑、提交记录和后台管理页面。每张截图裁去浏览器隐私信息并记录对应功能；无法访问的页面不制作伪截图。

- [ ] **Step 3: 生成说明书正文**

  章节固定为封面、修订记录、目录、软件概述、运行环境、总体结构、安装与启动、用户认证、博客管理、在线判题、记录查询、后台管理、异常提示和退出。每项功能包含入口、前置条件、操作步骤和预期结果。

- [ ] **Step 4: 生成 DOCX**

  Run: `python3 copyright-application/scripts/build_application_package.py --documents manual`

  Expected: 说明书不少于15页；不存在“待补截图”等内部标记；若截图不足，以真实文字说明补足，不虚构界面。

### Task 4: 生成60页源程序鉴别材料

**Files:**
- Create: `copyright-application/scripts/build_source_deposit.py`
- Create: `copyright-application/qa/source-selection.json`
- Create: `copyright-application/03-Blog and OJ V1.0源程序鉴别材料.docx`

**Interfaces:**
- Consumes: `qa/source-inventory.json` 中无敏感匹配的自有源码。
- Produces: 记录文件边界、行号和哈希的选择清单，以及60页源程序DOCX。

- [ ] **Step 1: 选择前后各30页代码**

  前段优先覆盖前端入口、路由、认证状态、博客和OJ页面；后段优先覆盖后端认证、博客、题库、消息队列、WebSocket和Docker判题。文件之间允许以路径标题分隔，但文件内部保持原始行序，不抽取零散片段。

- [ ] **Step 2: 实现确定性分页**

  每页输出50个实际源程序行，行号独立成列，页眉为 `Blog and OJ V1.0 源程序`，页脚为当前页码；路径标题不计入50行。总计恰好3000个代码行、60页。

- [ ] **Step 3: 生成鉴别材料**

  Run: `python3 copyright-application/scripts/build_source_deposit.py`

  Expected: 退出码0；`source-selection.json` 记录3000行来源；DOCX恰好60页或在渲染后通过字体/行距微调至60页。

### Task 5: 渲染、视觉核验和最终验证

**Files:**
- Create: `copyright-application/scripts/verify_package.py`
- Create: `copyright-application/02-Blog and OJ V1.0软件操作说明书.pdf`
- Create: `copyright-application/03-Blog and OJ V1.0源程序鉴别材料.pdf`
- Create: `copyright-application/qa/rendered-manual/*.png`
- Create: `copyright-application/qa/rendered-source/*.png`
- Create: `copyright-application/qa/verification-report.md`

**Interfaces:**
- Consumes: 全部 DOCX、YAML、JSON和截图。
- Produces: PDF、逐页PNG和机器可读/人工可读验证结果。

- [ ] **Step 1: 使用文档技能规定的渲染脚本生成PDF和页面图**

  对每份 DOCX 执行 LibreOffice 转换和 `render_docx.py` 页面渲染。源程序PDF必须60页；说明书逐页检查表格、图片、目录、页眉和页脚。

- [ ] **Step 2: 实现最终验证脚本**

  校验所有必需文件存在、统一字段在文本提取结果中一致、源程序PDF页数为60、选择清单每页50行、页码连续、截图分辨率可读，并扫描身份证号、手机号、密码、密钥、令牌和私钥模式。

- [ ] **Step 3: 执行验证**

  Run: `python3 copyright-application/scripts/verify_package.py`

  Expected: 输出 `PASS`，报告中所有检查为通过；任何敏感模式命中只报告文件和类型，不打印秘密内容。

- [ ] **Step 4: 人工视觉抽查**

  检查说明书全部页面以及源程序第1、2、29、30、31、32、59、60页；发现溢出、乱码或不足50行时调整生成参数并重新执行步骤1至3。

- [ ] **Step 5: 检查工作区改动范围**

  Run: `git status --short && git diff --check -- copyright-application docs/superpowers/plans/2026-08-11-software-copyright-application.md`

  Expected: 业务代码没有新增修改；用户原有 `GetServiceImpl.java` 修改仍保持未暂存；新文件仅位于 `copyright-application/` 和本计划路径。
