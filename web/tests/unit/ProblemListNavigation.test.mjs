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

test('requests and renders only the current topic page', () => {
  assert.match(source, /const requestData = paginationQuery\(requestedPage, requestedKeyword\)/)
  assert.match(source, /data:\s*requestData/)
  assert.match(source, /problems\.value\s*=\s*page\.records/)
  assert.doesNotMatch(source, /filteredProblems|\.slice\(/)
  assert.match(source, /normalizePageResponse\(resp, 20\)/)
})

test('renders bottom-right accessible topic pagination controls', () => {
  assert.match(source, /class="pagination-controls"/)
  assert.match(source, /aria-label="上一页"/)
  assert.match(source, /aria-label="跳转页码"/)
  assert.match(source, /aria-label="下一页"/)
  assert.match(source, /justify-content:\s*flex-end/)
})

test('resets topic searches to page one and disables boundary arrows', () => {
  assert.match(source, /activeKeyword\.value = searchKeyword\.value\.trim\(\)/)
  assert.match(source, /getProblemList\(1\)/)
  assert.match(source, /:disabled="loading \|\| currentPage <= 1"/)
  assert.match(source, /:disabled="loading \|\| totalPages === 0 \|\| currentPage >= totalPages"/)
})

test('ignores stale topic responses and retries the exact failed query', () => {
  assert.match(source, /const topicListRequestId = ref\(0\)/)
  assert.match(source, /const requestId = \+\+topicListRequestId\.value/)
  const staleGuards = source.match(/if \(requestId !== topicListRequestId\.value\) return/g) || []
  assert.ok(staleGuards.length >= 3, 'success, error and complete callbacks should ignore stale requests')
  assert.match(source, /@click="retryProblemList"/)
  assert.match(source, /getProblemList\(retryPage\.value, retryKeyword\.value\)/)
})
