# Admin Layout and Scrollable Modals Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add visible page spacing and card framing to all three admin pages, and keep topic/evaluate modal actions reachable when content is long.

**Architecture:** Each admin view will use a full-width background shell containing a Bootstrap `container` and the existing management card. The two long-form modals will use a viewport-constrained flex column: header and footer remain fixed while `.modal-body` becomes the only vertical scroll region.

**Tech Stack:** Vue 3 single-file components, Bootstrap 5 utility/classes, scoped CSS, Node.js built-in test runner.

## Global Constraints

- Do not change management APIs, payloads, or business workflows.
- Keep the existing responsive table behavior.
- Apply scrolling to `TopicFormModal` and `EvaluateModal`; do not change `DeleteTopicModal`.
- Preserve existing modal accessibility attributes, loading states, and disabled submission behavior.

---

### Task 1: Add a unified spaced card shell to the three admin pages

**Files:**
- Modify: `web/tests/unit/AdminListViews.test.mjs`
- Modify: `web/tests/unit/ManageTopicsView.test.mjs`
- Modify: `web/src/views/manage/ManageUsersView.vue`
- Modify: `web/src/views/manage/ManageLoginRecordsView.vue`
- Modify: `web/src/views/manage/ManageTopicsView.vue`

**Interfaces:**
- Consumes: Existing admin page templates and Bootstrap `container`/`card` classes.
- Produces: Every page has `.admin-page-shell > .container.admin-page-container > .card.admin-card`.

- [ ] **Step 1: Write failing layout tests**

Add a reusable assertion to `AdminListViews.test.mjs` and a matching assertion to `ManageTopicsView.test.mjs`:

```js
const assertAdminCardShell = source => {
  assert.match(source, /<main class="admin-page-shell">/)
  assert.match(source, /<div class="container admin-page-container">[\s\S]*<section class="card admin-card">/)
  assert.match(source, /\.admin-page-shell\s*\{[\s\S]*background:\s*#f4f7fb/)
  assert.match(source, /\.admin-page-container\s*\{[\s\S]*padding-top:\s*1\.5rem/)
  assert.match(source, /\.admin-card\s*\{[\s\S]*border:\s*1px solid/)
}
```

Call the helper for user and login-record sources, and assert the same structure for the topic source.

- [ ] **Step 2: Run the focused tests and verify RED**

Run:

```bash
cd web && node --test tests/unit/AdminListViews.test.mjs tests/unit/ManageTopicsView.test.mjs
```

Expected: FAIL because `admin-page-shell`, `admin-page-container`, the gray background, and visible card border do not exist.

- [ ] **Step 3: Implement the page shells**

In each of the three views, change the outer structure from:

```vue
<main class="container admin-page">
  <section class="card admin-card">
    <!-- existing page content -->
  </section>
</main>
```

to:

```vue
<main class="admin-page-shell">
  <div class="container admin-page-container">
    <section class="card admin-card">
      <!-- existing page content -->
    </section>
  </div>
</main>
```

For `ManageTopicsView.vue`, keep all three modal components inside `<main>` but outside the new container so their fixed positioning is unaffected.

Replace the old page/card rules in all three files with:

```css
.admin-page-shell { min-height: calc(100vh - 56px); background: #f4f7fb; }
.admin-page-container { padding-top: 1.5rem; padding-bottom: 2.5rem; }
.admin-card { border: 1px solid rgba(148, 163, 184, .24); border-radius: 14px; box-shadow: 0 10px 30px rgba(15, 23, 42, .08); overflow: hidden; }
@media (max-width: 575.98px) {
  .admin-page-container { padding-top: .75rem; padding-bottom: 1.25rem; }
}
```

- [ ] **Step 4: Run the focused tests and verify GREEN**

Run:

```bash
cd web && node --test tests/unit/AdminListViews.test.mjs tests/unit/ManageTopicsView.test.mjs
```

Expected: all focused tests PASS.

- [ ] **Step 5: Commit the page shell change**

```bash
git add web/tests/unit/AdminListViews.test.mjs web/tests/unit/ManageTopicsView.test.mjs web/src/views/manage/ManageUsersView.vue web/src/views/manage/ManageLoginRecordsView.vue web/src/views/manage/ManageTopicsView.vue
git commit -m "style: frame admin pages with cards"
```

---

### Task 2: Make topic and evaluate modal bodies independently scrollable

**Files:**
- Modify: `web/tests/unit/TopicFormModal.test.mjs`
- Modify: `web/tests/unit/TopicActionModals.test.mjs`
- Modify: `web/src/components/manage/TopicFormModal.vue`
- Modify: `web/src/components/manage/EvaluateModal.vue`

**Interfaces:**
- Consumes: Existing modal props, emitted events, Bootstrap modal markup, and form submission handlers.
- Produces: `.admin-modal-form` flex forms and `.admin-modal-body` scrolling regions inside viewport-constrained modal content.

- [ ] **Step 1: Write failing scroll-layout tests**

Add these assertions to the topic and evaluate modal test files:

```js
assert.match(source, /<form class="admin-modal-form"/)
assert.match(source, /<div class="modal-body admin-modal-body">/)
assert.match(source, /\.modal-content\s*\{[\s\S]*max-height:\s*calc\(100vh - 2rem\)/)
assert.match(source, /\.admin-modal-form\s*\{[\s\S]*min-height:\s*0/)
assert.match(source, /\.admin-modal-body\s*\{[\s\S]*overflow-y:\s*auto/)
```

Use `source` in `TopicFormModal.test.mjs` and `evaluateSource` in `TopicActionModals.test.mjs`.

- [ ] **Step 2: Run the focused tests and verify RED**

Run:

```bash
cd web && node --test tests/unit/TopicFormModal.test.mjs tests/unit/TopicActionModals.test.mjs
```

Expected: FAIL because the form/body classes and explicit flex/overflow rules are missing.

- [ ] **Step 3: Implement the scrollable flex modal structure**

In both modal templates, change:

```vue
<form @submit.prevent="submitForm">
  <div class="modal-body">
```

or the corresponding `submitRows` form to:

```vue
<form class="admin-modal-form" @submit.prevent="submitForm">
  <div class="modal-body admin-modal-body">
```

Use `@submit.prevent="submitRows"` unchanged in `EvaluateModal.vue`.

Add the following scoped rules to both components:

```css
.modal-dialog { margin-top: 1rem; margin-bottom: 1rem; }
.modal-content { max-height: calc(100vh - 2rem); overflow: hidden; }
.admin-modal-form { display: flex; min-height: 0; flex: 1 1 auto; flex-direction: column; }
.admin-modal-body { min-height: 0; overflow-y: auto; overscroll-behavior: contain; }
.modal-header, .modal-footer { flex: 0 0 auto; }
```

Keep the existing z-index, state, case-card, and textarea rules.

- [ ] **Step 4: Run the focused tests and verify GREEN**

Run:

```bash
cd web && node --test tests/unit/TopicFormModal.test.mjs tests/unit/TopicActionModals.test.mjs
```

Expected: all focused tests PASS.

- [ ] **Step 5: Commit the modal scrolling change**

```bash
git add web/tests/unit/TopicFormModal.test.mjs web/tests/unit/TopicActionModals.test.mjs web/src/components/manage/TopicFormModal.vue web/src/components/manage/EvaluateModal.vue
git commit -m "fix: keep admin modal actions reachable"
```

---

### Task 3: Run complete frontend verification

**Files:**
- Verify: `web/src/views/manage/*.vue`
- Verify: `web/src/components/manage/TopicFormModal.vue`
- Verify: `web/src/components/manage/EvaluateModal.vue`
- Verify: `web/tests/unit/*.test.mjs`

**Interfaces:**
- Consumes: The completed page and modal layout changes from Tasks 1 and 2.
- Produces: Verified frontend code ready for delivery.

- [ ] **Step 1: Run all unit tests**

```bash
cd web && npm run test:unit
```

Expected: all tests PASS with zero failures.

- [ ] **Step 2: Run lint**

```bash
cd web && npm run lint
```

Expected: exit code 0 with no lint errors.

- [ ] **Step 3: Run the production build**

```bash
cd web && npm run build
```

Expected: build completes successfully; existing Browserslist age and bundle-size warnings are acceptable.

- [ ] **Step 4: Inspect the final diff and repository state**

```bash
git diff --check
git status --short
```

Expected: no whitespace errors and only the planned files are changed before the final commit.
