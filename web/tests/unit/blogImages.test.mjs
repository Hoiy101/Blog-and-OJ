import test from 'node:test'
import assert from 'node:assert/strict'
import { createBlogImageFormData } from '../../src/api/blogImages.mjs'

test('builds multipart fields expected by backend', () => {
  const entries = []
  class Form { append(key, value) { entries.push([key, value]) } }
  const file = { name: 'x.png' }
  createBlogImageFormData(12, file, Form)
  assert.deepEqual(entries, [['blog_id', '12'], ['file', file]])
})
