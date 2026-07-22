import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const routerSource = await readFile(
  new URL('../../src/router/index.js', import.meta.url),
  'utf8'
)
const navbarSource = await readFile(
  new URL('../../src/components/NavBar.vue', import.meta.url),
  'utf8'
)
const loginSource = await readFile(
  new URL('../../src/views/user/bot/account/UserAccountLoginView.vue', import.meta.url),
  'utf8'
)

test('registers three authenticated administrator routes', () => {
  const contracts = [
    ['manage_users', '/manage/users/'],
    ['manage_login_records', '/manage/login-records/'],
    ['manage_topics', '/manage/topics/']
  ]

  assert.match(routerSource, /ManageUsersView/)
  assert.match(routerSource, /ManageLoginRecordsView/)
  assert.match(routerSource, /ManageTopicsView/)

  for (const [name, path] of contracts) {
    const route = routerSource.match(
      new RegExp(`\\{[\\s\\S]*?path:\\s*["']${path.replaceAll('/', '\\/')}["'][\\s\\S]*?name:\\s*["']${name}["'][\\s\\S]*?meta:\\s*\\{[^}]*requiresAuth:\\s*true[^}]*requiresAdmin:\\s*true[^}]*\\}`)
    )
    assert.ok(route, `${name} should require administrator authentication`)
  }
})

test('redirects authenticated non-administrators away from admin routes', () => {
  assert.match(routerSource, /import\s*\{\s*isAdmin\s*\}/)
  assert.match(routerSource, /to\.meta\.requiresAdmin\s*&&\s*!isAdmin\(store\.state\.user\.root\)/)
  assert.match(routerSource, /next\(\{\s*name:\s*["']home["']\s*\}\)/)
})

test('preserves a safe requested admin path through identity hydration', () => {
  assert.match(routerSource, /query:\s*\{\s*redirect:\s*to\.fullPath\s*\}/)
  assert.match(loginSource, /const route = useRoute\(\)/)
  assert.match(loginSource, /route\.query\.redirect\.startsWith\('\/'\)/)
  assert.match(loginSource, /router\.push\(redirect \|\| \{ name: 'home' \}\)/)
})

test('shows an administrator dropdown with all management links', () => {
  assert.match(navbarSource, /后台管理/)
  assert.match(navbarSource, /v-if="admin"/)
  assert.match(navbarSource, /name:'manage_users'/)
  assert.match(navbarSource, /name:'manage_login_records'/)
  assert.match(navbarSource, /name:'manage_topics'/)
  assert.match(navbarSource, /computed\(\(\)\s*=>\s*isAdmin\(store\.state\.user\.root\)\)/)
})
