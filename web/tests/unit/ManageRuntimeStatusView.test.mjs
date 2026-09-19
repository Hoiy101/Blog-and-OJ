import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'
import { circuitBreakerState, formatFailureRate } from '../../src/utils/admin.mjs'

const source = await readFile(
  new URL('../../src/views/manage/ManageRuntimeStatusView.vue', import.meta.url),
  'utf8'
)

test('frames the runtime status page in the shared admin card shell', () => {
  assert.match(source, /<main class="admin-page-shell">/)
  assert.match(source, /<div class="container admin-page-container">/)
  assert.match(source, /<section class="card admin-card h-100">/)
  const shellRule = source.match(/\.admin-page-shell\s*\{([^}]*)\}/)?.[1] || ''
  assert.match(shellRule, /min-height:\s*calc\(100vh - 56px\)/)
  assert.doesNotMatch(shellRule, /background(?:-color)?\s*:/)
  assert.match(source, /\.admin-card\s*\{[\s\S]*border:\s*1px solid/)
})

test('renders one card for the judge service and one for the circuit breaker', () => {
  assert.match(source, /判题服务/)
  assert.match(source, /熔断器/)
  assert.match(source, /排队任务/)
  assert.match(source, /运行中/)
  assert.match(source, /失败率/)
  assert.match(source, /窗口内调用/)
  assert.match(source, /已拦截/)
})

test('loads both endpoints together and keeps a refresh control', () => {
  assert.match(source, /adminApi\.judgeStatus\(store\.state\.user\.token\)/)
  assert.match(source, /adminApi\.judgeCircuitBreaker\(store\.state\.user\.token\)/)
  assert.match(source, /Promise\.all\(/)
  assert.match(source, /@click="loadStatus"/)
  assert.match(source, /onMounted\(loadStatus\)/)
  assert.match(source, /更新于/)
})

test('stops the auto refresh timer when it is switched off or the page unmounts', () => {
  assert.match(source, /clearInterval\(timer\)/)
  assert.match(source, /onUnmounted\(stopTimer\)/)
  assert.match(source, /watch\(autoRefresh/)
})

test('shows an empty state before the first remote call creates a breaker', () => {
  assert.match(source, /v-else-if="breakers\.length === 0"/)
  assert.match(source, /尚未产生远程调用/)
})

test('reads the judge online flag as either a boolean or a string', () => {
  assert.match(source, /judge\.value\.online === true \|\| judge\.value\.online === 'true'/)
})

test('maps every circuit breaker state to a label and a badge', () => {
  assert.equal(circuitBreakerState('CLOSED').label, '正常')
  assert.equal(circuitBreakerState('CLOSED').badge, 'bg-success')
  assert.equal(circuitBreakerState('OPEN').label, '熔断中')
  assert.equal(circuitBreakerState('OPEN').badge, 'bg-danger')
  assert.equal(circuitBreakerState('HALF_OPEN').label, '试探恢复')
  assert.equal(circuitBreakerState('DISABLED').label, '已停用')
  assert.equal(circuitBreakerState('FORCED_OPEN').label, '强制熔断')
  assert.equal(circuitBreakerState(undefined).label, '未知')
})

test('reports an unmeasured failure rate as 统计中', () => {
  assert.equal(formatFailureRate(-1), '统计中')
  assert.equal(formatFailureRate(null), '统计中')
  assert.equal(formatFailureRate('abc'), '统计中')
  assert.equal(formatFailureRate(0), '0.0%')
  assert.equal(formatFailureRate(83.333), '83.3%')
})
