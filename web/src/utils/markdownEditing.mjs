import { normalizeImageWidth } from './markdown.mjs'

export const imageMarkdown = (alt, url, width = 25) => {
  const normalized = normalizeImageWidth(width)
  const suffix = normalized === 25 ? '' : `{width=${normalized}%}`
  return `\n![${alt || '图片'}](${url})${suffix}\n`
}
export function insertAtSelection(value, snippet, start, end) {
  const text = String(value ?? '')
  const insertion = String(snippet)
  return { value: text.slice(0, start) + insertion + text.slice(end), cursor: start + insertion.length }
}
