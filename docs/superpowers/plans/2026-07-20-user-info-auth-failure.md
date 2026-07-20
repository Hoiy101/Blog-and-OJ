# User Info Auth Failure Handling Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Clear all local authentication state and return to the login page whenever the user information request reports a business or HTTP failure.

**Architecture:** Centralize Token and Vuex state cleanup inside the `getinfo` action so both failure channels behave identically. Keep navigation in the login view through one shared callback, avoiding a circular dependency between the Store and Router.

**Tech Stack:** Vue 3, Vuex 4, Vue Router 4, jQuery Ajax, Node.js built-in test runner

## Global Constraints

- Handle both non-`success` business responses and HTTP request errors.
- Remove `jwt_token` from `localStorage` and clear Vuex user state before notifying the page.
- Use `登录状态已失效，请重新登录` when no backend error message exists.
- Do not import Router into the Vuex module or change backend code.
- Preserve the successful user information flow.

---

### Task 1: Clear invalid authentication and return to login

**Files:**
- Create: `web/tests/unit/UserInfoAuthFailure.test.mjs`
- Modify: `web/src/store/user.js:61-86`
- Modify: `web/src/views/user/bot/account/UserAccountLoginView.vue:36-71`

**Interfaces:**
- Consumes: `getinfo` failures shaped as `{ error_message?: string }` or `{ responseJSON?: { error_message?: string } }`.
- Produces: a logged-out Vuex state, no `localStorage.jwt_token`, an error message, and navigation to the `user_account_login` route.

- [ ] **Step 1: Write the failing source-level regression test**

```js
import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const storeSource = await readFile(
  new URL('../../src/store/user.js', import.meta.url),
  'utf8'
)
const loginViewSource = await readFile(
  new URL('../../src/views/user/bot/account/UserAccountLoginView.vue', import.meta.url),
  'utf8'
)

test('clears authentication for business and HTTP getinfo failures', () => {
  const getinfoAction = storeSource.match(
    /getinfo\(context, data\) \{([\s\S]*?)\n        \},\n        uploadAvatar/
  )?.[1]

  assert.ok(getinfoAction, 'getinfo action should exist')
  assert.match(getinfoAction, /const handleError = \(resp\) => \{[\s\S]*localStorage\.removeItem\("jwt_token"\)[\s\S]*context\.commit\("logout"\)[\s\S]*data\.error\(resp\)/)
  assert.equal(getinfoAction.match(/handleError\(resp\)/g)?.length, 2)
})

test('both getinfo calls show the failure and return to login', () => {
  assert.match(loginViewSource, /const handleGetInfoError = \(resp\) => \{/)
  assert.match(loginViewSource, /登录状态已失效，请重新登录/)
  assert.match(loginViewSource, /router\.replace\(\{ name: 'user_account_login' \}\)/)
  assert.equal(loginViewSource.match(/error:\s*handleGetInfoError/g)?.length, 2)
})
```

- [ ] **Step 2: Run the focused test and verify the missing behavior fails**

Run: `cd web && npm run test:unit -- --test-name-pattern="getinfo failures|return to login"`

Expected: both new tests FAIL because `getinfo` does not clear authentication and the view does not share a redirecting failure callback.

- [ ] **Step 3: Centralize authentication cleanup in the Vuex action**

At the start of `getinfo(context, data)`, add:

```js
const handleError = (resp) => {
    localStorage.removeItem("jwt_token");
    context.commit("logout");
    data.error(resp);
};
```

Replace the business-failure branch and the Ajax error callback with:

```js
else{
    handleError(resp);
}
```

```js
error(resp){
    handleError(resp);
}
```

- [ ] **Step 4: Share one user information failure callback in the login view**

After the login form refs, add:

```js
const handleGetInfoError = (resp) => {
    store.commit("updatePullingInfo", false);
    error_message.value = resp?.error_message
        || resp?.responseJSON?.error_message
        || "登录状态已失效，请重新登录";
    router.replace({ name: 'user_account_login' });
};
```

In both `store.dispatch("getinfo", ...)` calls, provide:

```js
error: handleGetInfoError
```

Keep the existing success callbacks unchanged.

- [ ] **Step 5: Run focused and complete frontend verification**

Run: `cd web && npm run test:unit -- --test-name-pattern="getinfo failures|return to login"`

Expected: both focused tests PASS.

Run: `cd web && npm run test:unit`

Expected: all unit tests pass.

Run: `cd web && npm run lint`

Expected: lint exits with code 0.

- [ ] **Step 6: Review the exact implementation diff**

Run: `git diff --check -- web/src/store/user.js web/src/views/user/bot/account/UserAccountLoginView.vue web/tests/unit/UserInfoAuthFailure.test.mjs`

Expected: no whitespace errors.

Run: `git diff -- web/src/store/user.js web/src/views/user/bot/account/UserAccountLoginView.vue web/tests/unit/UserInfoAuthFailure.test.mjs`

Expected: only shared `getinfo` cleanup, shared navigation handling, and regression tests are changed.

- [ ] **Step 7: Commit the implementation**

```bash
git add web/src/store/user.js web/src/views/user/bot/account/UserAccountLoginView.vue web/tests/unit/UserInfoAuthFailure.test.mjs
git commit -m "fix: clear authentication after user info failure"
```
