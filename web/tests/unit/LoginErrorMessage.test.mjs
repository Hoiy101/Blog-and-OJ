import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(
  new URL('../../src/views/user/bot/account/UserAccountLoginView.vue', import.meta.url),
  'utf8'
)

test('shows backend login errors with a generic fallback', () => {
  assert.match(
    source,
    /error\(resp\)[\s\S]{0,240}resp\?\.error_message\s*\|\|[\s\S]{0,120}resp\?\.responseJSON\?\.error_message/
  )
  assert.match(source, /登录失败，请稍后重试/)
  assert.doesNotMatch(source, /error_message\.value\s*=\s*["']用户名或密码错误["']/)
})
