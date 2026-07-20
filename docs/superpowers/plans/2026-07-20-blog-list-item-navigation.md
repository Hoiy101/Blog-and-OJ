# Blog List Item Navigation Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Remove the blog “查看详情” button and open the existing blog detail view from the complete hovered blog item.

**Architecture:** Keep the existing `viewBlogDetail(blogId)` Ajax and state flow unchanged, and move its activation from a nested button to the `.blog-item` container. Add native link semantics, keyboard activation, and focus styling directly to that container while preserving the existing light-gray hover behavior.

**Tech Stack:** Vue 3 Composition API, Node.js built-in test runner, ESLint

## Global Constraints

- Keep the existing `#f8fafc` light-gray hover background and horizontal movement.
- Do not add or modify routes.
- Do not modify `viewBlogDetail`, blog-detail rendering, image behavior, Markdown behavior, or backend APIs.
- Preserve unrelated backend working-tree changes.
- Support click, Enter, and Space activation on every blog item.

---

### Task 1: Make the Complete Blog Item Open Its Detail

**Files:**
- Modify: `web/src/views/pk/PkIndexView.vue:56-78,480-495`
- Create: `web/tests/unit/BlogListNavigation.test.mjs`

**Interfaces:**
- Consumes: existing `viewBlogDetail(blogId)` method.
- Produces: focusable `.blog-item` containers with mouse and keyboard activation and no “查看详情” button.

- [ ] **Step 1: Write the failing UI contract tests**

```js
import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(
  new URL('../../src/views/pk/PkIndexView.vue', import.meta.url),
  'utf8'
)

test('removes the blog detail button and activates the complete blog item', () => {
  assert.doesNotMatch(source, />\s*查看详情\s*</)
  assert.match(source, /class="blog-item border-bottom p-4"/)
  assert.match(source, /@click="viewBlogDetail\(record\.id\)"/)
})

test('supports keyboard navigation with link semantics', () => {
  assert.match(source, /role="link"/)
  assert.match(source, /tabindex="0"/)
  assert.match(source, /@keydown\.enter="viewBlogDetail\(record\.id\)"/)
  assert.match(source, /@keydown\.space\.prevent="viewBlogDetail\(record\.id\)"/)
})

test('preserves the light-gray hover and exposes mouse and keyboard focus', () => {
  assert.match(source, /\.blog-item\s*\{[^}]*cursor:\s*pointer/s)
  assert.match(source, /\.blog-item:hover\s*\{[^}]*background-color:\s*#f8fafc/s)
  assert.match(source, /\.blog-item:focus-visible\s*\{[^}]*outline:/s)
})
```

- [ ] **Step 2: Run the new test and verify RED**

Run: `cd web && node --test tests/unit/BlogListNavigation.test.mjs`

Expected: all 3 tests fail because the button remains, the container has no events or link semantics, and focus styling is absent.

- [ ] **Step 3: Move activation to the blog item and remove the button**

Replace the blog item opening tag with:

```html
<div
  v-for="record in filteredRecords"
  :key="record.id"
  class="blog-item border-bottom p-4"
  role="link"
  tabindex="0"
  @click="viewBlogDetail(record.id)"
  @keydown.enter="viewBlogDetail(record.id)"
  @keydown.space.prevent="viewBlogDetail(record.id)"
>
```

Delete this nested action block completely:

```html
<div class="d-flex justify-content-end">
  <button
    class="btn btn-sm btn-outline-primary me-2"
    @click="viewBlogDetail(record.id)"
  >
    查看详情
  </button>
</div>
```

- [ ] **Step 4: Add keyboard focus styling without changing hover styling**

Keep the existing `.blog-item` and `.blog-item:hover` rules. Insert between them:

```css
.blog-item:focus-visible {
  outline: 2px solid #0d6efd;
  outline-offset: -2px;
}
```

- [ ] **Step 5: Run the focused test and verify GREEN**

Run: `cd web && node --test tests/unit/BlogListNavigation.test.mjs`

Expected: 3 tests pass, 0 fail.

- [ ] **Step 6: Commit the blog-item navigation change**

```bash
git add web/src/views/pk/PkIndexView.vue web/tests/unit/BlogListNavigation.test.mjs
git commit -m "feat: open blog details from clickable list items"
```

---

### Task 2: Regression Verification

**Files:**
- Verify only; no production files should change.

**Interfaces:**
- Consumes: completed blog-list item navigation from Task 1.
- Produces: evidence that frontend unit tests, lint, build, and diff checks pass.

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

Expected: no whitespace errors; only the planned frontend files and documentation belong to this feature, while the pre-existing backend changes remain unstaged.
