# Problem List Row Navigation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Remove problem-management buttons from the problem library and make each hovered problem row navigate directly to its detail page.

**Architecture:** Keep navigation in the existing `handleView(id)` method and bind mouse and keyboard activation directly to each table row. Remove the now-unreachable add/delete UI, state, Ajax methods, imports, columns, and button styles from the same view.

**Tech Stack:** Vue 3 Composition API, Vue Router, Node.js built-in test runner, ESLint

## Global Constraints

- Keep the title, problem count, search, loading, error, empty, and footer behavior unchanged.
- Keep only problem number, title/description, and difficulty columns.
- Do not modify backend add/remove APIs or the problem detail page.
- Support mouse click, Enter, and Space navigation on each problem row.
- Preserve unrelated working-tree files.

---

### Task 1: Replace Management Buttons with Row Navigation

**Files:**
- Modify: `web/src/views/ranklist/RanKlistIndexView.vue:1-600`
- Create: `web/tests/unit/ProblemListNavigation.test.mjs`

**Interfaces:**
- Consumes: existing `handleView(id)` method and named Vue Router route `Details`.
- Produces: clickable, focusable problem rows with `role="link"`, plus a problem list with no add/view/delete controls.

- [ ] **Step 1: Write the failing UI contract tests**

```js
import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(
  new URL('../../src/views/ranklist/RanKlistIndexView.vue', import.meta.url),
  'utf8'
)

test('removes problem add, view, and delete controls and their dead logic', () => {
  assert.doesNotMatch(source, />\s*添加题目\s*</)
  assert.doesNotMatch(source, />\s*查看\s*</)
  assert.doesNotMatch(source, />\s*删除\s*</)
  assert.doesNotMatch(source, /topicadd|addtopic|removetopic|errortopic/)
  assert.doesNotMatch(source, /bootstrap\/dist\/js\/bootstrap|\breactive\b/)
})

test('navigates to problem details from the whole row by mouse or keyboard', () => {
  assert.match(source, /<tr[^>]*class="problem-row"/)
  assert.match(source, /@click="handleView\(problem\.id\)"/)
  assert.match(source, /@keydown\.enter="handleView\(problem\.id\)"/)
  assert.match(source, /@keydown\.space\.prevent="handleView\(problem\.id\)"/)
  assert.match(source, /role="link"/)
  assert.match(source, /tabindex="0"/)
})

test('uses a pointer cursor for interactive problem rows', () => {
  assert.match(source, /\.problem-row\s*\{[^}]*cursor:\s*pointer/s)
})
```

- [ ] **Step 2: Run the new test and verify RED**

Run: `cd web && node --test tests/unit/ProblemListNavigation.test.mjs`

Expected: 3 failures because management controls remain and rows are not interactive.

- [ ] **Step 3: Remove the add UI and rebalance the header**

Change both retained header columns from `col-md-4` to `col-md-6`, then delete the third header column containing the add button and modal. The header should contain only:

```html
<div class="col-md-6">
  <h5 class="mb-0">题库列表</h5>
  <p class="text-muted mb-0 small">共 {{ problems.length }} 道题目</p>
</div>
<div class="col-md-6">
  <!-- existing search input group unchanged -->
</div>
```

- [ ] **Step 4: Remove action columns and bind whole-row navigation**

Keep the first three headers and replace the row opening tag with:

```html
<tr
  v-for="problem in filteredProblems"
  :key="problem.id"
  class="problem-row"
  role="link"
  tabindex="0"
  @click="handleView(problem.id)"
  @keydown.enter="handleView(problem.id)"
  @keydown.space.prevent="handleView(problem.id)"
>
```

Delete the “查看”和“删除” headers and data cells. Keep the problem number, title/description, and difficulty cells unchanged.

- [ ] **Step 5: Remove dead script and CSS**

Use only the required Vue imports:

```js
import { ref, onMounted, computed } from 'vue'
```

Delete the Bootstrap `Modal` import, `topicadd`, `errortopic`, `addtopic`, `removetopic`, and their setup return entries. Delete the problem-table view-button CSS block and add:

```css
.problem-row {
  cursor: pointer;
}

.problem-row:focus-visible {
  outline: 2px solid #0d6efd;
  outline-offset: -2px;
}
```

- [ ] **Step 6: Run the focused test and verify GREEN**

Run: `cd web && node --test tests/unit/ProblemListNavigation.test.mjs`

Expected: 3 tests pass, 0 fail.

- [ ] **Step 7: Commit the row-navigation change**

```bash
git add web/src/views/ranklist/RanKlistIndexView.vue web/tests/unit/ProblemListNavigation.test.mjs
git commit -m "feat: navigate problem list from clickable rows"
```

---

### Task 2: Regression Verification

**Files:**
- Verify only; no production files should change.

**Interfaces:**
- Consumes: completed problem-list navigation from Task 1.
- Produces: evidence that the frontend tests, lint, build, and diff checks pass.

- [ ] **Step 1: Run all frontend unit tests**

Run: `cd web && npm run test:unit`

Expected: all tests pass with 0 failures.

- [ ] **Step 2: Run frontend lint**

Run: `cd web && npm run lint`

Expected: exit code 0 with no lint errors.

- [ ] **Step 3: Build the frontend**

Run: `cd web && npm run build`

Expected: exit code 0. Existing browser-data and bundle-size warnings are acceptable.

- [ ] **Step 4: Check whitespace and scope**

Run: `git diff --check && git status --short`

Expected: no whitespace errors; pre-existing `.DS_Store`, backup, and generated-code directories remain untouched.
