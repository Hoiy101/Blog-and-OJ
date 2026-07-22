import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const storeSource = await readFile(
  new URL('../../src/store/user.js', import.meta.url),
  'utf8'
)

test('stores administrator identity from getinfo and clears it on logout', () => {
  assert.match(storeSource, /root:\s*["']{2}/)
  assert.match(storeSource, /state\.root\s*=\s*user\.root/)

  const logoutMutation = storeSource.match(
    /logout\(state\)\s*\{([\s\S]*?)\n\s*\},\n\s*updatePullingInfo/
  )?.[1]

  assert.ok(logoutMutation, 'logout mutation should exist')
  assert.match(logoutMutation, /state\.root\s*=\s*["']{2}/)
})
