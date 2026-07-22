# Admin Management Frontend Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build an administrator-only frontend for user management, login records, and topic management, including topic creation, editing, deletion, and judge-case editing.

**Architecture:** Add pure admin-domain helpers for testable transformations, a focused Promise-based jQuery API client, three routed Vue pages, and small modal components for topic operations. Extend the existing Vuex user state and backend info response with the `root` flag, then enforce administrator access in both navigation rendering and the router guard.

**Tech Stack:** Vue 3 Composition API, Vue Router 4, Vuex 4, Bootstrap 5, jQuery Ajax, Node.js built-in test runner, Spring Boot/JUnit 5/Mockito.

## Global Constraints

- Keep the existing Vue 3, Vue Router, Vuex, jQuery Ajax, and Bootstrap stack; add no frontend dependencies.
- Treat only the exact string `"true"` as administrator access.
- Send `status=0` to ban a user and `status=1` to unban a user, matching the implemented backend behavior.
- New-topic fields start empty and the create payload must not contain `topic_id`.
- Topic deletion requires an explicit second confirmation and sends `topic_id` to `/oj/topic/remove/`.
- Do not add judge-case deletion because the backend does not support it.
- Preserve unrelated user changes and existing public problem-list behavior.

---

## File Map

- Create `web/src/utils/admin.mjs`: pure permission, record, topic-form, and evaluate-payload transformations.
- Create `web/src/api/admin.mjs`: authenticated management API calls and network-error normalization.
- Create `web/src/views/manage/ManageUsersView.vue`: user list and ban/unban workflow.
- Create `web/src/views/manage/ManageLoginRecordsView.vue`: latest-100 login record table.
- Create `web/src/views/manage/ManageTopicsView.vue`: topic list, search, and modal orchestration.
- Create `web/src/components/manage/TopicFormModal.vue`: shared empty-create and populated-edit topic form.
- Create `web/src/components/manage/EvaluateModal.vue`: judge-case editor with append behavior.
- Create `web/src/components/manage/DeleteTopicModal.vue`: destructive confirmation UI.
- Modify `web/src/store/user.js`: persist and clear `root`.
- Modify `web/src/router/index.js`: register and guard admin routes.
- Modify `web/src/components/NavBar.vue`: render the admin dropdown.
- Modify `backendcloud/backend/src/main/java/com/kob/backend/service/impl/user/account/InfoServiceImp.java`: return `root`.
- Create focused Node and JUnit tests under the existing test directories.

---

### Task 1: Admin domain helpers

**Files:**
- Create: `web/tests/unit/AdminUtils.test.mjs`
- Create: `web/src/utils/admin.mjs`

**Interfaces:**
- Produces: `isAdmin(root): boolean`
- Produces: `statusForBannedState(isBanned): 0 | 1`
- Produces: `latestLoginRecords(records, limit = 100): Array<object>`
- Produces: `emptyTopicForm(): TopicForm`
- Produces: `topicToForm(topic): TopicForm`
- Produces: `topicPayload(form, topicId?): Record<string, string>`
- Produces: `evaluatePayload(records, topicId): Array<Record<string, string>>`

- [ ] **Step 1: Write failing helper tests**

Create tests that assert exact admin-string matching, ban/unban status mapping, stable newest-first record limiting, all-empty topic defaults, snake-case topic payloads, omission of `topic_id` for creation, inclusion for editing, and omission of blank judge-case IDs.

```js
test('recognizes only the backend administrator string', () => {
  assert.equal(isAdmin('true'), true)
  assert.equal(isAdmin(true), false)
  assert.equal(isAdmin('false'), false)
})

test('maps the current state to the next backend action', () => {
  assert.equal(statusForBannedState(false), 0)
  assert.equal(statusForBannedState(true), 1)
})

test('creates empty topic values and omits topic_id for creation', () => {
  const form = emptyTopicForm()
  assert.ok(Object.values(form).every(value => value === ''))
  assert.equal('topic_id' in topicPayload(form), false)
})
```

- [ ] **Step 2: Run the helper test and verify RED**

Run: `cd web && node --test tests/unit/AdminUtils.test.mjs`

Expected: FAIL with `ERR_MODULE_NOT_FOUND` for `src/utils/admin.mjs`.

- [ ] **Step 3: Implement the pure helpers**

Use a single canonical topic field map:

```js
const TOPIC_FIELDS = [
  ['test_point', 'testPoint'], ['title', 'title'],
  ['description', 'description'], ['star', 'star'],
  ['time_limit', 'timeLimit'], ['mem_limit', 'memLimit'],
  ['input_format', 'inputFormat'], ['output_format', 'outputFormat'],
  ['sample_input', 'sampleInput'], ['sample_output', 'sampleOutput'],
  ['hint', 'hint']
]

export const isAdmin = root => root === 'true'
export const statusForBannedState = isBanned => isBanned ? 1 : 0
```

Sort valid timestamps descending without mutating the input, preserve original relative order for equal or invalid timestamps, normalize all outgoing values to strings, and include judge-case `id` only when it is non-empty.

- [ ] **Step 4: Run the helper test and verify GREEN**

Run: `cd web && node --test tests/unit/AdminUtils.test.mjs`

Expected: all `AdminUtils` tests PASS.

- [ ] **Step 5: Commit the helper boundary**

```bash
git add web/src/utils/admin.mjs web/tests/unit/AdminUtils.test.mjs
git commit -m "feat: add admin domain helpers"
```

---

### Task 2: Administrator identity in backend and Vuex

**Files:**
- Create: `backendcloud/backend/src/test/java/com/kob/backend/service/impl/user/account/InfoServiceImpTests.java`
- Modify: `backendcloud/backend/src/main/java/com/kob/backend/service/impl/user/account/InfoServiceImp.java`
- Create: `web/tests/unit/AdminIdentity.test.mjs`
- Modify: `web/src/store/user.js`

**Interfaces:**
- Backend `/user/account/info/` adds response key `root`.
- Vuex user state exposes `state.user.root` and clears it on logout.

- [ ] **Step 1: Write failing backend and store tests**

The JUnit test installs a mocked authenticated `UserDetailsImpl` in `SecurityContextHolder`, calls `getinfo()`, and asserts `response.get("root")` equals `"true"`. Clear the security context in `@AfterEach`.

The Node source test asserts `root: ""` exists in initial state, `state.root = user.root` exists in `updateUser`, and `state.root = ""` exists in logout.

- [ ] **Step 2: Verify both tests fail for the missing field**

Run: `cd backendcloud && mvn -pl backend -Dtest=InfoServiceImpTests test`

Expected: FAIL because the response has no `root` key.

Run: `cd web && node --test tests/unit/AdminIdentity.test.mjs`

Expected: FAIL because the Vuex source has no root state.

- [ ] **Step 3: Add the minimal backend and Vuex fields**

Add to the successful info response:

```java
map.put("root", user.getRoot());
```

Add `root` beside the existing username/photo fields, assign `user.root` in `updateUser`, and reset it in `logout`.

- [ ] **Step 4: Re-run focused tests**

Expected: both focused tests PASS.

- [ ] **Step 5: Commit administrator identity support**

```bash
git add backendcloud/backend/src/main/java/com/kob/backend/service/impl/user/account/InfoServiceImp.java backendcloud/backend/src/test/java/com/kob/backend/service/impl/user/account/InfoServiceImpTests.java web/src/store/user.js web/tests/unit/AdminIdentity.test.mjs
git commit -m "feat: expose administrator identity"
```

---

### Task 3: Management API client

**Files:**
- Create: `web/tests/unit/AdminApi.test.mjs`
- Create: `web/src/api/admin.mjs`

**Interfaces:**
- Produces: `adminApi.listUsers(token)`
- Produces: `adminApi.updateBanned(token, username, status)`
- Produces: `adminApi.listLoginRecords(token)`
- Produces: `adminApi.listTopics(token)`
- Produces: `adminApi.getTopic(token, id)`
- Produces: `adminApi.addTopic(token, payload)`
- Produces: `adminApi.updateTopic(token, payload)`
- Produces: `adminApi.removeTopic(token, topicId)`
- Produces: `adminApi.getEvaluates(token, topicId)`
- Produces: `adminApi.updateEvaluates(token, payload)`
- Produces: `requestErrorMessage(error): string`

- [ ] **Step 1: Write a failing API contract source test**

Read `admin.mjs` and assert every endpoint, HTTP verb, Bearer header, form parameters, and JSON settings for evaluate updates are present. Assert the error normalizer prefers `responseJSON.error_message` and distinguishes 401 from generic connection failures.

- [ ] **Step 2: Verify RED**

Run: `cd web && node --test tests/unit/AdminApi.test.mjs`

Expected: FAIL because `src/api/admin.mjs` does not exist.

- [ ] **Step 3: Implement the Promise-based client**

Wrap `$.ajax` with:

```js
const request = options => new Promise((resolve, reject) => {
  $.ajax({
    ...options,
    headers: { ...options.headers, Authorization: `Bearer ${options.token}` },
    success: resolve,
    error: reject
  })
})
```

Remove the internal `token` option before passing to jQuery. Use `contentType: 'application/json; charset=UTF-8'` and `data: JSON.stringify(payload)` only for `/manage/evaluate/updata/`; use form objects for the remaining POST/GET parameters.

- [ ] **Step 4: Verify GREEN**

Run: `cd web && node --test tests/unit/AdminApi.test.mjs`

Expected: PASS.

- [ ] **Step 5: Commit the API client**

```bash
git add web/src/api/admin.mjs web/tests/unit/AdminApi.test.mjs
git commit -m "feat: add admin API client"
```

---

### Task 4: Admin navigation and route protection

**Files:**
- Create: `web/tests/unit/AdminAccess.test.mjs`
- Modify: `web/src/router/index.js`
- Modify: `web/src/components/NavBar.vue`

**Interfaces:**
- Adds named routes `manage_users`, `manage_login_records`, and `manage_topics`.
- Adds route metadata `requiresAdmin: true`.
- Navigation renders the three matching route links only for administrators.

- [ ] **Step 1: Write failing source-level access tests**

Assert the router imports all three views, registers all paths with both auth metadata flags, checks `to.meta.requiresAdmin && !isAdmin(store.state.user.root)`, and redirects non-admin users home. Assert the navbar imports/uses `isAdmin`, contains the admin dropdown and all route names, and gates it on the computed administrator state.

- [ ] **Step 2: Verify RED**

Run: `cd web && node --test tests/unit/AdminAccess.test.mjs`

Expected: FAIL because routes and navigation do not exist.

- [ ] **Step 3: Implement routes, guard, and dropdown**

Add the routes under `/manage/`. In the guard, retain the existing login redirect first, then add:

```js
if (to.meta.requiresAdmin && !isAdmin(store.state.user.root)) {
  next({ name: 'home' })
  return
}
```

Use a computed `admin` value in `NavBar.vue`; the dropdown appears in the left navigation beside the public sections.

- [ ] **Step 4: Verify GREEN and regression tests**

Run: `cd web && npm run test:unit`

Expected: all tests PASS.

- [ ] **Step 5: Commit the access shell**

```bash
git add web/src/router/index.js web/src/components/NavBar.vue web/tests/unit/AdminAccess.test.mjs
git commit -m "feat: protect admin navigation and routes"
```

---

### Task 5: User and login-record management pages

**Files:**
- Create: `web/tests/unit/AdminListViews.test.mjs`
- Create: `web/src/views/manage/ManageUsersView.vue`
- Create: `web/src/views/manage/ManageLoginRecordsView.vue`

**Interfaces:**
- Consumes: `adminApi`, `statusForBannedState`, and `latestLoginRecords`.
- Pages expose retryable loading/error/empty/content states.

- [ ] **Step 1: Write failing view contract tests**

Assert the user view calls `listUsers`, uses `statusForBannedState`, calls `updateBanned`, disables the active row while updating, and reloads after `error_message === 'success'`. Assert the record view calls `listLoginRecords` and passes the response through `latestLoginRecords(response, 100)`.

- [ ] **Step 2: Verify RED**

Run: `cd web && node --test tests/unit/AdminListViews.test.mjs`

Expected: FAIL because both views are missing.

- [ ] **Step 3: Implement `ManageUsersView.vue`**

Use a Bootstrap card and responsive table with username, role, status, and action columns. Treat `user.banned === 'true'` as banned and `user.root === 'true'` as administrator. Await the update request, show backend business errors, and always re-enable the row in `finally`.

- [ ] **Step 4: Implement `ManageLoginRecordsView.vue`**

Use a Bootstrap card and responsive table with username, IP, and time. Fetch once on mount, normalize to an array, sort/limit through the helper, and keep retry behavior local to the page.

- [ ] **Step 5: Verify focused and full tests**

Run: `cd web && node --test tests/unit/AdminListViews.test.mjs`

Run: `cd web && npm run test:unit`

Expected: all tests PASS.

- [ ] **Step 6: Commit list pages**

```bash
git add web/src/views/manage/ManageUsersView.vue web/src/views/manage/ManageLoginRecordsView.vue web/tests/unit/AdminListViews.test.mjs
git commit -m "feat: add user and login management pages"
```

---

### Task 6: Topic creation and editing modal

**Files:**
- Create: `web/tests/unit/TopicFormModal.test.mjs`
- Create: `web/src/components/manage/TopicFormModal.vue`

**Interfaces:**
- Props: `visible`, `mode: 'create' | 'edit'`, `topicId`, `initialValue`, `loading`, `submitting`, `error`.
- Emits: `close`, `submit` with a copied form object.

- [ ] **Step 1: Write a failing modal source test**

Assert all eleven fields exist, create/edit titles are distinct, numeric fields use positive minimums, title uses `maxlength="100"`, submit is disabled while loading/submitting, errors render inside the modal, and the component emits `submit` without mutating the prop object.

- [ ] **Step 2: Verify RED**

Run: `cd web && node --test tests/unit/TopicFormModal.test.mjs`

Expected: FAIL because the modal is missing.

- [ ] **Step 3: Implement the shared form modal**

Render a Bootstrap `modal d-block` plus backdrop when visible. Clone `initialValue` whenever the modal opens or changes target. Use textareas for long-form fields and a two-column grid for numeric/short fields. Prevent close while submitting and submit only through the form event.

- [ ] **Step 4: Verify GREEN**

Run the focused test; expected PASS.

- [ ] **Step 5: Commit the topic form**

```bash
git add web/src/components/manage/TopicFormModal.vue web/tests/unit/TopicFormModal.test.mjs
git commit -m "feat: add topic form modal"
```

---

### Task 7: Judge-case and delete confirmation modals

**Files:**
- Create: `web/tests/unit/TopicActionModals.test.mjs`
- Create: `web/src/components/manage/EvaluateModal.vue`
- Create: `web/src/components/manage/DeleteTopicModal.vue`

**Interfaces:**
- Evaluate modal emits `submit` with its copied rows and supports `add-row` locally.
- Delete modal emits `confirm` only from the explicit destructive button.

- [ ] **Step 1: Write failing modal contract tests**

Assert evaluate rows contain input/output controls, an “添加判例” button appends `{ id: '', input: '', output: '' }`, empty fields are marked required, and submit/error/loading states exist. Assert delete confirmation shows both ID and title, uses danger styling, and disables close/confirm while submitting.

- [ ] **Step 2: Verify RED**

Run: `cd web && node --test tests/unit/TopicActionModals.test.mjs`

Expected: FAIL because both modals are missing.

- [ ] **Step 3: Implement both focused modals**

Clone judge-case props on open so cancelled edits never leak into the topic page. Keep existing IDs as strings, add blank rows without IDs, and make the delete modal a separate component with no API knowledge.

- [ ] **Step 4: Verify GREEN**

Run the focused test; expected PASS.

- [ ] **Step 5: Commit action modals**

```bash
git add web/src/components/manage/EvaluateModal.vue web/src/components/manage/DeleteTopicModal.vue web/tests/unit/TopicActionModals.test.mjs
git commit -m "feat: add topic action modals"
```

---

### Task 8: Topic management page integration

**Files:**
- Create: `web/tests/unit/ManageTopicsView.test.mjs`
- Create: `web/src/views/manage/ManageTopicsView.vue`

**Interfaces:**
- Consumes all topic/evaluate API functions and the three modal components.
- Uses `emptyTopicForm`, `topicToForm`, `topicPayload`, and `evaluatePayload`.

- [ ] **Step 1: Write a failing integration contract test**

Assert the page has ID/title/star columns, all three row actions, a header-level create button, title/ID filtering, and handlers for these exact flows:

```text
create -> emptyTopicForm -> addTopic -> reload
edit -> getTopic -> topicToForm -> updateTopic -> reload
evaluates -> getEvaluates -> evaluatePayload -> updateEvaluates
delete -> confirmation -> removeTopic -> reload
```

Also assert business errors remain assigned to the active modal rather than closing it.

- [ ] **Step 2: Verify RED**

Run: `cd web && node --test tests/unit/ManageTopicsView.test.mjs`

Expected: FAIL because the page is missing.

- [ ] **Step 3: Implement list and modal orchestration**

Fetch topics on mount, filter without mutating source data, and maintain separate state objects for topic form, evaluates, and delete confirmation. Create mode passes `emptyTopicForm()` immediately. Edit and evaluate modes open with loading state, then populate from their detail APIs.

On successful create/update/delete, close the relevant modal and await `loadTopics()`. On successful evaluate update, close its modal; the topic list itself is unchanged. On any business failure, set the matching modal error and leave user input intact.

- [ ] **Step 4: Verify focused and full tests**

Run: `cd web && node --test tests/unit/ManageTopicsView.test.mjs`

Run: `cd web && npm run test:unit`

Expected: all tests PASS.

- [ ] **Step 5: Commit topic management**

```bash
git add web/src/views/manage/ManageTopicsView.vue web/tests/unit/ManageTopicsView.test.mjs
git commit -m "feat: add topic management page"
```

---

### Task 9: Full verification and polish

**Files:**
- Modify only files implicated by verification failures.

**Interfaces:**
- Produces a clean frontend build and passing backend focused/full tests.

- [ ] **Step 1: Run the complete frontend test suite**

Run: `cd web && npm run test:unit`

Expected: all tests PASS with no failures.

- [ ] **Step 2: Run frontend lint**

Run: `cd web && npm run lint`

Expected: exit 0 with no lint errors.

- [ ] **Step 3: Run the production build**

Run: `cd web && npm run build`

Expected: exit 0 and a successful production build under `web/dist`.

- [ ] **Step 4: Run backend tests**

Run: `cd backendcloud && mvn -pl backend test`

Expected: all backend tests PASS.

- [ ] **Step 5: Inspect the final diff**

Run: `git status --short`, `git diff --check`, and `git diff HEAD~8 --stat` (or compare from the plan commit if fewer task commits were needed).

Expected: only planned admin/backend-info files and generated `web/dist` changes, if tracked by the repository; no whitespace errors or unrelated edits.

- [ ] **Step 6: Commit any verification-only fixes**

```bash
git add web/src/utils/admin.mjs web/src/api/admin.mjs web/src/store/user.js web/src/router/index.js web/src/components/NavBar.vue web/src/components/manage/TopicFormModal.vue web/src/components/manage/EvaluateModal.vue web/src/components/manage/DeleteTopicModal.vue web/src/views/manage/ManageUsersView.vue web/src/views/manage/ManageLoginRecordsView.vue web/src/views/manage/ManageTopicsView.vue web/tests/unit/AdminUtils.test.mjs web/tests/unit/AdminIdentity.test.mjs web/tests/unit/AdminApi.test.mjs web/tests/unit/AdminAccess.test.mjs web/tests/unit/AdminListViews.test.mjs web/tests/unit/TopicFormModal.test.mjs web/tests/unit/TopicActionModals.test.mjs web/tests/unit/ManageTopicsView.test.mjs backendcloud/backend/src/main/java/com/kob/backend/service/impl/user/account/InfoServiceImp.java backendcloud/backend/src/test/java/com/kob/backend/service/impl/user/account/InfoServiceImpTests.java
git commit -m "fix: polish admin management frontend"
```

Skip this commit when verification required no code changes.
