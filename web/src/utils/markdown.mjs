const escapeHtml = value => String(value ?? '')
  .replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;')
  .replace(/"/g, '&quot;').replace(/'/g, '&#39;')

const safeUrl = value => {
  const url = String(value ?? '').trim()
  return /^(https?:\/\/|\/|\.\.?\/|#)/i.test(url) ? url : ''
}

export function normalizeImageWidth(value, fallback = 25) {
  const text = String(value ?? '').trim()
  if (!/^\d{1,3}$/.test(text)) return fallback
  const width = Number(text)
  return width >= 10 && width <= 100 ? width : fallback
}

const inline = (source, options) => {
  let text = escapeHtml(source)
  const tokens = []
  const keep = html => { const key = `@@MDTOKEN${tokens.length}@@`; tokens.push(html); return key }
  text = text.replace(/`([^`]+)`/g, (_, code) => keep(`<code>${code}</code>`))
  text = text.replace(/!\[([^\]]*)\]\(([^\s)]+)(?:\s+[&quot;][^&quot;]*[&quot;])?\)(?:\{width=([^}%]+)%\})?/g, (_, alt, rawUrl, rawWidth) => {
    const url = safeUrl(rawUrl)
    if (!url) return alt
    const width = normalizeImageWidth(rawWidth, 25)
    const imageAttributes = `data-image-width="${width}" style="--markdown-image-width:${width}%"`
    const placeholder = safeUrl(options.imagePlaceholder)
    return keep(placeholder
      ? `<img src="${escapeHtml(placeholder)}" data-original-src="${escapeHtml(url)}" data-image-state="loading" ${imageAttributes} alt="${alt}" loading="lazy">`
      : `<img src="${escapeHtml(url)}" ${imageAttributes} alt="${alt}" loading="lazy">`)
  })
  text = text.replace(/\[([^\]]+)\]\(([^\s)]+)(?:\s+[&quot;][^&quot;]*[&quot;])?\)/g, (_, label, rawUrl) => {
    const url = safeUrl(rawUrl)
    return url ? keep(`<a href="${escapeHtml(url)}" target="_blank" rel="noopener noreferrer">${label}</a>`) : label
  })
  text = text.replace(/\*\*([^*]+)\*\*/g, '<strong>$1</strong>')
    .replace(/__([^_]+)__/g, '<strong>$1</strong>')
    .replace(/(^|[^*])\*([^*\n]+)\*/g, '$1<em>$2</em>')
    .replace(/~~([^~]+)~~/g, '<del>$1</del>')
  return text.replace(/@@MDTOKEN(\d+)@@/g, (_, index) => tokens[Number(index)])
}

export function renderMarkdown(source, options = {}) {
  const lines = String(source ?? '').replace(/\r\n?/g, '\n').split('\n')
  const output = []
  let paragraph = []
  let list = null
  let inCode = false
  let code = []
  let language = ''
  const flushParagraph = () => {
    if (paragraph.length) output.push(`<p>${paragraph.map(line => inline(line, options)).join('<br>')}</p>`)
    paragraph = []
  }
  const closeList = () => { if (list) output.push(`</${list}>`); list = null }
  for (let index = 0; index < lines.length; index += 1) {
    const line = lines[index]
    const fence = line.match(/^```\s*([\w-]*)/)
    if (fence) {
      if (inCode) {
        output.push(`<pre><code${language ? ` class="language-${escapeHtml(language)}"` : ''}>${escapeHtml(code.join('\n'))}</code></pre>`)
        inCode = false; code = []; language = ''
      } else { flushParagraph(); closeList(); inCode = true; language = fence[1] }
      continue
    }
    if (inCode) { code.push(line); continue }
    if (!line.trim()) { flushParagraph(); closeList(); continue }
    const heading = line.match(/^(#{1,6})\s+(.+)$/)
    if (heading) { flushParagraph(); closeList(); const level = heading[1].length; output.push(`<h${level}>${inline(heading[2], options)}</h${level}>`); continue }
    if (/^ {0,3}([-*_])(?:\s*\1){2,}\s*$/.test(line)) { flushParagraph(); closeList(); output.push('<hr>'); continue }
    const quote = line.match(/^>\s?(.*)$/)
    if (quote) { flushParagraph(); closeList(); output.push(`<blockquote><p>${inline(quote[1], options)}</p></blockquote>`); continue }
    const item = line.match(/^\s*([-+*]|\d+\.)\s+(.+)$/)
    if (item) {
      flushParagraph(); const type = /\d/.test(item[1]) ? 'ol' : 'ul'
      if (list !== type) { closeList(); list = type; output.push(`<${type}>`) }
      output.push(`<li>${inline(item[2], options)}</li>`); continue
    }
    if (line.includes('|') && index + 1 < lines.length && /^\s*\|?\s*:?-+/.test(lines[index + 1])) {
      flushParagraph(); closeList()
      const headers = line.replace(/^\||\|$/g, '').split('|')
      index += 1; const rows = []
      while (index + 1 < lines.length && lines[index + 1].includes('|') && lines[index + 1].trim()) rows.push(lines[++index])
      output.push(`<table><thead><tr>${headers.map(cell => `<th>${inline(cell.trim(), options)}</th>`).join('')}</tr></thead><tbody>${rows.map(row => `<tr>${row.replace(/^\||\|$/g, '').split('|').map(cell => `<td>${inline(cell.trim(), options)}</td>`).join('')}</tr>`).join('')}</tbody></table>`)
      continue
    }
    paragraph.push(line)
  }
  if (inCode) output.push(`<pre><code>${escapeHtml(code.join('\n'))}</code></pre>`)
  flushParagraph(); closeList()
  return output.join('\n')
}
