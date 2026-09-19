const TOPIC_FIELDS = [
  ['test_point', 'testPoint'],
  ['title', 'title'],
  ['description', 'description'],
  ['star', 'star'],
  ['time_limit', 'timeLimit'],
  ['mem_limit', 'memLimit'],
  ['input_format', 'inputFormat'],
  ['output_format', 'outputFormat'],
  ['sample_input', 'sampleInput'],
  ['sample_output', 'sampleOutput'],
  ['hint', 'hint']
]

const stringValue = value => value == null ? '' : String(value)

export const isAdmin = root => root === 'true'

export const statusForBannedState = isBanned => isBanned ? 1 : 0

export const latestLoginRecords = (records, limit = 100) => {
  if (!Array.isArray(records)) return []

  return records
    .map((record, index) => ({
      record,
      index,
      timestamp: Date.parse(record?.time)
    }))
    .sort((left, right) => {
      const leftValid = Number.isFinite(left.timestamp)
      const rightValid = Number.isFinite(right.timestamp)

      if (leftValid && rightValid) {
        return right.timestamp - left.timestamp || left.index - right.index
      }
      if (leftValid) return -1
      if (rightValid) return 1
      return left.index - right.index
    })
    .slice(0, Math.max(0, limit))
    .map(item => item.record)
}

const CIRCUIT_BREAKER_STATES = {
  CLOSED: { label: '正常', badge: 'bg-success', hint: '请求正常发往判题服务' },
  OPEN: { label: '熔断中', badge: 'bg-danger', hint: '请求被直接拦截，不再发往判题服务' },
  HALF_OPEN: { label: '试探恢复', badge: 'bg-warning text-dark', hint: '正在放行少量请求探测判题服务' },
  FORCED_OPEN: { label: '强制熔断', badge: 'bg-danger', hint: '熔断器被手动置为打开' },
  DISABLED: { label: '已停用', badge: 'bg-secondary', hint: '熔断器已停用，不做任何拦截' },
  METRICS_ONLY: { label: '仅统计', badge: 'bg-secondary', hint: '只统计指标，不做拦截' }
}

const UNKNOWN_CIRCUIT_BREAKER_STATE = { label: '未知', badge: 'bg-secondary', hint: '' }

export const circuitBreakerState = state =>
  CIRCUIT_BREAKER_STATES[state] || UNKNOWN_CIRCUIT_BREAKER_STATE

// 窗口内调用数还不够 minimumNumberOfCalls 时，Resilience4j 的失败率是 -1
export const formatFailureRate = rate => {
  const value = rate === null || rate === undefined || rate === '' ? NaN : Number(rate)
  return !Number.isFinite(value) || value < 0 ? '统计中' : `${value.toFixed(1)}%`
}

export const emptyTopicForm = () => Object.fromEntries(
  TOPIC_FIELDS.map(([field]) => [field, ''])
)

export const topicToForm = topic => Object.fromEntries(
  TOPIC_FIELDS.map(([field, camelField]) => [
    field,
    stringValue(topic?.[field] ?? topic?.[camelField])
  ])
)

export const topicPayload = (form, topicId) => {
  const payload = Object.fromEntries(
    TOPIC_FIELDS.map(([field]) => [field, stringValue(form?.[field])])
  )

  if (topicId !== undefined && topicId !== null && topicId !== '') {
    payload.topic_id = String(topicId)
  }

  return payload
}

export const evaluatePayload = (records, topicId) => (
  Array.isArray(records) ? records : []
).map(record => {
  const payload = {
    topic_id: stringValue(topicId),
    input: stringValue(record?.input),
    output: stringValue(record?.output)
  }

  if (record?.id !== undefined && record?.id !== null && record.id !== '') {
    payload.id = String(record.id)
  }

  return record?.id !== undefined && record?.id !== null && record.id !== ''
    ? { id: payload.id, topic_id: payload.topic_id, input: payload.input, output: payload.output }
    : payload
})
