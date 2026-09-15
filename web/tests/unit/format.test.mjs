import test from 'node:test'
import assert from 'node:assert/strict'
import {
  difficultyClass,
  difficultyText,
  formatDateTime,
  formatRelativeTime,
  parseDateTime,
  truncateText
} from '../../src/utils/format.mjs'

test('parses the backend "yyyy-MM-dd HH:mm:ss" format as local time', () => {
  const date = parseDateTime('2026-09-05 19:54:03')
  assert.ok(date)
  assert.equal(date.getFullYear(), 2026)
  assert.equal(date.getMonth(), 8)
  assert.equal(date.getDate(), 5)
  assert.equal(date.getHours(), 19)
  assert.equal(date.getMinutes(), 54)
  assert.equal(date.getSeconds(), 3)
})

test('rejects empty and placeholder time values', () => {
  for (const value of ['', null, undefined, 'null', 'undefined', 'not a date']) {
    assert.equal(parseDateTime(value), null, `value ${value} should not parse`)
  }
})

test('formats dates with and without seconds and falls back gracefully', () => {
  assert.equal(formatDateTime('2026-01-02 03:04:05'), '2026-01-02 03:04')
  assert.equal(formatDateTime('2026-01-02 03:04:05', { withSeconds: true }), '2026-01-02 03:04:05')
  assert.equal(formatDateTime(new Date(2026, 0, 2, 3, 4)), '2026-01-02 03:04')
  assert.equal(formatDateTime(''), '未设置')
  assert.equal(formatDateTime('garbage', { fallback: '-' }), '-')
})

test('describes recent times relative to now', () => {
  const now = new Date(2026, 8, 5, 12, 0, 0)
  assert.equal(formatRelativeTime(new Date(2026, 8, 5, 11, 59, 30), now), '刚刚')
  assert.equal(formatRelativeTime(new Date(2026, 8, 5, 11, 30, 0), now), '30 分钟前')
  assert.equal(formatRelativeTime(new Date(2026, 8, 5, 9, 0, 0), now), '3 小时前')
  assert.equal(formatRelativeTime(new Date(2026, 8, 3, 12, 0, 0), now), '2 天前')
  assert.equal(formatRelativeTime(new Date(2026, 7, 1, 12, 0, 0), now), '2026-08-01 12:00')
  assert.equal(formatRelativeTime('nope', now), '')
})

test('maps difficulty stars to labels and classes', () => {
  assert.equal(difficultyText('1'), '简单')
  assert.equal(difficultyText(3), '中等')
  assert.equal(difficultyText('5'), '困难')
  assert.equal(difficultyText('7'), '7 星')
  assert.equal(difficultyText(''), '未知')
  assert.equal(difficultyClass('2'), 'difficulty-easy')
  assert.equal(difficultyClass('4'), 'difficulty-medium')
  assert.equal(difficultyClass('5'), 'difficulty-hard')
  assert.equal(difficultyClass('abc'), 'difficulty-unknown')
})

test('truncates long descriptions on a single line', () => {
  assert.equal(truncateText('  hello\n  world  ', 80), 'hello world')
  assert.equal(truncateText('a'.repeat(100), 10), `${'a'.repeat(10)}…`)
  assert.equal(truncateText(null), '')
})
