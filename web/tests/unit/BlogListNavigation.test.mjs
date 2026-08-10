import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(
  new URL('../../src/views/pk/PkIndexView.vue', import.meta.url),
  'utf8'
)

test('removes the blog detail button and activates the complete blog item', () => {
  assert.doesNotMatch(source, />\s*查看详情\s*</)
  const blogItemTag = source.match(
    /<div\s+v-for="record in records"[\s\S]*?>/
  )?.[0]

  assert.ok(blogItemTag, 'blog list item opening tag should exist')
  assert.match(blogItemTag, /class="blog-item border-bottom p-4"/)
  assert.match(blogItemTag, /@click="viewBlogDetail\(record\.id\)"/)
})

test('supports keyboard navigation with link semantics', () => {
  assert.match(source, /role="link"/)
  assert.match(source, /tabindex="0"/)
  assert.match(source, /@keydown\.enter="viewBlogDetail\(record\.id\)"/)
  assert.match(source, /@keydown\.space\.prevent="viewBlogDetail\(record\.id\)"/)
})

test('preserves the light-gray hover and exposes mouse and keyboard focus', () => {
  assert.match(source, /\.blog-item\s*\{[^}]*cursor:\s*pointer/s)
  assert.match(source, /\.blog-item:hover\s*\{[^}]*background-color:\s*#f8fafc/s)
  assert.match(source, /\.blog-item:focus-visible\s*\{[^}]*outline:/s)
})

test('requests and renders only the current blog page', () => {
  assert.match(source, /const requestData = paginationQuery\(requestedPage, requestedKeyword\)/)
  assert.match(source, /data:\s*requestData/)
  assert.match(source, /records\.value\s*=\s*page\.records/)
  assert.doesNotMatch(source, /filteredRecords|\.slice\(/)
  assert.match(source, /const currentPage = ref\(1\)/)
  assert.match(source, /const totalPages = ref\(0\)/)
})

test('renders bottom-right accessible blog pagination controls', () => {
  assert.match(source, /class="pagination-controls"/)
  assert.match(source, /aria-label="上一页"/)
  assert.match(source, /type="number"/)
  assert.match(source, /aria-label="跳转页码"/)
  assert.match(source, /aria-label="下一页"/)
  assert.match(source, /justify-content:\s*flex-end/)
})

test('resets blog searches to page one and disables boundary arrows', () => {
  assert.match(source, /activeKeyword\.value = searchKeyword\.value\.trim\(\)/)
  assert.match(source, /getBlogList\(1\)/)
  assert.match(source, /:disabled="loading \|\| currentPage <= 1"/)
  assert.match(source, /:disabled="loading \|\| totalPages === 0 \|\| currentPage >= totalPages"/)
})

test('ignores stale blog responses and retries the exact failed query', () => {
  assert.match(source, /const blogListRequestId = ref\(0\)/)
  assert.match(source, /const requestId = \+\+blogListRequestId\.value/)
  const staleGuards = source.match(/if \(requestId !== blogListRequestId\.value\) return/g) || []
  assert.ok(staleGuards.length >= 3, 'success, error and complete callbacks should ignore stale requests')
  assert.match(source, /@click="retryBlogList"/)
  assert.match(source, /getBlogList\(retryPage\.value, retryKeyword\.value\)/)
})
