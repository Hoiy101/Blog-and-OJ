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
    /<div\s+v-for="record in filteredRecords"[\s\S]*?>/
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
