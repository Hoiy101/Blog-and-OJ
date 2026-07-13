import test from 'node:test'
import assert from 'node:assert/strict'
import { loadImageUrl, resetImageLoadCache } from '../../src/utils/markdownImages.mjs'

test('shares one preload promise for repeated image URLs', async () => {
  resetImageLoadCache()
  let created = 0
  class FakeImage {
    constructor() { created += 1 }
    set src(value) { this._src = value; queueMicrotask(() => this.onload()) }
  }
  const first = loadImageUrl('https://a.test/a.png', FakeImage)
  const second = loadImageUrl('https://a.test/a.png', FakeImage)
  assert.equal(first, second)
  await first
  assert.equal(created, 1)
})

test('caches failures until reset', async () => {
  resetImageLoadCache()
  let created = 0
  class BrokenImage {
    constructor() { created += 1 }
    set src(value) { this._src = value; queueMicrotask(() => this.onerror()) }
  }
  await assert.rejects(loadImageUrl('https://a.test/missing.png', BrokenImage))
  await assert.rejects(loadImageUrl('https://a.test/missing.png', BrokenImage))
  assert.equal(created, 1)
})
