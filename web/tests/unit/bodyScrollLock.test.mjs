import test from 'node:test'
import assert from 'node:assert/strict'
import { lockBodyScroll, unlockBodyScroll, resetBodyScrollLocks } from '../../src/utils/bodyScrollLock.mjs'

test('restores body overflow only after all lightboxes release it', () => {
  const body = { style: { overflow: 'auto' } }
  resetBodyScrollLocks(body)
  lockBodyScroll('one', body); lockBodyScroll('two', body)
  assert.equal(body.style.overflow, 'hidden')
  unlockBodyScroll('one', body)
  assert.equal(body.style.overflow, 'hidden')
  unlockBodyScroll('two', body)
  assert.equal(body.style.overflow, 'auto')
})
