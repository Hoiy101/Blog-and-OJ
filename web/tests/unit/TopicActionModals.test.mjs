import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const evaluateSource = await readFile(
  new URL('../../src/components/manage/EvaluateModal.vue', import.meta.url),
  'utf8'
)
const deleteSource = await readFile(
  new URL('../../src/components/manage/DeleteTopicModal.vue', import.meta.url),
  'utf8'
)

test('edits copied judge cases and appends empty rows', () => {
  assert.match(evaluateSource, /rows\.value = props\.initialValue\.map\(row => \(\{ \.\.\.row/)
  assert.match(evaluateSource, /rows\.value\.push\(\{ id: '', input: '', output: '' \}\)/)
  assert.match(evaluateSource, /v-model="row\.input"/)
  assert.match(evaluateSource, /v-model="row\.output"/)
  assert.match(evaluateSource, /添加判例/)
  assert.match(evaluateSource, /required/)
  assert.match(evaluateSource, /emit\('submit', rows\.value\.map\(row => \(\{ \.\.\.row \}\)\)\)/)
})

test('keeps evaluate loading, errors, and submission state in the modal', () => {
  assert.match(evaluateSource, /v-if="loading"/)
  assert.match(evaluateSource, /v-if="error"/)
  assert.match(evaluateSource, /:disabled="submitting \|\| loading"/)
  assert.match(evaluateSource, /modal-backdrop/)
})

test('keeps actions visible while long judge-case content scrolls', () => {
  assert.match(evaluateSource, /<form class="admin-modal-form"/)
  assert.match(evaluateSource, /<div class="modal-body admin-modal-body">/)
  assert.match(evaluateSource, /\.modal-content\s*\{[\s\S]*max-height:\s*calc\(100vh - 2rem\)/)
  assert.match(evaluateSource, /\.admin-modal-form\s*\{[\s\S]*min-height:\s*0/)
  assert.match(evaluateSource, /\.admin-modal-body\s*\{[\s\S]*overflow-y:\s*auto/)
})

test('requires a dedicated destructive confirmation for topic deletion', () => {
  assert.match(deleteSource, /确认删除题目/)
  assert.match(deleteSource, /topic\.id/)
  assert.match(deleteSource, /topic\.title/)
  assert.match(deleteSource, /btn btn-danger/)
  assert.match(deleteSource, /emit\('confirm'\)/)
  assert.match(deleteSource, /:disabled="submitting"/)
  assert.match(deleteSource, /v-if="error"/)
})
