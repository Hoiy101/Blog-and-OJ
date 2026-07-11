import { normalizeImageWidth } from './markdown.mjs'
import { imageMarkdown } from './markdownEditing.mjs'

const NODE_ELEMENT = 1
const NODE_TEXT = 3
const ALLOWED_TAGS = new Set([
  'P', 'BR', 'H1', 'H2', 'H3', 'H4', 'H5', 'H6', 'UL', 'OL', 'LI',
  'BLOCKQUOTE', 'STRONG', 'EM', 'DEL', 'A', 'CODE', 'PRE', 'TABLE',
  'THEAD', 'TBODY', 'TR', 'TH', 'TD', 'IMG'
])
const DROPPED_TAGS = new Set(['SCRIPT', 'STYLE', 'IFRAME', 'OBJECT'])

export const isSafeEditorUrl = value => /^(https?:\/\/|\/|\.\.?\/|#)/i.test(String(value || '').trim())

export function calculateImageWidth(pointerX, containerLeft, containerWidth) {
  if (!Number.isFinite(containerWidth) || containerWidth <= 0) return 25
  const raw = Math.round(((pointerX - containerLeft) / containerWidth) * 100)
  return Math.min(100, Math.max(10, raw))
}

const children = node => [...(node?.childNodes || [])]
const childrenMarkdown = node => children(node).map(serializeEditorNode).join('')
const compact = value => String(value || '').replace(/[ \t]+\n/g, '\n').replace(/\n{3,}/g, '\n\n').trim()

const tableMarkdown = table => {
  const rows = []
  const visit = node => {
    if (node?.tagName === 'TR') {
      rows.push(children(node).filter(cell => ['TH', 'TD'].includes(cell.tagName)).map(cell => compact(childrenMarkdown(cell))))
      return
    }
    children(node).forEach(visit)
  }
  visit(table)
  if (!rows.length) return ''
  const header = rows[0]
  const separator = header.map(() => '---')
  return [header, separator, ...rows.slice(1)].map(row => `| ${row.join(' | ')} |`).join('\n') + '\n\n'
}

export function serializeEditorNode(node) {
  if (!node) return ''
  if (node.nodeType === NODE_TEXT) return node.textContent || ''
  if (node.nodeType !== NODE_ELEMENT) return ''
  if (node.dataset?.editorUi) return ''
  const tag = String(node.tagName || '').toLowerCase()
  const inner = childrenMarkdown(node)
  if (tag === 'img') {
    const url = node.dataset?.originalSrc || node.getAttribute?.('src') || ''
    if (!isSafeEditorUrl(url)) return ''
    const alt = node.getAttribute?.('alt') || node.attributes?.alt || '图片'
    return imageMarkdown(alt, url, normalizeImageWidth(node.dataset?.imageWidth)).trim()
  }
  if (tag === 'br') return '\n'
  if (tag === 'strong' || tag === 'b') return `**${inner}**`
  if (tag === 'em' || tag === 'i') return `*${inner}*`
  if (tag === 'del' || tag === 's') return `~~${inner}~~`
  if (tag === 'code' && node.parentNode?.tagName !== 'PRE') return `\`${inner}\``
  if (tag === 'a') {
    const href = node.getAttribute?.('href') || ''
    return isSafeEditorUrl(href) ? `[${inner}](${href})` : inner
  }
  if (/^h[1-6]$/.test(tag)) return `${'#'.repeat(Number(tag[1]))} ${inner}\n\n`
  if (tag === 'blockquote') return `${inner.split('\n').filter(Boolean).map(line => `> ${line}`).join('\n')}\n\n`
  if (tag === 'li') return `${inner}\n`
  if (tag === 'ul') return `${inner.split('\n').filter(Boolean).map(line => `- ${line}`).join('\n')}\n\n`
  if (tag === 'ol') return `${inner.split('\n').filter(Boolean).map((line, index) => `${index + 1}. ${line}`).join('\n')}\n\n`
  if (tag === 'pre') return `\`\`\`\n${node.textContent || inner}\n\`\`\`\n\n`
  if (tag === 'table') return tableMarkdown(node)
  if (['thead', 'tbody', 'tr', 'th', 'td'].includes(tag)) return inner
  if (tag === 'p' || tag === 'div') return `${inner}\n\n`
  return inner
}

export function serializeEditor(root) {
  return compact(children(root).map(serializeEditorNode).join(''))
}

const adjacentFromText = (node, offset, direction) => {
  if (direction === 'backward' && offset === 0) return node.previousSibling
  if (direction === 'forward' && offset === String(node.textContent || '').length) return node.nextSibling
  return null
}

export function adjacentImageFromSelection(selection, direction) {
  if (!selection?.isCollapsed) return null
  const container = selection.anchorNode
  const offset = selection.anchorOffset
  let candidate
  if (container?.nodeType === NODE_TEXT) candidate = adjacentFromText(container, offset, direction)
  else {
    const nodes = container?.childNodes || []
    candidate = direction === 'backward' ? nodes[offset - 1] : nodes[offset]
  }
  return candidate?.matches?.('img[data-original-src]') ? candidate : null
}

const sanitizeNode = (node, documentRef) => {
  if (node.nodeType === NODE_TEXT) return documentRef.createTextNode(node.textContent || '')
  const fragment = documentRef.createDocumentFragment()
  if (node.nodeType !== NODE_ELEMENT || DROPPED_TAGS.has(node.tagName)) return fragment
  const cleanChildren = children(node).map(child => sanitizeNode(child, documentRef))
  if (!ALLOWED_TAGS.has(node.tagName)) {
    cleanChildren.forEach(child => fragment.appendChild(child))
    return fragment
  }
  const clean = documentRef.createElement(node.tagName.toLowerCase())
  if (node.tagName === 'A') {
    const href = node.getAttribute?.('href') || ''
    if (!isSafeEditorUrl(href)) {
      cleanChildren.forEach(child => fragment.appendChild(child))
      return fragment
    }
    clean.setAttribute('href', href)
  }
  if (node.tagName === 'IMG') {
    const original = node.dataset?.originalSrc || node.getAttribute?.('src') || ''
    if (!isSafeEditorUrl(original)) return fragment
    const width = normalizeImageWidth(node.dataset?.imageWidth)
    clean.setAttribute('src', original)
    clean.setAttribute('alt', node.getAttribute?.('alt') || '图片')
    clean.dataset.originalSrc = original
    clean.dataset.imageWidth = String(width)
    clean.style.setProperty('--markdown-image-width', `${width}%`)
    return clean
  }
  cleanChildren.forEach(child => clean.appendChild(child))
  return clean
}

export function sanitizeEditorFragment(root, documentRef) {
  const fragment = documentRef.createDocumentFragment()
  children(root).forEach(node => fragment.appendChild(sanitizeNode(node, documentRef)))
  return fragment
}
