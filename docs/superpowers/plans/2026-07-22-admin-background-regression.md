# Admin Background Regression Fix Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Restore the global background image on all three admin pages without removing their spacing or card treatment.

**Architecture:** The application-level `body` remains the only owner of `brck.avif`. Admin page shells keep their viewport sizing but stop painting an opaque background over the body.

**Tech Stack:** Vue 3 single-file components, scoped CSS, Node.js built-in test runner.

## Global Constraints

- Do not modify `App.vue` or duplicate the background image in admin views.
- Preserve the admin container spacing, card border, radius, shadow, and responsive rules.
- Do not change admin APIs, business logic, or modal behavior.

---

### Task 1: Remove the opaque admin page background with regression coverage

**Files:**
- Modify: `web/tests/unit/AdminListViews.test.mjs`
- Modify: `web/tests/unit/ManageTopicsView.test.mjs`
- Modify: `web/src/views/manage/ManageUsersView.vue`
- Modify: `web/src/views/manage/ManageLoginRecordsView.vue`
- Modify: `web/src/views/manage/ManageTopicsView.vue`

**Interfaces:**
- Consumes: Global `body` background from `web/src/App.vue` and the existing `.admin-page-shell` rules.
- Produces: Three admin shells with viewport height but no local `background` declaration.

- [ ] **Step 1: Write the failing regression tests**

Replace the old gray-background assertion in `AdminListViews.test.mjs` with:

```js
const shellRule = source.match(/\.admin-page-shell\s*\{([^}]*)\}/)?.[1] || ''
assert.match(shellRule, /min-height:\s*calc\(100vh - 56px\)/)
assert.doesNotMatch(shellRule, /background(?:-color)?\s*:/)
```

Add the same `shellRule` assertions to the topic layout test in `ManageTopicsView.test.mjs`. Keep the existing container-spacing and card-border assertions unchanged.

- [ ] **Step 2: Run the focused tests and verify RED**

```bash
cd web && node --test tests/unit/AdminListViews.test.mjs tests/unit/ManageTopicsView.test.mjs
```

Expected: two layout tests FAIL because all three shell rules still contain `background: #f4f7fb`.

- [ ] **Step 3: Remove only the covering backgrounds**

In each management view, change:

```css
.admin-page-shell { min-height: calc(100vh - 56px); background: #f4f7fb; }
```

to:

```css
.admin-page-shell { min-height: calc(100vh - 56px); }
```

Do not alter the remaining page or card styles.

- [ ] **Step 4: Run the focused tests and verify GREEN**

```bash
cd web && node --test tests/unit/AdminListViews.test.mjs tests/unit/ManageTopicsView.test.mjs
```

Expected: all focused tests PASS.

- [ ] **Step 5: Run complete frontend verification**

```bash
cd web
npm run test:unit
npm run lint
npm run build
cd ..
git diff --check
```

Expected: all unit tests pass, lint reports no errors, production build completes, and no whitespace errors are reported. Existing Browserslist age and bundle-size warnings are acceptable.

- [ ] **Step 6: Commit the regression fix**

```bash
git add web/tests/unit/AdminListViews.test.mjs web/tests/unit/ManageTopicsView.test.mjs web/src/views/manage/ManageUsersView.vue web/src/views/manage/ManageLoginRecordsView.vue web/src/views/manage/ManageTopicsView.vue
git commit -m "fix: restore admin background image"
```
