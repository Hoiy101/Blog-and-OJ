import test from 'node:test'
import assert from 'node:assert/strict'
import {
  clampPage,
  normalizePageResponse,
  paginationQuery
} from '../../src/utils/pagination.mjs'

test('builds trimmed current-page query parameters', () => {
  assert.deepEqual(paginationQuery(3, ' Java '), { page: 3, keyword: 'Java' })
  assert.deepEqual(paginationQuery(0, ''), { page: 1, keyword: '' })
})

test('clamps numeric page input to available pages', () => {
  assert.equal(clampPage(-1, 5), 1)
  assert.equal(clampPage(9, 5), 5)
  assert.equal(clampPage('abc', 5), 1)
  assert.equal(clampPage(3, 0), 1)
})

test('normalizes a page response without retaining other pages', () => {
  const page = normalizePageResponse({
    records: [{ id: 11 }],
    currentPage: 2,
    pageSize: 10,
    total: 21,
    totalPages: 3
  }, 10)

  assert.deepEqual(page.records, [{ id: 11 }])
  assert.equal(page.currentPage, 2)
  assert.equal(page.pageSize, 10)
  assert.equal(page.total, 21)
  assert.equal(page.totalPages, 3)
})

test('rejects a response without current-page records', () => {
  assert.throws(() => normalizePageResponse([], 10), /分页数据格式不正确/)
})

test('rejects missing, negative, or inconsistent pagination metadata', () => {
  const valid = {
    records: [{ id: 1 }],
    currentPage: 1,
    pageSize: 10,
    total: 1,
    totalPages: 1
  }

  for (const field of ['currentPage', 'pageSize', 'total', 'totalPages']) {
    const response = { ...valid }
    delete response[field]
    assert.throws(() => normalizePageResponse(response, 10), /分页数据格式不正确/)
  }
  assert.throws(
    () => normalizePageResponse({ ...valid, total: -1 }, 10),
    /分页数据格式不正确/
  )
  assert.throws(
    () => normalizePageResponse({ ...valid, currentPage: 2 }, 10),
    /分页数据格式不正确/
  )
})
