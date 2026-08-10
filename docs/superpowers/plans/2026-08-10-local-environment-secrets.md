# Local Environment Secrets Migration Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Persist current service connection values as local environment variables, remove their literal values from tracked configuration, and safely commit and push the full prepared change set.

**Architecture:** A mode-`600` zsh fragment outside the repository owns real values and is sourced by `~/.zshenv`. Spring Boot property files consume `BLOG_OJ_*` variables, while tracked documentation contains names only.

**Tech Stack:** zsh, Spring Boot property placeholders, Maven, Vue CLI, Git.

## Global Constraints

- Never print secret values in tool output, documentation, commits, or the final response.
- Preserve all existing staged and unstaged source changes.
- Exclude `backendcloud/globalCode/` from Git.
- Use the user-requested commit message and push the current `dxy` branch only after verification.

---

### Task 1: Persist local environment variables

**Files:**
- Create outside repository: `~/.config/blog-and-oj/env.zsh`
- Create or modify outside repository: `~/.zshenv`

- [ ] Extract current property values without logging them.
- [ ] Write shell-quoted `BLOG_OJ_*` exports with file mode `600`.
- [ ] Add an idempotent source block to `~/.zshenv`.
- [ ] Start a clean login zsh and assert every required variable is non-empty.

### Task 2: Replace tracked values and exclude runtime code

**Files:**
- Modify: `backendcloud/backend/src/main/resources/application.properties`
- Modify: `backendcloud/evaluatesystem/src/main/resources/application.properties`
- Modify: `backendcloud/.gitignore`
- Modify: `README.md`

- [ ] Verify a pre-change assertion fails because property values are still literal.
- [ ] Replace connection properties with exact `${BLOG_OJ_*}` placeholders.
- [ ] Add `/globalCode/` to the backendcloud ignore rules and unstage its generated Java file.
- [ ] Document variable names, local persistence, and validation without real values.
- [ ] Verify the placeholder and ignore assertions pass.

### Task 3: Verify, commit, and push

- [ ] Run backend Maven tests with the IntelliJ-bundled Maven executable.
- [ ] Run evaluatesystem Maven tests.
- [ ] Run frontend unit tests, lint, and production build.
- [ ] Run `git add .`, then remove ignored runtime output from the index if necessary.
- [ ] Scan the staged patch for literal sensitive values without printing them.
- [ ] Commit with `完成了布隆过滤器和判断部分的优化`.
- [ ] Push current branch `dxy` to `origin` and verify the resulting status.
