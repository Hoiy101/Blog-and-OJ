import { normalizeImageWidth } from './markdown.mjs'

export const imageMarkdown = (alt, url, width = 25, title = '') => {
  const normalized = normalizeImageWidth(width)
  const safeAlt = String(alt || '图片').replace(/\]|\[|\r|\n/g, '') || '图片'
  const safeTitle = String(title || '').replace(/["\r\n]/g, '')
  const suffix = normalized === 25 ? '' : `{width=${normalized}%}`
  const titleSuffix = safeTitle ? ` "${safeTitle}"` : ''
  return `\n![${safeAlt}](${url}${titleSuffix})${suffix}\n`
}
export function insertAtSelection(value, snippet, start, end) {
  const text = String(value ?? '')
  const insertion = String(snippet)
  return { value: text.slice(0, start) + insertion + text.slice(end), cursor: start + insertion.length }
}
