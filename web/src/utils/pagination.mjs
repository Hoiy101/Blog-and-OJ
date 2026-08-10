export function clampPage(page, totalPages) {
  const maximum = Number.isFinite(Number(totalPages)) && Number(totalPages) > 0
    ? Math.floor(Number(totalPages))
    : 1
  const parsed = Number.parseInt(page, 10)
  if (!Number.isFinite(parsed) || parsed < 1) return 1
  return Math.min(parsed, maximum)
}

export function paginationQuery(page, keyword) {
  return {
    page: Math.max(1, Number.parseInt(page, 10) || 1),
    keyword: String(keyword || '').trim()
  }
}

export function normalizePageResponse(response, fallbackPageSize) {
  if (!response || !Array.isArray(response.records)) {
    throw new TypeError('分页数据格式不正确')
  }

  const { currentPage, pageSize, total, totalPages } = response
  const metadataIsValid = Number.isInteger(currentPage)
    && currentPage >= 1
    && Number.isInteger(pageSize)
    && pageSize === fallbackPageSize
    && Number.isInteger(total)
    && total >= 0
    && Number.isInteger(totalPages)
    && totalPages >= 0
    && totalPages === Math.ceil(total / pageSize)
    && currentPage <= Math.max(totalPages, 1)
    && response.records.length <= pageSize

  if (!metadataIsValid) {
    throw new TypeError('分页数据格式不正确')
  }

  return {
    records: response.records,
    currentPage,
    pageSize,
    total,
    totalPages
  }
}
