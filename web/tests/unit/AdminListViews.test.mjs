import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const usersSource = await readFile(
  new URL('../../src/views/manage/ManageUsersView.vue', import.meta.url),
  'utf8'
)
const recordsSource = await readFile(
  new URL('../../src/views/manage/ManageLoginRecordsView.vue', import.meta.url),
  'utf8'
)

const assertAdminCardShell = source => {
  assert.match(source, /<main class="admin-page-shell">/)
  assert.match(source, /<div class="container admin-page-container">[\s\S]*<section class="card admin-card">/)
  const shellRule = source.match(/\.admin-page-shell\s*\{([^}]*)\}/)?.[1] || ''
  assert.match(shellRule, /min-height:\s*calc\(100vh - 56px\)/)
  assert.doesNotMatch(shellRule, /background(?:-color)?\s*:/)
  assert.match(source, /\.admin-page-container\s*\{[\s\S]*padding-top:\s*1\.5rem/)
  assert.match(source, /\.admin-card\s*\{[\s\S]*border:\s*1px solid/)
}

test('frames user and login-record management in spaced cards', () => {
  assertAdminCardShell(usersSource)
  assertAdminCardShell(recordsSource)
})

test('loads users and refreshes after ban state changes', () => {
  assert.match(usersSource, /adminApi\.listUsers\(store\.state\.user\.token\)/)
  assert.match(usersSource, /statusForBannedState\(banned\)/)
  assert.match(usersSource, /adminApi\.updateBanned\([\s\S]*user\.username[\s\S]*status/)
  assert.match(usersSource, /resp\.error_message !== 'success'/)
  assert.match(usersSource, /await loadUsers\(\)/)
  assert.match(usersSource, /:disabled="updatingUsername === user\.username"/)
})

test('renders user role, status, errors, empty state, and retry control', () => {
  assert.match(usersSource, /用户管理/)
  assert.match(usersSource, /user\.root === 'true'/)
  assert.match(usersSource, /user\.banned === 'true'/)
  assert.match(usersSource, /v-else-if="error"/)
  assert.match(usersSource, /@click="loadUsers"/)
  assert.match(usersSource, /暂无用户数据/)
})

test('loads and limits login records through the domain helper', () => {
  assert.match(recordsSource, /adminApi\.listLoginRecords\(store\.state\.user\.token\)/)
  assert.match(recordsSource, /latestLoginRecords\(resp, 100\)/)
  assert.match(recordsSource, /用户登录信息/)
  assert.match(recordsSource, /登录 IP/)
  assert.match(recordsSource, /登录时间/)
  assert.match(recordsSource, /@click="loadRecords"/)
  assert.match(recordsSource, /暂无登录记录/)
})
