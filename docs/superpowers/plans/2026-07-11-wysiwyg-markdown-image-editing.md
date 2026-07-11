# WYSIWYG Markdown Image Editing Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace the split Ace Markdown editor/preview with one dependency-free WYSIWYG editor that persists safe Markdown, displays uploaded images inline, resizes them by dragging, and removes them with natural Backspace/Delete behavior.

**Architecture:** Keep Markdown as the database and component API format. Extend the existing safe renderer with project-local image width syntax, add focused DOM/Markdown editing utilities, and make `MarkdownEditor.vue` own a sanitized `contenteditable` surface while `MarkdownContent.vue` remains the read-only renderer and lightbox.

**Tech Stack:** Vue 3 Composition API, native `contenteditable`/Selection/Range/Pointer Events, project-local ESM utilities, Node `node:test`, existing Spring Boot/Maven backend.

## Global Constraints

- Database content remains a Markdown string; editor HTML is never stored.
- MinIO stores Markdown images only; deleting an image reference must not delete the MinIO object.
- Do not add npm dependencies and do not change the existing hard-coded MinIO configuration.
- Persist resized images as `![alt](url){width=N%}` with integer `N` from 10 through 100.
- Images without valid width metadata render at 25%.
- Editing is purely WYSIWYG: no Ace source view and no edit/preview tabs.
- Upload is disabled without `blogId` and retains the text `先创建博客，再修改文章上传图片`.
- Read-only blog images retain placeholder-first loading, failure fallback, request caching, race protection, and the overlay lightbox.
- Implement every behavior test-first and preserve unrelated dirty-worktree changes.

## File Structure

- Create `web/src/utils/wysiwygMarkdown.mjs`: width normalization/calculation, editor DOM serialization, safe paste normalization, and image-adjacency helpers.
- Create `web/tests/unit/wysiwygMarkdown.test.mjs`: unit coverage for all pure editor rules using small DOM-shaped test nodes.
- Create `web/tests/unit/WysiwygMarkdownEditor.test.mjs`: source-level component contract coverage that does not require adding a Vue test dependency.
- Modify `web/src/utils/markdown.mjs`: parse validated `{width=N%}` image metadata and emit safe image width attributes.
- Modify `web/tests/unit/markdown.test.mjs`: renderer compatibility and width security tests.
- Modify `web/src/components/MarkdownEditor.vue`: replace Ace with the contenteditable editor and implement upload, formatting, paste, image selection, resize, and keyboard deletion.
- Modify `web/src/components/MarkdownContent.vue`: make the default 25% CSS compatible with per-image validated width.
- Modify `web/src/utils/markdownEditing.mjs`: emit optional width metadata from the shared image Markdown helper.
- Modify `web/tests/unit/markdownEditing.test.mjs`: preserve standard syntax at 25% and persist non-default widths.
- Keep existing package metadata unchanged; unused Ace packages can be removed in a separate cleanup because `web/package-lock.json` already contains unrelated user changes.
- Do not modify backend production behavior; run its full suite as a regression gate.

---

### Task 1: Persist and render validated image widths

**Files:**
- Modify: `web/src/utils/markdown.mjs`
- Modify: `web/src/utils/markdownEditing.mjs`
- Modify: `web/src/components/MarkdownContent.vue`
- Modify: `web/tests/unit/markdown.test.mjs`
- Modify: `web/tests/unit/markdownEditing.test.mjs`

**Interfaces:**
- Produces: `normalizeImageWidth(value, fallback = 25): number` exported from `markdown.mjs`.
- Produces: `imageMarkdown(alt, url, width = 25): string` from `markdownEditing.mjs`; width 25 omits extension, other valid widths append `{width=N%}`.
- Produces: rendered images with `data-image-width="N"` and inline custom property `--markdown-image-width:N%`.

- [ ] **Step 1: Write failing renderer and syntax tests**

Add to `web/tests/unit/markdown.test.mjs`:

```js
import { normalizeImageWidth, renderMarkdown } from '../../src/utils/markdown.mjs'

test('renders persisted image width and defaults invalid widths to 25 percent', () => {
  const resized = renderMarkdown('![图](https://a.test/a.png){width=40%}')
  assert.match(resized, /data-image-width="40"/)
  assert.match(resized, /--markdown-image-width:40%/)

  const unsafe = renderMarkdown('![图](https://a.test/a.png){width=999%}')
  assert.match(unsafe, /data-image-width="25"/)
  assert.doesNotMatch(unsafe, /999/)
  assert.equal(normalizeImageWidth('10'), 10)
  assert.equal(normalizeImageWidth('100'), 100)
  assert.equal(normalizeImageWidth('9'), 25)
  assert.equal(normalizeImageWidth('calc(100)'), 25)
})
```

Add to `web/tests/unit/markdownEditing.test.mjs`:

```js
test('only appends width metadata for a non-default image width', () => {
  assert.equal(imageMarkdown('图', 'https://a.test/a.png', 25), '\n![图](https://a.test/a.png)\n')
  assert.equal(imageMarkdown('图', 'https://a.test/a.png', 40), '\n![图](https://a.test/a.png){width=40%}\n')
})
```

- [ ] **Step 2: Run tests and verify RED**

Run: `cd web && node --test tests/unit/markdown.test.mjs tests/unit/markdownEditing.test.mjs`

Expected: FAIL because `normalizeImageWidth` is not exported and width metadata is not parsed.

- [ ] **Step 3: Implement width normalization and rendering**

In `web/src/utils/markdown.mjs`, add:

```js
export function normalizeImageWidth(value, fallback = 25) {
  const text = String(value ?? '').trim()
  if (!/^\d{1,3}$/.test(text)) return fallback
  const width = Number(text)
  return width >= 10 && width <= 100 ? width : fallback
}
```

Extend only the image token expression so the width suffix is consumed with the image instead of appearing as text:

```js
text = text.replace(
  /!\[([^\]]*)\]\(([^\s)]+)(?:\s+[&quot;][^&quot;]*[&quot;])?\)(?:\{width=([^}%]+)%\})?/g,
  (_, alt, rawUrl, rawWidth) => {
    const url = safeUrl(rawUrl)
    if (!url) return alt
    const width = normalizeImageWidth(rawWidth, 25)
    const imageAttributes = `data-image-width="${width}" style="--markdown-image-width:${width}%"`
    const placeholder = safeUrl(options.imagePlaceholder)
    return keep(placeholder
      ? `<img src="${escapeHtml(placeholder)}" data-original-src="${escapeHtml(url)}" data-image-state="loading" ${imageAttributes} alt="${alt}" loading="lazy">`
      : `<img src="${escapeHtml(url)}" ${imageAttributes} alt="${alt}" loading="lazy">`)
  }
)
```

In `web/src/utils/markdownEditing.mjs`, replace the helper with:

```js
import { normalizeImageWidth } from './markdown.mjs'

export const imageMarkdown = (alt, url, width = 25) => {
  const normalized = normalizeImageWidth(width)
  const suffix = normalized === 25 ? '' : `{width=${normalized}%}`
  return `\n![${alt || '图片'}](${url})${suffix}\n`
}
```

In `MarkdownContent.vue`, replace the fixed width rule with:

```css
.markdown-content img {
  display: block;
  width: var(--markdown-image-width, 25%);
  max-width: 100%;
  height: auto;
  margin: 1rem auto;
  cursor: zoom-in;
  border-radius: 4px;
}
```

- [ ] **Step 4: Run focused and full frontend tests**

Run: `cd web && node --test tests/unit/markdown.test.mjs tests/unit/markdownEditing.test.mjs && npm run test:unit`

Expected: all tests PASS; no literal `{width=...}` remains visible after rendering.

- [ ] **Step 5: Commit the width syntax slice**

```bash
git add web/src/utils/markdown.mjs web/src/utils/markdownEditing.mjs web/src/components/MarkdownContent.vue web/tests/unit/markdown.test.mjs web/tests/unit/markdownEditing.test.mjs
git commit -m "feat: persist markdown image widths"
```

---

### Task 2: Add focused WYSIWYG Markdown utilities

**Files:**
- Create: `web/src/utils/wysiwygMarkdown.mjs`
- Create: `web/tests/unit/wysiwygMarkdown.test.mjs`

**Interfaces:**
- Consumes: `normalizeImageWidth` and `imageMarkdown` from Task 1.
- Produces: `calculateImageWidth(pointerX, containerLeft, containerWidth): number`.
- Produces: `serializeEditorNode(node): string` and `serializeEditor(root): string`.
- Produces: `adjacentImageFromSelection(selection, direction): Element|null` where direction is `'backward'` or `'forward'`.
- Produces: `sanitizeEditorFragment(root, documentRef): DocumentFragment` containing only the allowlisted editable structure.

- [ ] **Step 1: Write failing pure utility tests**

Create `web/tests/unit/wysiwygMarkdown.test.mjs` with lightweight node builders:

```js
import test from 'node:test'
import assert from 'node:assert/strict'
import {
  calculateImageWidth,
  serializeEditorNode,
  serializeEditor,
  adjacentImageFromSelection
} from '../../src/utils/wysiwygMarkdown.mjs'

const text = data => ({ nodeType: 3, textContent: data })
const element = (tagName, children = [], attrs = {}) => ({
  nodeType: 1,
  tagName: tagName.toUpperCase(),
  childNodes: children,
  children,
  dataset: attrs.dataset || {},
  getAttribute: name => attrs[name] ?? null,
  matches: selector => selector === 'img[data-original-src]' && tagName === 'img' && Boolean(attrs.dataset?.originalSrc)
})

test('clamps drag width to the supported integer percentage', () => {
  assert.equal(calculateImageWidth(140, 100, 400), 10)
  assert.equal(calculateImageWidth(260, 100, 400), 40)
  assert.equal(calculateImageWidth(700, 100, 400), 100)
})

test('serializes supported DOM and ignores editor-only resize handles', () => {
  const image = element('img', [], {
    alt: '图',
    dataset: { originalSrc: 'https://a.test/a.png', imageWidth: '40' }
  })
  const handle = element('span', [], { dataset: { editorUi: 'resize-handle' } })
  const root = element('div', [element('h2', [text('标题')]), element('p', [text('正文')]), image, handle])
  assert.equal(serializeEditor(root), '## 标题\n\n正文\n\n![图](https://a.test/a.png){width=40%}')
})

test('finds an image immediately before a collapsed caret', () => {
  const image = element('img', [], { dataset: { originalSrc: 'https://a.test/a.png' } })
  const parent = element('p', [image, text('')])
  parent.childNodes[0].parentNode = parent
  const selection = { isCollapsed: true, anchorNode: parent, anchorOffset: 1 }
  assert.equal(adjacentImageFromSelection(selection, 'backward'), image)
})
```

- [ ] **Step 2: Run test and verify RED**

Run: `cd web && node --test tests/unit/wysiwygMarkdown.test.mjs`

Expected: FAIL with `ERR_MODULE_NOT_FOUND` for `wysiwygMarkdown.mjs`.

- [ ] **Step 3: Implement the utility module**

Create `web/src/utils/wysiwygMarkdown.mjs` with these exact top-level rules:

```js
import { normalizeImageWidth } from './markdown.mjs'
import { imageMarkdown } from './markdownEditing.mjs'

const NODE_TEXT = 3
const NODE_ELEMENT = 1
const blockGap = value => String(value || '').replace(/\n{3,}/g, '\n\n').trim()

export function calculateImageWidth(pointerX, containerLeft, containerWidth) {
  if (!Number.isFinite(containerWidth) || containerWidth <= 0) return 25
  const raw = Math.round(((pointerX - containerLeft) / containerWidth) * 100)
  return Math.min(100, Math.max(10, raw))
}

const childrenMarkdown = node => [...(node.childNodes || [])].map(serializeEditorNode).join('')

export function serializeEditorNode(node) {
  if (!node) return ''
  if (node.nodeType === NODE_TEXT) return node.textContent || ''
  if (node.nodeType !== NODE_ELEMENT) return ''
  if (node.dataset?.editorUi) return ''
  const tag = String(node.tagName || '').toLowerCase()
  const inner = childrenMarkdown(node)
  if (tag === 'img') {
    const url = node.dataset?.originalSrc || node.getAttribute?.('src') || ''
    const alt = node.getAttribute?.('alt') || '图片'
    return imageMarkdown(alt, url, normalizeImageWidth(node.dataset?.imageWidth)).trim()
  }
  if (tag === 'br') return '\n'
  if (tag === 'strong' || tag === 'b') return `**${inner}**`
  if (tag === 'em' || tag === 'i') return `*${inner}*`
  if (tag === 'del' || tag === 's') return `~~${inner}~~`
  if (tag === 'code' && node.parentNode?.tagName !== 'PRE') return `\`${inner}\``
  if (tag === 'a') return `[${inner}](${node.getAttribute?.('href') || ''})`
  if (/^h[1-6]$/.test(tag)) return `${'#'.repeat(Number(tag[1]))} ${inner}\n\n`
  if (tag === 'blockquote') return `${inner.split('\n').filter(Boolean).map(line => `> ${line}`).join('\n')}\n\n`
  if (tag === 'li') return `${inner}\n`
  if (tag === 'ul') return `${inner.split('\n').filter(Boolean).map(line => `- ${line}`).join('\n')}\n\n`
  if (tag === 'ol') return `${inner.split('\n').filter(Boolean).map((line, index) => `${index + 1}. ${line}`).join('\n')}\n\n`
  if (tag === 'pre') return `\`\`\`\n${node.textContent || ''}\n\`\`\`\n\n`
  if (tag === 'p' || tag === 'div') return `${inner}\n\n`
  return inner
}

export function serializeEditor(root) {
  return blockGap([...(root?.childNodes || [])].map(serializeEditorNode).join(''))
}

export function adjacentImageFromSelection(selection, direction) {
  if (!selection?.isCollapsed) return null
  const container = selection.anchorNode
  const offset = selection.anchorOffset
  const nodes = container?.childNodes || []
  const candidate = direction === 'backward' ? nodes[offset - 1] : nodes[offset]
  return candidate?.matches?.('img[data-original-src]') ? candidate : null
}
```

Add `sanitizeEditorFragment(root, documentRef)` in the same file. It must recursively create new nodes rather than mutating/preserving pasted nodes; allow `P`, `BR`, `H1`–`H6`, `UL`, `OL`, `LI`, `BLOCKQUOTE`, `STRONG`, `EM`, `DEL`, `A`, `CODE`, `PRE`, `TABLE`, `THEAD`, `TBODY`, `TR`, `TH`, `TD`, and `IMG`; keep only safe `href`, safe image original URL/alt, and normalized image width. Unknown elements contribute only sanitized descendants; `SCRIPT`, `STYLE`, `IFRAME`, and `OBJECT` contribute nothing.

Use this implementation so pasted nodes and attributes are never reused:

```js
const ALLOWED_TAGS = new Set([
  'P', 'BR', 'H1', 'H2', 'H3', 'H4', 'H5', 'H6', 'UL', 'OL', 'LI',
  'BLOCKQUOTE', 'STRONG', 'EM', 'DEL', 'A', 'CODE', 'PRE', 'TABLE',
  'THEAD', 'TBODY', 'TR', 'TH', 'TD', 'IMG'
])
const DROPPED_TAGS = new Set(['SCRIPT', 'STYLE', 'IFRAME', 'OBJECT'])
const isSafeEditorUrl = value => /^(https?:\/\/|\/|\.\.?\/|#)/i.test(String(value || '').trim())

const sanitizeNode = (node, documentRef) => {
  if (node.nodeType === NODE_TEXT) return documentRef.createTextNode(node.textContent || '')
  const fragment = documentRef.createDocumentFragment()
  if (node.nodeType !== NODE_ELEMENT || DROPPED_TAGS.has(node.tagName)) return fragment
  const children = [...(node.childNodes || [])].map(child => sanitizeNode(child, documentRef))
  if (!ALLOWED_TAGS.has(node.tagName)) {
    children.forEach(child => fragment.appendChild(child))
    return fragment
  }
  const clean = documentRef.createElement(node.tagName.toLowerCase())
  if (node.tagName === 'A') {
    const href = node.getAttribute('href') || ''
    if (!isSafeEditorUrl(href)) {
      children.forEach(child => fragment.appendChild(child))
      return fragment
    }
    clean.setAttribute('href', href)
  }
  if (node.tagName === 'IMG') {
    const original = node.dataset?.originalSrc || node.getAttribute('src') || ''
    if (!isSafeEditorUrl(original)) return fragment
    const width = normalizeImageWidth(node.dataset?.imageWidth)
    clean.setAttribute('src', original)
    clean.setAttribute('alt', node.getAttribute('alt') || '图片')
    clean.dataset.originalSrc = original
    clean.dataset.imageWidth = String(width)
    clean.style.setProperty('--markdown-image-width', `${width}%`)
  }
  children.forEach(child => clean.appendChild(child))
  return clean
}

export function sanitizeEditorFragment(root, documentRef) {
  const fragment = documentRef.createDocumentFragment()
  ;[...(root?.childNodes || [])].forEach(node => fragment.appendChild(sanitizeNode(node, documentRef)))
  return fragment
}
```

- [ ] **Step 4: Expand tests for sanitizer and remaining structures**

Use DOM-shaped fixtures to assert that event attributes and editor UI nodes do not serialize, unsafe links serialize as their text only, supported lists/code/tables retain Markdown meaning, and invalid image widths serialize at 25% without a suffix.

Run: `cd web && node --test tests/unit/wysiwygMarkdown.test.mjs`

Expected: all WYSIWYG utility tests PASS.

- [ ] **Step 5: Run all frontend unit tests and commit**

```bash
cd web && npm run test:unit
git add web/src/utils/wysiwygMarkdown.mjs web/tests/unit/wysiwygMarkdown.test.mjs
git commit -m "feat: add WYSIWYG markdown utilities"
```

Expected: all tests PASS and the commit contains only the new utility slice.

---

### Task 3: Replace Ace with a safe contenteditable editor

**Files:**
- Modify: `web/src/components/MarkdownEditor.vue`
- Create: `web/tests/unit/WysiwygMarkdownEditor.test.mjs`

**Interfaces:**
- Consumes: `renderMarkdown`, `serializeEditor`, `sanitizeEditorFragment`, `uploadBlogImage`, and `imageMarkdown`.
- Produces: unchanged Vue API: props `modelValue`, `blogId`; event `update:modelValue`.
- Produces: editable images carrying `data-original-src`, `data-image-width`, and placeholder-first `src`.

- [ ] **Step 1: Write a failing component contract test**

Create `web/tests/unit/WysiwygMarkdownEditor.test.mjs`:

```js
import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(new URL('../../src/components/MarkdownEditor.vue', import.meta.url), 'utf8')

test('uses one contenteditable WYSIWYG surface without Ace or preview tabs', () => {
  assert.match(source, /contenteditable="true"/)
  assert.doesNotMatch(source, /VAceEditor|vue3-ace-editor|tab === 'preview'/)
  assert.match(source, /@paste="handlePaste"/)
  assert.match(source, /@keydown="handleKeydown"/)
})

test('retains upload guard and emits markdown model updates', () => {
  assert.match(source, /:disabled="!blogId \|\| uploading"/)
  assert.match(source, /先创建博客，再修改文章上传图片/)
  assert.match(source, /emit\('update:modelValue'/)
})
```

- [ ] **Step 2: Run the contract test and verify RED**

Run: `cd web && node --test tests/unit/WysiwygMarkdownEditor.test.mjs`

Expected: FAIL because the current component still contains `VAceEditor` and preview tabs.

- [ ] **Step 3: Replace the template with one editable surface**

Use this structure in `MarkdownEditor.vue`:

```vue
<div class="markdown-editor">
  <div class="markdown-editor-toolbar">
    <button type="button" class="btn btn-sm btn-outline-secondary" @mousedown.prevent="formatBlock('strong')">粗体</button>
    <button type="button" class="btn btn-sm btn-outline-secondary" @mousedown.prevent="formatBlock('h2')">标题</button>
    <button type="button" class="btn btn-sm btn-outline-secondary" @mousedown.prevent="createLink">链接</button>
    <button type="button" class="btn btn-sm btn-outline-primary" :disabled="!blogId || uploading" @click="$refs.file.click()">
      {{ uploading ? '上传中…' : '上传图片' }}
    </button>
    <input ref="file" hidden type="file" accept="image/png,image/jpeg,image/webp,image/gif" @change="upload">
    <small v-if="!blogId" class="text-muted">先创建博客，再修改文章上传图片</small>
    <span v-if="error" class="text-danger">{{ error }}</span>
  </div>
  <div
    ref="editorRoot"
    class="wysiwyg-editor markdown-content"
    contenteditable="true"
    role="textbox"
    aria-multiline="true"
    @input="syncMarkdown"
    @paste="handlePaste"
    @keydown="handleKeydown"
    @compositionstart="composing = true"
    @compositionend="finishComposition"
  ></div>
</div>
```

- [ ] **Step 4: Implement one-way local DOM ownership and safe synchronization**

In the component setup:

```js
const editorRoot = ref(null)
const composing = ref(false)
let emittedMarkdown = null
let renderGeneration = 0

const renderExternalMarkdown = async markdown => {
  const generation = ++renderGeneration
  await nextTick()
  if (generation !== renderGeneration || !editorRoot.value) return
  editorRoot.value.innerHTML = renderMarkdown(markdown, { imagePlaceholder: violationImage })
  await hydrateMarkdownImages(editorRoot.value, () => generation === renderGeneration)
}

watch(() => props.modelValue, value => {
  if (value === emittedMarkdown) {
    emittedMarkdown = null
    return
  }
  renderExternalMarkdown(value)
}, { immediate: true })

const syncMarkdown = () => {
  if (composing.value || !editorRoot.value) return
  const markdown = serializeEditor(editorRoot.value)
  emittedMarkdown = markdown
  emit('update:modelValue', markdown)
}
```

Implement toolbar formatting with an allowlisted wrapper and current Range:

```js
const wrapSelection = tagName => {
  const selection = window.getSelection()
  if (!selection?.rangeCount || !editorRoot.value?.contains(selection.anchorNode)) return
  const range = selection.getRangeAt(0)
  const wrapper = document.createElement(tagName)
  if (range.collapsed) wrapper.appendChild(document.createTextNode(tagName === 'h2' ? '标题' : '文本'))
  else wrapper.appendChild(range.extractContents())
  range.insertNode(wrapper)
  selection.removeAllRanges()
  const next = document.createRange()
  next.selectNodeContents(wrapper)
  next.collapse(false)
  selection.addRange(next)
  syncMarkdown()
}
const formatBlock = kind => wrapSelection(kind === 'strong' ? 'strong' : 'h2')
```

`createLink` obtains an `https://` URL through a small toolbar input (not `prompt`), accepts it only when `/^(https?:\/\/|\/|\.\.?\/|#)/i` matches, wraps the selection in an `a` created by `document.createElement`, and calls `syncMarkdown`. `handlePaste` prevents default, parses clipboard `text/html` into a detached template, calls `sanitizeEditorFragment(template.content, document)`, and inserts the returned fragment with the current Range. If HTML is absent, split clipboard `text/plain` on newlines, insert text nodes separated by `br`, then call `syncMarkdown`.

- [ ] **Step 5: Insert uploaded images at the saved caret**

Save a cloned Range whenever selection changes inside `editorRoot`. Before awaiting the upload, preserve that range. On success, create an image element with:

```js
image.src = violationImage
image.alt = file.name.replace(/\.[^.]+$/, '') || '图片'
image.dataset.originalSrc = url
image.dataset.imageWidth = '25'
image.style.setProperty('--markdown-image-width', '25%')
```

Insert it at the preserved range, add a trailing text node or paragraph so the caret can move after it, preload the original through `loadImageUrl`, swap `src` only if the component generation is current, move the caret after the image, then call `syncMarkdown()`. If no saved range belongs to this editor, append the image to the editor root.

- [ ] **Step 6: Confirm Ace is no longer used by the editor**

Run: `rg -n "vue3-ace-editor|ace-builds|VAceEditor" web/src web/tests`

Expected: no result under `web/src` or `web/tests`. Keep dependency metadata unchanged in this feature to preserve the pre-existing `package-lock.json` modification; dependency cleanup is outside scope.

- [ ] **Step 7: Run component contract, unit, lint, and build checks**

```bash
cd web
node --test tests/unit/WysiwygMarkdownEditor.test.mjs
npm run test:unit
npm run lint
npm run build
```

Expected: contract and unit tests PASS; lint reports no errors; production build succeeds with only the repository's existing bundle-size/browser-data warnings.

- [ ] **Step 8: Commit the WYSIWYG editor base**

```bash
git add web/src/components/MarkdownEditor.vue web/tests/unit/WysiwygMarkdownEditor.test.mjs
git commit -m "feat: replace markdown source editor with WYSIWYG"
```

---

### Task 4: Add image selection, drag resizing, and adjacent-key deletion

**Files:**
- Modify: `web/src/components/MarkdownEditor.vue`
- Modify: `web/src/utils/wysiwygMarkdown.mjs`
- Modify: `web/tests/unit/wysiwygMarkdown.test.mjs`
- Modify: `web/tests/unit/WysiwygMarkdownEditor.test.mjs`

**Interfaces:**
- Consumes: `calculateImageWidth`, `adjacentImageFromSelection`, and `serializeEditor`.
- Produces: editor-only resize UI marked `data-editor-ui`, excluded from serialization.
- Produces: pointer handlers `beginResize`, `continueResize`, and `finishResize`.
- Produces: keyboard behavior for selected and adjacent images.

- [ ] **Step 1: Add failing component interaction contract tests**

Append to `WysiwygMarkdownEditor.test.mjs`:

```js
test('supports image selection resizing and whole-image keyboard deletion', () => {
  assert.match(source, /@click="handleEditorClick"/)
  assert.match(source, /beginResize/)
  assert.match(source, /continueResize/)
  assert.match(source, /finishResize/)
  assert.match(source, /adjacentImageFromSelection/)
  assert.match(source, /event\.key === 'Backspace'/)
  assert.match(source, /event\.key === 'Delete'/)
  assert.match(source, /data-editor-ui/)
})
```

Add focused pure tests verifying `calculateImageWidth` returns 10/40/100 at the boundary inputs and `adjacentImageFromSelection` returns only an immediately adjacent image for backward/forward directions.

- [ ] **Step 2: Run tests and verify RED**

Run: `cd web && node --test tests/unit/wysiwygMarkdown.test.mjs tests/unit/WysiwygMarkdownEditor.test.mjs`

Expected: FAIL because resize handlers and editor UI do not exist.

- [ ] **Step 3: Implement image selection UI**

On image click inside the editor:

- Prevent navigation/lightbox behavior.
- Clear the previous selection.
- Add `.is-selected` to the image.
- Insert a sibling overlay with `data-editor-ui="image-controls"` and left/right buttons carrying `data-editor-ui="resize-handle"`.
- Store the selected image in a ref.
- On click outside the selected image/controls or Escape, remove the controls and selection class.

CSS must keep the image centered, use `width: var(--markdown-image-width, 25%)`, show a visible focus outline, and place handles on the horizontal image edges without contributing to layout or serialization.

- [ ] **Step 4: Implement pointer resizing**

`beginResize(event, side)` records the selected image, editor rectangle, pointer id, and side. Register pointermove/pointerup on `window` for the duration of the drag. For a right handle use the current pointer X; for a left handle mirror the pointer relative to the editor right edge. Each move calls `calculateImageWidth`, then updates:

```js
image.dataset.imageWidth = String(width)
image.style.setProperty('--markdown-image-width', `${width}%`)
```

`finishResize` removes global listeners, releases pointer capture if held, calls `syncMarkdown()` exactly once, and keeps the image selected. `onBeforeUnmount` must also remove any active global listeners.

- [ ] **Step 5: Implement selected and adjacent keyboard deletion**

In `handleKeydown(event)`:

```js
const deletingBackward = event.key === 'Backspace'
const deletingForward = event.key === 'Delete'
if (!deletingBackward && !deletingForward) return

const selection = window.getSelection()
const image = selectedImage.value || adjacentImageFromSelection(
  selection,
  deletingBackward ? 'backward' : 'forward'
)
if (!image || !editorRoot.value.contains(image)) return

event.preventDefault()
const caretAnchor = image.previousSibling || image.parentNode
clearImageSelection()
image.remove()
placeCaretNear(caretAnchor, deletingBackward ? 'after' : 'before')
syncMarkdown()
```

`placeCaretNear` must create a collapsed Range inside the editor even when deleting the only content node. This operation removes the DOM image/Markdown reference only and must not call any MinIO API.

- [ ] **Step 6: Run focused and complete frontend verification**

```bash
cd web
node --test tests/unit/wysiwygMarkdown.test.mjs tests/unit/WysiwygMarkdownEditor.test.mjs
npm run test:unit
npm run lint
npm run build
```

Expected: all tests PASS, lint is clean, and build succeeds.

- [ ] **Step 7: Commit image editing behavior**

```bash
git add web/src/components/MarkdownEditor.vue web/src/utils/wysiwygMarkdown.mjs web/tests/unit/wysiwygMarkdown.test.mjs web/tests/unit/WysiwygMarkdownEditor.test.mjs
git commit -m "feat: resize and delete WYSIWYG blog images"
```

---

### Task 5: Integration regression and final review

**Files:**
- Modify only files required by failures discovered in this task.
- Review: all files changed since `db456f2` plus the pre-existing uncommitted Markdown/MinIO work.

**Interfaces:**
- Verifies the unchanged backend upload API and blog Markdown API integrate with the new editor.
- Verifies the final editor continues to emit Markdown understood by the read-only renderer.

- [ ] **Step 1: Run fresh frontend verification**

```bash
cd web
npm run test:unit
npm run lint
npm run build
```

Expected: every frontend unit test passes; lint has zero errors; build exits 0. Existing bundle-size and stale browser-data warnings are acceptable.

- [ ] **Step 2: Run fresh backend verification**

```bash
"/Applications/IntelliJ IDEA.app/Contents/plugins/maven/lib/maven3/bin/mvn" -f backendcloud/backend/pom.xml test
```

Expected: all backend tests pass. The sandbox may log an expected RabbitMQ connection warning while `BackendApplicationTests` still passes.

- [ ] **Step 3: Verify assets, formatting, and scope**

```bash
shasum -a 256 /Users/mac/Desktop/violation-del.png web/src/assets/images/violation-del.png
git diff --check
git status --short
```

Expected: both image hashes match; `git diff --check` is silent; unrelated `.DS_Store` changes remain untouched.

- [ ] **Step 4: Perform independent code review**

Request review against `docs/superpowers/specs/2026-07-11-wysiwyg-markdown-image-editing-design.md`. The reviewer must specifically check:

- DOM/Markdown round-trip data loss.
- unsafe URL/HTML/style injection.
- IME and local `v-model` synchronization loops.
- stale upload/image-loading callbacks.
- pointer listener cleanup.
- adjacent caret deletion accuracy.
- undo/redo behavior and no MinIO deletion call.

Fix every Critical or Important finding test-first, then rerun Steps 1–3.

- [ ] **Step 5: Commit any review fixes separately**

```bash
git add web/src/components/MarkdownEditor.vue web/src/components/MarkdownContent.vue web/src/utils/markdown.mjs web/src/utils/markdownEditing.mjs web/src/utils/wysiwygMarkdown.mjs web/tests/unit/markdown.test.mjs web/tests/unit/markdownEditing.test.mjs web/tests/unit/wysiwygMarkdown.test.mjs web/tests/unit/WysiwygMarkdownEditor.test.mjs
git commit -m "fix: harden WYSIWYG markdown image editing"
```

If review finds no issue, do not create an empty commit.

- [ ] **Step 6: Report completion without pushing**

Summarize behavior, test counts, lint/build results, known non-blocking warnings, and preserved unrelated changes. Do not push or create a pull request unless the user explicitly asks.
