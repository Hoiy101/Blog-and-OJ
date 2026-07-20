# Login Error Message Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make the login page display the backend-provided login failure reason, with a safe generic fallback when no reason is available.

**Architecture:** Keep the Vuex login action unchanged because it already forwards both business-failure responses and jQuery request errors. Update the login view's error callback to normalize those two response shapes directly at the presentation boundary.

**Tech Stack:** Vue 3, Vuex 4, jQuery Ajax, Node.js built-in test runner

## Global Constraints

- Preserve the existing successful login, token persistence, and navigation flow.
- Do not change backend login logic or introduce a frontend error-code mapping.
- Use `登录失败，请稍后重试` when neither response shape contains a non-empty `error_message`.

---

### Task 1: Display backend login failure messages

**Files:**
- Create: `web/tests/unit/LoginErrorMessage.test.mjs`
- Modify: `web/src/views/user/bot/account/UserAccountLoginView.vue:68-70`

**Interfaces:**
- Consumes: Vuex `login` action failure callback argument, either `{ error_message: string }` or a jQuery jqXHR-like `{ responseJSON?: { error_message?: string } }`.
- Produces: `error_message.value`, containing the backend message or `登录失败，请稍后重试`.

- [ ] **Step 1: Write the failing source-level regression test**

```js
import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(
  new URL('../../src/views/user/bot/account/UserAccountLoginView.vue', import.meta.url),
  'utf8'
)

test('shows backend login errors with a generic fallback', () => {
  assert.match(
    source,
    /error\(resp\)[\s\S]{0,240}resp\?\.error_message\s*\|\|[\s\S]{0,120}resp\?\.responseJSON\?\.error_message/
  )
  assert.match(source, /登录失败，请稍后重试/)
  assert.doesNotMatch(source, /error_message\.value\s*=\s*["']用户名或密码错误["']/)
})
```

- [ ] **Step 2: Run the test and verify the old hard-coded behavior fails**

Run: `cd web && npm run test:unit -- --test-name-pattern="shows backend login errors"`

Expected: FAIL because the callback is currently `error()` and assigns the hard-coded `用户名或密码错误` message.

- [ ] **Step 3: Implement the minimal response normalization**

Replace the login failure callback with:

```js
error(resp){
    error_message.value = resp?.error_message
        || resp?.responseJSON?.error_message
        || "登录失败，请稍后重试";
}
```

- [ ] **Step 4: Run focused and full frontend verification**

Run: `cd web && npm run test:unit -- --test-name-pattern="shows backend login errors"`

Expected: PASS for the focused regression test.

Run: `cd web && npm run test:unit`

Expected: all unit tests pass.

Run: `cd web && npm run lint`

Expected: lint exits with code 0.

- [ ] **Step 5: Review the exact implementation diff**

Run: `git diff --check -- web/src/views/user/bot/account/UserAccountLoginView.vue web/tests/unit/LoginErrorMessage.test.mjs`

Expected: no whitespace errors.

Run: `git diff -- web/src/views/user/bot/account/UserAccountLoginView.vue web/tests/unit/LoginErrorMessage.test.mjs`

Expected: only the login failure callback and its regression test are changed.

- [ ] **Step 6: Commit the implementation**

```bash
git add web/src/views/user/bot/account/UserAccountLoginView.vue web/tests/unit/LoginErrorMessage.test.mjs
git commit -m "fix: show backend login failure messages"
```
