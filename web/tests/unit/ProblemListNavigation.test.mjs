import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(
  new URL('../../src/views/ranklist/RanKlistIndexView.vue', import.meta.url),
  'utf8'
)

test('removes problem add, view, and delete controls and their dead logic', () => {
  assert.doesNotMatch(source, />\s*添加题目\s*</)
  assert.doesNotMatch(source, />\s*查看\s*</)
  assert.doesNotMatch(source, />\s*删除\s*</)
  assert.doesNotMatch(source, /topicadd|addtopic|removetopic|errortopic/)
  assert.doesNotMatch(source, /bootstrap\/dist\/js\/bootstrap|\breactive\b/)
  assert.doesNotMatch(source, /error-message/)
})

test('navigates to problem details from the whole row by mouse or keyboard', () => {
  assert.match(source, /<tr[^>]*class="problem-row"/)
  assert.match(source, /@click="handleView\(problem\.id\)"/)
  assert.match(source, /@keydown\.enter="handleView\(problem\.id\)"/)
  assert.match(source, /@keydown\.space\.prevent="handleView\(problem\.id\)"/)
  assert.match(source, /role="link"/)
  assert.match(source, /tabindex="0"/)
})

test('uses a pointer cursor for interactive problem rows', () => {
  assert.match(source, /\.problem-row\s*\{[^}]*cursor:\s*pointer/s)
})
