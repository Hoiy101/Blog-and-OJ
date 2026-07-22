import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(
  new URL('../../src/views/manage/ManageTopicsView.vue', import.meta.url),
  'utf8'
)

test('frames topic management in a spaced card', () => {
  assert.match(source, /<main class="admin-page-shell">/)
  assert.match(source, /<div class="container admin-page-container">[\s\S]*<section class="card admin-card">/)
  const shellRule = source.match(/\.admin-page-shell\s*\{([^}]*)\}/)?.[1] || ''
  assert.match(shellRule, /min-height:\s*calc\(100vh - 56px\)/)
  assert.doesNotMatch(shellRule, /background(?:-color)?\s*:/)
  assert.match(source, /\.admin-page-container\s*\{[\s\S]*padding-top:\s*1\.5rem/)
  assert.match(source, /\.admin-card\s*\{[\s\S]*border:\s*1px solid/)
})

test('renders searchable topic rows with create, edit, evaluate, and delete actions', () => {
  assert.match(source, /题库管理/)
  assert.match(source, /v-model="searchKeyword"/)
  assert.match(source, /topic\.id/)
  assert.match(source, /topic\.title/)
  assert.match(source, /topic\.star/)
  assert.match(source, /@click="openCreate"/)
  assert.match(source, /@click="openEdit\(topic\)"/)
  assert.match(source, /@click="openEvaluates\(topic\)"/)
  assert.match(source, /@click="openDelete\(topic\)"/)
})

test('creates from an empty form and reloads after success', () => {
  assert.match(source, /topicForm\.value = emptyTopicForm\(\)/)
  assert.match(source, /topicPayload\(form\)/)
  assert.match(source, /adminApi\.addTopic\(store\.state\.user\.token, payload\)/)
  assert.match(source, /await loadTopics\(\)/)
})

test('loads and updates a selected topic', () => {
  assert.match(source, /adminApi\.getTopic\(store\.state\.user\.token, topic\.id\)/)
  assert.match(source, /topicForm\.value = topicToForm\(resp\)/)
  assert.match(source, /topicPayload\(form, activeTopic\.value\.id\)/)
  assert.match(source, /adminApi\.updateTopic\(store\.state\.user\.token, payload\)/)
})

test('loads and updates judge cases with the current topic id', () => {
  assert.match(source, /adminApi\.getEvaluates\(store\.state\.user\.token, topic\.id\)/)
  assert.match(source, /evaluatePayload\(rows, activeTopic\.value\.id\)/)
  assert.match(source, /adminApi\.updateEvaluates\(store\.state\.user\.token, payload\)/)
})

test('deletes only through the confirmation modal and reloads after success', () => {
  assert.match(source, /<DeleteTopicModal/)
  assert.match(source, /@confirm="deleteTopic"/)
  assert.match(source, /adminApi\.removeTopic\(store\.state\.user\.token, activeTopic\.value\.id\)/)
})

test('keeps separate business errors for each active modal', () => {
  assert.match(source, /topicModalError/)
  assert.match(source, /evaluateModalError/)
  assert.match(source, /deleteModalError/)
  assert.match(source, /resp\.error_message !== 'success'/)
})

test('ignores stale topic and evaluate detail responses', () => {
  assert.match(source, /const topicRequestId = ref\(0\)/)
  assert.match(source, /const requestId = \+\+topicRequestId\.value/)
  assert.match(source, /if \(requestId !== topicRequestId\.value\) return/)
  assert.match(source, /const evaluateRequestId = ref\(0\)/)
  assert.match(source, /const requestId = \+\+evaluateRequestId\.value/)
  assert.match(source, /if \(requestId !== evaluateRequestId\.value\) return/)
})
