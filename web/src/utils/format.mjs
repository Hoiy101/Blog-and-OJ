const INVALID_TIME_VALUES = new Set(['', 'null', 'undefined'])

/**
 * 后端有两种时间格式：列表接口返回 "yyyy-MM-dd HH:mm:ss"，
 * 部分详情接口返回 Java Date.toString() 的字符串。这里统一解析。
 */
export function parseDateTime(value) {
  if (value instanceof Date) return Number.isNaN(value.getTime()) ? null : value
  if (typeof value === 'number') {
    const date = new Date(value)
    return Number.isNaN(date.getTime()) ? null : date
  }
  const text = String(value ?? '').trim()
  if (INVALID_TIME_VALUES.has(text)) return null

  const isoLike = text.match(/^(\d{4}-\d{2}-\d{2})[ T](\d{2}:\d{2}(?::\d{2})?)(\.\d+)?$/)
  const candidate = isoLike ? `${isoLike[1]}T${isoLike[2]}` : text
  const date = new Date(candidate)
  return Number.isNaN(date.getTime()) ? null : date
}

const pad = number => String(number).padStart(2, '0')

export function formatDateTime(value, { fallback = '未设置', withSeconds = false } = {}) {
  const date = parseDateTime(value)
  if (!date) return fallback
  const base = `${date.getFullYear()}-${pad(date.getMonth() + 1)}-${pad(date.getDate())} ${pad(date.getHours())}:${pad(date.getMinutes())}`
  return withSeconds ? `${base}:${pad(date.getSeconds())}` : base
}

export function formatRelativeTime(value, now = new Date()) {
  const date = parseDateTime(value)
  if (!date) return ''
  const diff = now.getTime() - date.getTime()
  const minute = 60 * 1000
  const hour = 60 * minute
  const day = 24 * hour
  if (diff < 0) return formatDateTime(date)
  if (diff < minute) return '刚刚'
  if (diff < hour) return `${Math.floor(diff / minute)} 分钟前`
  if (diff < day) return `${Math.floor(diff / hour)} 小时前`
  if (diff < 7 * day) return `${Math.floor(diff / day)} 天前`
  return formatDateTime(date)
}

const DIFFICULTY_TEXT = { 1: '简单', 2: '较易', 3: '中等', 4: '较难', 5: '困难' }

export function difficultyLevel(star) {
  const level = Number.parseInt(star, 10)
  return Number.isFinite(level) && level > 0 ? level : null
}

export function difficultyClass(star) {
  const level = difficultyLevel(star)
  if (level === null) return 'difficulty-unknown'
  if (level <= 2) return 'difficulty-easy'
  if (level <= 4) return 'difficulty-medium'
  return 'difficulty-hard'
}

export function difficultyText(star) {
  const level = difficultyLevel(star)
  if (level === null) return '未知'
  return DIFFICULTY_TEXT[level] || `${level} 星`
}

export function truncateText(value, maxLength = 80) {
  const text = String(value ?? '').replace(/\s+/g, ' ').trim()
  if (text.length <= maxLength) return text
  return `${text.slice(0, maxLength)}…`
}
