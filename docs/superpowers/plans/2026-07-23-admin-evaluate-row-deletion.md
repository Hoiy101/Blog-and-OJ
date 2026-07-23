# Admin Evaluate Row Deletion Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add an accessible red remove control to every judge-case card and omit locally removed rows from the existing batch-save payload.

**Architecture:** `EvaluateModal.vue` remains the owner of the editable row copy. A new `removeRow(index)` mutates only that local array; the existing submit handler emits a copy of the remaining rows, so `ManageTopicsView.vue` and the API layer need no changes.

**Tech Stack:** Vue 3 single-file components, Bootstrap 5 classes, scoped CSS, Node.js built-in test runner.

## Global Constraints

- Do not add or modify backend endpoints.
- Do not stage, commit, overwrite, or otherwise modify the existing uncommitted backend file.
- Do not send a request when the red remove control is clicked.
- Preserve judge-case creation, batch submission, error display, and modal scrolling behavior.
- Do not add a confirmation or undo interaction.

---

### Task 1: Add local judge-case row deletion

**Files:**
- Modify: `web/tests/unit/TopicActionModals.test.mjs`
- Modify: `web/src/components/manage/EvaluateModal.vue`

**Interfaces:**
- Consumes: `rows: Ref<Array<{ id: string, input: string, output: string }>>` and the existing `submitRows()` event flow.
- Produces: `removeRow(index: number): void`, a per-row remove button, and submission containing only remaining rows.

- [ ] **Step 1: Write the failing row-deletion test**

Add this test to `web/tests/unit/TopicActionModals.test.mjs`:

```js
test('removes a judge case locally before batch submission', () => {
  assert.match(evaluateSource, /class="btn btn-sm btn-link text-danger p-0 case-remove"/)
  assert.match(evaluateSource, /:aria-label="`删除判例 \$\{index \+ 1\}`"/)
  assert.match(evaluateSource, /:disabled="submitting \|\| loading"/)
  assert.match(evaluateSource, /@click="removeRow\(index\)"/)
  assert.match(evaluateSource, /const removeRow = index => \{[\s\S]*rows\.value\.splice\(index, 1\)/)
  assert.match(evaluateSource, /return \{ rows, addRow, removeRow, requestClose, submitRows \}/)
  assert.match(evaluateSource, /emit\('submit', rows\.value\.map\(row => \(\{ \.\.\.row \}\)\)\)/)
})
```

- [ ] **Step 2: Run the focused test and verify RED**

Run:

```bash
cd web && node --test tests/unit/TopicActionModals.test.mjs
```

Expected: FAIL because the remove button and `removeRow` function do not exist.

- [ ] **Step 3: Add the per-row remove control**

Replace the current ID/new badge block in the card header with:

```vue
<div class="d-flex align-items-center gap-2">
  <span v-if="row.id" class="badge bg-light text-dark">ID: {{ row.id }}</span>
  <span v-else class="badge bg-info text-dark">新增</span>
  <button
    type="button"
    class="btn btn-sm btn-link text-danger p-0 case-remove"
    :aria-label="`删除判例 ${index + 1}`"
    :disabled="submitting || loading"
    @click="removeRow(index)"
  >
    &times;
  </button>
</div>
```

Add the local removal function immediately after `addRow`:

```js
const removeRow = index => {
  rows.value.splice(index, 1)
}
```

Expose it from `setup`:

```js
return { rows, addRow, removeRow, requestClose, submitRows }
```

Add a scoped style that makes the control clearly visible without changing the card layout:

```css
.case-remove { width: 1.75rem; height: 1.75rem; font-size: 1.6rem; line-height: 1; text-decoration: none; }
```

- [ ] **Step 4: Run the focused test and verify GREEN**

Run:

```bash
cd web && node --test tests/unit/TopicActionModals.test.mjs
```

Expected: all focused tests PASS.

- [ ] **Step 5: Run complete frontend verification**

Run:

```bash
cd web
npm run test:unit
npm run lint
npm run build
cd ..
git diff --check
git status --short
```

Expected: all unit tests pass, lint reports no errors, production build completes, and status contains only the two planned frontend files plus the pre-existing backend modification before the frontend commit.

- [ ] **Step 6: Commit only the frontend feature**

Run:

```bash
git add web/tests/unit/TopicActionModals.test.mjs web/src/components/manage/EvaluateModal.vue
git commit -m "feat: remove judge cases in admin modal"
```

Expected: the frontend test and component are committed; `backendcloud/backend/src/main/java/com/kob/backend/service/impl/manage/topic/UpdataEvaluateServiceImpl.java` remains modified and unstaged.
