# Judge Result Modal Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make the submit button remain the only waiting indicator, then show either a WebSocket judge result or an HTTP submission error in the existing result modal.

**Architecture:** Extract judge-message validation and HTTP error normalization into a small pure utility so the asynchronous state transitions can be tested without mounting the large Ace-based view. `QuestionDetails.vue` owns the loading/modal state, while `ResultModal.vue` renders either a normal judge result or a submission error from one normalized result object.

**Tech Stack:** Vue 3 Composition API, jQuery Ajax, browser WebSocket, Node.js built-in test runner, ESLint

## Global Constraints

- Do not modify backend APIs or the WebSocket protocol.
- Do not display judge status information below the submit button.
- A successful HTTP response does not finish the waiting state; only a valid WebSocket judge result does.
- An HTTP submission failure stops waiting immediately and opens the result modal.
- Invalid or unrelated WebSocket messages do not change the waiting state.
- Do not add a judge timeout, WebSocket reconnection, or unrelated refactoring.

---

### Task 1: Normalize Judge Results and HTTP Errors

**Files:**
- Create: `web/src/utils/judgeSubmission.mjs`
- Create: `web/tests/unit/judgeSubmission.test.mjs`

**Interfaces:**
- Consumes: jQuery-compatible failure object with optional `responseJSON` and `responseText`; parsed WebSocket message object.
- Produces: `extractSubmissionError(xhr): string`, `isJudgeResult(data): boolean`, `toJudgeModalResult(data): object`, and `toSubmissionErrorResult(message): object`.

- [ ] **Step 1: Write failing utility tests**

```js
import test from 'node:test'
import assert from 'node:assert/strict'
import {
  extractSubmissionError,
  isJudgeResult,
  toJudgeModalResult,
  toSubmissionErrorResult
} from '../../src/utils/judgeSubmission.mjs'

test('extracts an HTTP submission error using the documented priority', () => {
  assert.equal(extractSubmissionError({
    responseJSON: { message: '登录已过期', error: 'ignored' },
    responseText: 'also ignored'
  }), '登录已过期')
  assert.equal(extractSubmissionError({ responseJSON: { error: '请求被拒绝' } }), '请求被拒绝')
  assert.equal(extractSubmissionError({ responseText: '网络错误' }), '网络错误')
  assert.equal(extractSubmissionError({ responseJSON: { message: '   ' } }), '提交失败，请稍后重试')
})

test('accepts only WebSocket messages containing score and state', () => {
  assert.equal(isJudgeResult({ score: 100, state: 'accepted' }), true)
  assert.equal(isJudgeResult({ score: 0 }), false)
  assert.equal(isJudgeResult(null), false)
})

test('creates normalized modal data for judge results and submission errors', () => {
  assert.deepEqual(toJudgeModalResult({
    user_id: 7,
    evaluation_id: 9,
    score: 100,
    state: 'accepted'
  }), {
    user_id: 7,
    evaluation_id: 9,
    score: 100,
    state: 'accepted',
    message: ''
  })
  assert.deepEqual(toSubmissionErrorResult('服务不可用'), {
    user_id: null,
    evaluation_id: null,
    score: null,
    state: 'submission_error',
    message: '服务不可用'
  })
})
```

- [ ] **Step 2: Run the utility test and verify RED**

Run: `cd web && node --test tests/unit/judgeSubmission.test.mjs`

Expected: FAIL with `ERR_MODULE_NOT_FOUND` for `src/utils/judgeSubmission.mjs`.

- [ ] **Step 3: Implement the pure normalization utility**

```js
const FALLBACK_SUBMISSION_ERROR = '提交失败，请稍后重试'

function nonEmptyString(value) {
  return typeof value === 'string' && value.trim() ? value.trim() : ''
}

export function extractSubmissionError(xhr = {}) {
  return nonEmptyString(xhr?.responseJSON?.message)
    || nonEmptyString(xhr?.responseJSON?.error)
    || nonEmptyString(xhr?.responseText)
    || FALLBACK_SUBMISSION_ERROR
}

export function isJudgeResult(data) {
  return Boolean(data && data.score !== undefined && data.state !== undefined)
}

export function toJudgeModalResult(data) {
  return {
    user_id: data.user_id,
    evaluation_id: data.evaluation_id,
    score: data.score,
    state: data.state,
    message: ''
  }
}

export function toSubmissionErrorResult(message) {
  return {
    user_id: null,
    evaluation_id: null,
    score: null,
    state: 'submission_error',
    message: nonEmptyString(message) || FALLBACK_SUBMISSION_ERROR
  }
}
```

- [ ] **Step 4: Run the utility test and verify GREEN**

Run: `cd web && node --test tests/unit/judgeSubmission.test.mjs`

Expected: 3 tests pass, 0 fail.

- [ ] **Step 5: Commit the utility and tests**

```bash
git add web/src/utils/judgeSubmission.mjs web/tests/unit/judgeSubmission.test.mjs
git commit -m "test: define judge submission state mapping"
```

---

### Task 2: Drive the Submit Button and Modal from Async Results

**Files:**
- Modify: `web/src/views/ranklist/QuestionDetails.vue:1-702`
- Modify: `web/src/components/ResultModal.vue:1-137`
- Create: `web/tests/unit/JudgeResultUi.test.mjs`

**Interfaces:**
- Consumes: Task 1 exports from `web/src/utils/judgeSubmission.mjs`.
- Produces: a view where `isSubmitting` remains true after HTTP success, becomes false on HTTP error or valid WebSocket result, and `ResultModal` supports `submission_error`.

- [ ] **Step 1: Write failing UI contract tests**

```js
import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const questionSource = await readFile(
  new URL('../../src/views/ranklist/QuestionDetails.vue', import.meta.url),
  'utf8'
)
const modalSource = await readFile(
  new URL('../../src/components/ResultModal.vue', import.meta.url),
  'utf8'
)

test('uses only the submit button as the pending judge indicator', () => {
  assert.match(questionSource, /v-if="isSubmitting"/)
  assert.doesNotMatch(questionSource, /submissionResult/)
  assert.doesNotMatch(questionSource, /提交结果/)
  assert.doesNotMatch(questionSource, /getResultAlertClass/)
})

test('keeps waiting after HTTP success and stops on HTTP error or a judge result', () => {
  assert.doesNotMatch(questionSource, /complete\(\)[\s\S]{0,100}isSubmitting\.value = false/)
  assert.match(questionSource, /error\(xhr\)[\s\S]{0,240}isSubmitting\.value = false/)
  assert.match(questionSource, /isJudgeResult\(data\)[\s\S]{0,240}isSubmitting\.value = false/)
  assert.match(questionSource, /toSubmissionErrorResult\(extractSubmissionError\(xhr\)\)/)
  assert.match(questionSource, /toJudgeModalResult\(data\)/)
})

test('renders submission errors without score, record details, or solution action', () => {
  assert.match(modalSource, /isSubmissionError/)
  assert.match(modalSource, /v-if="!isSubmissionError" class="result-score"/)
  assert.match(modalSource, /v-if="!isSubmissionError" class="result-details"/)
  assert.match(modalSource, /resultData\.state !== 'accepted' && !isSubmissionError/)
  assert.match(modalSource, /submission_error: '提交失败'/)
  assert.match(modalSource, /props\.resultData\.message/)
})
```

- [ ] **Step 2: Run the UI contract test and verify RED**

Run: `cd web && node --test tests/unit/JudgeResultUi.test.mjs`

Expected: FAIL because `submissionResult` and the inline “提交结果” block still exist, and submission error rendering is absent.

- [ ] **Step 3: Remove the inline result area and obsolete state**

In `QuestionDetails.vue`, delete the template block beginning with `<!-- 提交结果 -->`, delete `submissionResult`, `getResultAlertClass`, and `getResultMessage`, and remove them from the setup return object. Keep the separate test-case list unchanged.

Replace the initial modal result with:

```js
const websocketResult = ref({
  user_id: null,
  evaluation_id: null,
  score: null,
  state: '',
  message: ''
})
```

- [ ] **Step 4: Connect HTTP failure and WebSocket success to modal state**

Add the import:

```js
import {
  extractSubmissionError,
  isJudgeResult,
  toJudgeModalResult,
  toSubmissionErrorResult
} from '../../utils/judgeSubmission.mjs'
```

Replace the Ajax callbacks with:

```js
success(resp) {
  console.log('提交成功，等待判题结果:', resp)
},
error(xhr) {
  isSubmitting.value = false
  websocketResult.value = toSubmissionErrorResult(extractSubmissionError(xhr))
  showResultModal.value = true
}
```

Do not retain a `complete` callback. In the WebSocket `onmessage` handler, replace the current condition/body with:

```js
if (isJudgeResult(data)) {
  isSubmitting.value = false
  websocketResult.value = toJudgeModalResult(data)
  showResultModal.value = true
}
```

- [ ] **Step 5: Render submission errors in the existing modal**

In `ResultModal.vue`, import `computed` as before and add:

```js
const isSubmissionError = computed(() => props.resultData.state === 'submission_error')
```

Use `v-if="!isSubmissionError"` on `.result-score` and `.result-details`. Change the solution button condition to:

```html
<button
  v-if="resultData.state !== 'accepted' && !isSubmissionError"
  class="btn btn-outline-secondary"
  @click="viewSolution"
>
  查看题解
</button>
```

Use the following state-specific text:

```js
// resultTitle titles map
submission_error: '提交失败'

// resultDesc, before the normal desc map fallback
if (isSubmissionError.value) {
  return props.resultData.message || '提交失败，请稍后重试'
}

// primary button label
{{ isSubmissionError ? '返回修改' : (resultData.state === 'accepted' ? '继续挑战' : '修改代码') }}
```

Treat `submission_error` as dangerous styling by including it in `result-danger`, excluding it from `result-info`, and return `isSubmissionError` from setup.

- [ ] **Step 6: Run focused tests and verify GREEN**

Run: `cd web && node --test tests/unit/judgeSubmission.test.mjs tests/unit/JudgeResultUi.test.mjs`

Expected: 6 tests pass, 0 fail.

- [ ] **Step 7: Commit the UI integration**

```bash
git add web/src/views/ranklist/QuestionDetails.vue web/src/components/ResultModal.vue web/tests/unit/JudgeResultUi.test.mjs
git commit -m "feat: show judge feedback only in result modal"
```

---

### Task 3: Regression Verification

**Files:**
- Verify only; no production files should change.

**Interfaces:**
- Consumes: completed judge submission UI from Tasks 1 and 2.
- Produces: evidence that unit tests, lint, build, and whitespace checks pass.

- [ ] **Step 1: Run all frontend unit tests**

Run: `cd web && npm run test:unit`

Expected: all tests pass with 0 failures.

- [ ] **Step 2: Run frontend lint**

Run: `cd web && npm run lint`

Expected: exit code 0 with no lint errors.

- [ ] **Step 3: Build the frontend**

Run: `cd web && npm run build`

Expected: exit code 0. Existing browser-data or bundle-size warnings are acceptable; compilation errors are not.

- [ ] **Step 4: Check the final diff**

Run: `git diff --check`

Expected: no output and exit code 0.

- [ ] **Step 5: Review scope and repository status**

Run: `git status --short && git diff --stat HEAD~2..HEAD`

Expected: the feature commits contain only the utility, judge UI, tests, and documentation. Pre-existing `.DS_Store` and backup-directory changes remain untouched.
