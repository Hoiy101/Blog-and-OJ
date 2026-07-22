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
