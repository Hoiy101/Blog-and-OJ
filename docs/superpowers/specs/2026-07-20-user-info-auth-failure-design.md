# 用户信息请求失败后的认证清理设计

## 背景

`InfoServiceImp` 现在会在用户被封禁时返回非 `success` 的 `error_message`。前端 `getinfo` action 当前只把失败响应交给页面，没有删除本地 Token、重置 Vuex 用户状态或保证返回登录页；HTTP 失败也存在相同行为。

## 目标

- `getinfo` 的业务失败和 HTTP 失败都清除本地认证状态。
- 失败后返回登录页，并向用户展示后端错误消息或通用兜底消息。
- 保持用户信息请求成功时的状态更新和页面跳转不变。

## 设计

### 状态层

在 Vuex `getinfo` action 内定义共用失败处理函数。该函数删除 `localStorage` 中的 `jwt_token`、提交现有 `logout` mutation 清空内存中的用户和 Token 状态，然后调用页面传入的 `error` 回调。业务响应的 `error_message` 不为 `success` 以及 jQuery Ajax 的 HTTP `error` 回调都使用该函数。

### 页面层

登录页定义一个共用的用户信息失败处理函数，负责：

1. 结束 `pulling_info` 状态；
2. 优先展示 `resp.error_message` 或 `resp.responseJSON.error_message`，缺失时展示“登录状态已失效，请重新登录”；
3. 使用 `router.replace({ name: 'user_account_login' })` 返回登录页。

已有 Token 的初始化请求和登录成功后的用户信息请求都传入该函数，避免遗漏失败分支。

Store 不直接导入 Router，以避免 `router → store → router` 的循环依赖。

## 错误处理

| 场景 | 处理 |
| --- | --- |
| `getinfo` 返回非 `success` | 清除本地与 Vuex 认证状态，显示后端消息，返回登录页 |
| `getinfo` 返回 HTTP 401/403 或其他请求错误 | 清除本地与 Vuex 认证状态，优先显示响应消息，否则显示兜底消息，返回登录页 |
| `getinfo` 成功 | 保持现有更新用户信息流程 |

## 验证

- 回归测试证明业务失败和 HTTP 失败共用认证清理逻辑。
- 回归测试证明两个 `getinfo` 调用点都提供返回登录页的失败处理。
- 运行完整前端单元测试和 ESLint。

## 范围

本次不修改用户信息后端逻辑，不更改其他接口的 HTTP 错误处理，也不引入 Store 与 Router 的直接依赖。
