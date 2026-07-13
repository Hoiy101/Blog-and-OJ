import { normalizeImageWidth } from './markdown.mjs'
import { imageMarkdown } from './markdownEditing.mjs'

const NODE_ELEMENT = 1
const NODE_TEXT = 3
const ALLOWED_TAGS = new Set([
  'P', 'BR', 'H1', 'H2', 'H3', 'H4', 'H5', 'H6', 'UL', 'OL', 'LI',
  'BLOCKQUOTE', 'STRONG', 'EM', 'DEL', 'A', 'CODE', 'PRE', 'TABLE',
  'THEAD', 'TBODY', 'TR', 'TH', 'TD', 'IMG', 'HR'
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
const rawText = node => node?.nodeType === NODE_TEXT ? (node.textContent || '') : children(node).map(rawText).join('')
const compactCell = value => String(value || '').replace(/\s+/g, ' ').trim()
const normalizeBlockSpacing = value => {
  const output = []
  let inFence = false
  for (const line of String(value || '').split('\n')) {
    const fence = /^```/.test(line)
    if (inFence) {
      output.push(line)
      if (fence) inFence = false
      continue
    }
    if (fence) {
      inFence = true
      output.push(line)
      continue
    }
    if (/^\s*$/.test(line)) {
      if (output.length && output[output.length - 1] !== '') output.push('')
    } else output.push(line)
  }
  while (output[0] === '') output.shift()
  while (output[output.length - 1] === '') output.pop()
  return output.join('\n')
}
const escapeMarkdownText = value => String(value || '').replace(/([\\`*_[\]{}()#+\-.!|>])/g, '\\$1')
const safeMarkdownUrl = value => encodeURI(String(value || '').trim()).replace(/\(/g, '%28').replace(/\)/g, '%29')
const safeMarkdownTitle = value => String(value || '').replace(/["\r\n]/g, '')

const tableMarkdown = table => {
  const rows = []
  const visit = node => {
    if (node?.tagName === 'TR') {
      rows.push(children(node).filter(cell => ['TH', 'TD'].includes(cell.tagName)).map(cell => compactCell(childrenMarkdown(cell))))
      return
    }
    children(node).forEach(visit)
  }
  visit(table)
  if (!rows.length) return ''
  const header = rows[0]
  const headerCells = children(table).flatMap(section => section.tagName === 'THEAD' ? children(section).flatMap(row => children(row)) : [])
  const separator = header.map((_, index) => {
    const alignment = headerCells[index]?.dataset?.markdownAlign
    if (alignment === 'left') return ':---'
    if (alignment === 'right') return '---:'
    if (alignment === 'center') return ':---:'
    return '---'
  })
  return [header, separator, ...rows.slice(1)].map(row => `| ${row.join(' | ')} |`).join('\n') + '\n\n'
}

export function serializeEditorNode(node) {
  if (!node) return ''
  if (node.nodeType === NODE_TEXT) return escapeMarkdownText(node.textContent || '')
  if (node.nodeType !== NODE_ELEMENT) return ''
  if (node.dataset?.editorUi) return ''
  const tag = String(node.tagName || '').toLowerCase()
  const inner = childrenMarkdown(node)
  if (tag === 'img') {
    const url = node.dataset?.originalSrc || node.getAttribute?.('src') || ''
    if (!isSafeEditorUrl(url)) return ''
    const alt = node.getAttribute?.('alt') || node.attributes?.alt || '图片'
    return imageMarkdown(alt, safeMarkdownUrl(url), normalizeImageWidth(node.dataset?.imageWidth), node.dataset?.markdownTitle).trim()
  }
  if (tag === 'br') return '\n'
  if (tag === 'strong' || tag === 'b') return `**${inner}**`
  if (tag === 'em' || tag === 'i') return `*${inner}*`
  if (tag === 'del' || tag === 's') return `~~${inner}~~`
  if (tag === 'code' && node.parentNode?.tagName !== 'PRE') return `\`${rawText(node)}\``
  if (tag === 'a') {
    const href = node.getAttribute?.('href') || ''
    const title = safeMarkdownTitle(node.dataset?.markdownTitle)
    const titleSuffix = title ? ` "${title}"` : ''
    return isSafeEditorUrl(href) ? `[${inner}](${safeMarkdownUrl(href)}${titleSuffix})` : inner
  }
  if (/^h[1-6]$/.test(tag)) return `${'#'.repeat(Number(tag[1]))} ${inner}\n\n`
  if (tag === 'blockquote') return `${inner.split('\n').filter(Boolean).map(line => `> ${line}`).join('\n')}\n\n`
  if (tag === 'li') return `${inner}\n`
  if (tag === 'ul') return `${inner.split('\n').filter(Boolean).map(line => `- ${line}`).join('\n')}\n\n`
  if (tag === 'ol') {
    const start = Number(node.dataset?.markdownStart || node.getAttribute?.('start')) || 1
    return `${inner.split('\n').filter(Boolean).map((line, index) => `${start + index}. ${line}`).join('\n')}\n\n`
  }
  if (tag === 'pre') {
    const codeClass = children(node).find(child => child.tagName === 'CODE')?.getAttribute?.('class') || ''
    const className = node.getAttribute?.('class') || codeClass
    const language = node.dataset?.markdownLanguage || String(className).match(/(?:^|\s)language-([\w-]+)/)?.[1] || ''
    return `\`\`\`${language}\n${rawText(node)}\n\`\`\`\n\n`
  }
  if (tag === 'hr') return '---\n\n'
  if (tag === 'table') return tableMarkdown(node)
  if (['thead', 'tbody', 'tr', 'th', 'td'].includes(tag)) return inner
  if (tag === 'p' || tag === 'div') return `${inner}\n\n`
  return inner
}

export function serializeEditor(root) {
  return normalizeBlockSpacing(children(root).map(serializeEditorNode).join(''))
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
    const title = safeMarkdownTitle(node.dataset?.markdownTitle || node.getAttribute?.('title'))
    if (title) clean.dataset.markdownTitle = title
  }
  if (node.tagName === 'IMG') {
    const original = node.dataset?.originalSrc || node.getAttribute?.('src') || ''
    if (!isSafeEditorUrl(original)) return fragment
    const width = normalizeImageWidth(node.dataset?.imageWidth)
    clean.setAttribute('src', original)
    clean.setAttribute('alt', node.getAttribute?.('alt') || '图片')
    clean.dataset.originalSrc = original
    clean.dataset.imageWidth = String(width)
    const title = safeMarkdownTitle(node.dataset?.markdownTitle || node.getAttribute?.('title'))
    if (title) clean.dataset.markdownTitle = title
    clean.style.setProperty('--markdown-image-width', `${width}%`)
    return clean
  }
  if (node.tagName === 'PRE') {
    const codeClass = children(node).find(child => child.tagName === 'CODE')?.getAttribute?.('class') || ''
    const language = node.dataset?.markdownLanguage || String(node.getAttribute?.('class') || codeClass).match(/(?:^|\s)language-([\w-]+)/)?.[1]
    if (language) clean.dataset.markdownLanguage = language
  }
  if (node.tagName === 'OL') {
    const start = Number(node.dataset?.markdownStart || node.getAttribute?.('start')) || 1
    clean.setAttribute('start', String(start))
    clean.dataset.markdownStart = String(start)
  }
  if (['TH', 'TD'].includes(node.tagName)) {
    const alignment = node.dataset?.markdownAlign
    if (['left', 'right', 'center'].includes(alignment)) clean.dataset.markdownAlign = alignment
  }
  cleanChildren.forEach(child => clean.appendChild(child))
  return clean
}

export function sanitizeEditorFragment(root, documentRef) {
  const fragment = documentRef.createDocumentFragment()
  children(root).forEach(node => fragment.appendChild(sanitizeNode(node, documentRef)))
  return fragment
}
