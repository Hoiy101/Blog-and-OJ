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

test('protects IME composition and stale asynchronous image work', () => {
  assert.match(source, /@compositionstart="composing = true"/)
  assert.match(source, /@compositionend="finishComposition"/)
  assert.match(source, /renderGeneration/)
  assert.match(source, /generation !== renderGeneration/)
})

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

test('removing an image reference never calls a MinIO deletion API', () => {
  assert.doesNotMatch(source, /deleteBlogImage|removeObject|image\/delete/)
})
