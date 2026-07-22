import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(
  new URL('../../src/components/manage/TopicFormModal.vue', import.meta.url),
  'utf8'
)

test('renders every topic field in the shared form', () => {
  for (const field of [
    'test_point', 'title', 'description', 'star', 'time_limit', 'mem_limit',
    'input_format', 'output_format', 'sample_input', 'sample_output', 'hint'
  ]) {
    assert.match(source, new RegExp(`v-model="form\\.${field}"`))
  }
})

test('distinguishes create and edit modes with required validation', () => {
  assert.match(source, /mode === 'create' \? '新增题目' : '修改题目'/)
  assert.match(source, /maxlength="100"/)
  assert.match(source, /v-model="form\.test_point"[\s\S]*min="1"/)
  assert.match(source, /v-model="form\.time_limit"[\s\S]*min="1"/)
  assert.match(source, /v-model="form\.mem_limit"[\s\S]*min="1"/)
  assert.match(source, /required/)
})

test('copies initial values and emits a copied form on submit', () => {
  assert.match(source, /form\.value = \{ \.\.\.emptyTopicForm\(\), \.\.\.props\.initialValue \}/)
  assert.match(source, /emit\('submit', \{ \.\.\.form\.value \}\)/)
  assert.match(source, /watch\([\s\S]*props\.visible/)
})

test('keeps loading, submitting, and errors inside the modal', () => {
  assert.match(source, /v-if="loading"/)
  assert.match(source, /v-if="error"/)
  assert.match(source, /:disabled="submitting \|\| loading"/)
  assert.match(source, /modal d-block/)
  assert.match(source, /modal-backdrop/)
})
