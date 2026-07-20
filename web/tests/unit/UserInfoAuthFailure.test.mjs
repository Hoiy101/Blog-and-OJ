import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const storeSource = await readFile(
  new URL('../../src/store/user.js', import.meta.url),
  'utf8'
)
const loginViewSource = await readFile(
  new URL('../../src/views/user/bot/account/UserAccountLoginView.vue', import.meta.url),
  'utf8'
)

test('clears authentication for business and HTTP getinfo failures', () => {
  const getinfoAction = storeSource.match(
    /getinfo\(context, data\) \{([\s\S]*?)\n {8}\},\n {8}uploadAvatar/
  )?.[1]

  assert.ok(getinfoAction, 'getinfo action should exist')
  assert.match(getinfoAction, /const handleError = \(resp\) => \{[\s\S]*localStorage\.removeItem\("jwt_token"\)[\s\S]*context\.commit\("logout"\)[\s\S]*data\.error\(resp\)/)
  assert.equal(getinfoAction.match(/handleError\(resp\)/g)?.length, 2)
})

test('both getinfo calls show the failure and return to login', () => {
  assert.match(loginViewSource, /const handleGetInfoError = \(resp\) => \{/)
  assert.match(loginViewSource, /登录状态已失效，请重新登录/)
  assert.match(loginViewSource, /router\.replace\(\{ name: 'user_account_login' \}\)/)
  assert.equal(loginViewSource.match(/error:\s*handleGetInfoError/g)?.length, 2)
})
