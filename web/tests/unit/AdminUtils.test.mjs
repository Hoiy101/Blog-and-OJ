import test from 'node:test'
import assert from 'node:assert/strict'
import {
  emptyTopicForm,
  evaluatePayload,
  isAdmin,
  latestLoginRecords,
  statusForBannedState,
  topicPayload,
  topicToForm
} from '../../src/utils/admin.mjs'

test('recognizes only the backend administrator string', () => {
  assert.equal(isAdmin('true'), true)
  assert.equal(isAdmin(true), false)
  assert.equal(isAdmin('false'), false)
  assert.equal(isAdmin(undefined), false)
})

test('maps the current banned state to the next backend action', () => {
  assert.equal(statusForBannedState(false), 0)
  assert.equal(statusForBannedState(true), 1)
})

test('returns the latest one hundred login records without mutating input', () => {
  const records = Array.from({ length: 105 }, (_, index) => ({
    id: index,
    time: new Date(2026, 0, 1, 0, index).toISOString()
  }))
  const original = [...records]

  const result = latestLoginRecords(records)

  assert.equal(result.length, 100)
  assert.equal(result[0].id, 104)
  assert.equal(result[99].id, 5)
  assert.deepEqual(records, original)
})

test('keeps invalid login times stable after valid records', () => {
  const records = [
    { id: 1, time: 'invalid-a' },
    { id: 2, time: '2026-07-22T10:00:00+08:00' },
    { id: 3, time: 'invalid-b' },
    { id: 4, time: '2026-07-22T11:00:00+08:00' }
  ]

  assert.deepEqual(latestLoginRecords(records).map(record => record.id), [4, 2, 1, 3])
})

test('creates a topic form with every field empty', () => {
  assert.deepEqual(emptyTopicForm(), {
    test_point: '',
    title: '',
    description: '',
    star: '',
    time_limit: '',
    mem_limit: '',
    input_format: '',
    output_format: '',
    sample_input: '',
    sample_output: '',
    hint: ''
  })
})

test('normalizes snake-case or camel-case topic details into the edit form', () => {
  assert.deepEqual(topicToForm({
    test_point: 3,
    title: 'A + B',
    description: 'desc',
    star: 2,
    timeLimit: 1,
    memLimit: 128,
    input_format: 'in',
    outputFormat: 'out',
    sample_input: '1 2',
    sampleOutput: '3',
    hint: null
  }), {
    test_point: '3',
    title: 'A + B',
    description: 'desc',
    star: '2',
    time_limit: '1',
    mem_limit: '128',
    input_format: 'in',
    output_format: 'out',
    sample_input: '1 2',
    sample_output: '3',
    hint: ''
  })
})

test('omits topic_id for creation and includes it for editing', () => {
  const form = { ...emptyTopicForm(), title: 'Two Sum', test_point: 2 }

  assert.equal('topic_id' in topicPayload(form), false)
  assert.deepEqual(topicPayload(form, 18), {
    ...emptyTopicForm(),
    title: 'Two Sum',
    test_point: '2',
    topic_id: '18'
  })
})

test('builds evaluate payloads and omits empty ids from new rows', () => {
  assert.deepEqual(evaluatePayload([
    { id: 7, input: '1 2', output: '3' },
    { id: '', input: '4 5', output: '9' }
  ], 12), [
    { id: '7', topic_id: '12', input: '1 2', output: '3' },
    { topic_id: '12', input: '4 5', output: '9' }
  ])
})
