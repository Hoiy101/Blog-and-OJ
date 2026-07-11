import test from 'node:test'
import assert from 'node:assert/strict'
import {
  adjacentImageFromSelection,
  calculateImageWidth,
  sanitizeEditorFragment,
  serializeEditor
} from '../../src/utils/wysiwygMarkdown.mjs'

const text = data => ({ nodeType: 3, textContent: data, childNodes: [] })

const element = (tagName, children = [], attrs = {}) => {
  const node = {
    nodeType: tagName === '#fragment' ? 11 : 1,
    tagName: tagName === '#fragment' ? undefined : tagName.toUpperCase(),
    childNodes: [],
    dataset: { ...(attrs.dataset || {}) },
    attributes: { ...attrs },
    style: { setProperty(name, value) { this[name] = value } },
    appendChild(child) {
      if (child.nodeType === 11) {
        ;[...child.childNodes].forEach(item => this.appendChild(item))
        return child
      }
      child.parentNode = this
      const previous = this.childNodes.at(-1) || null
      child.previousSibling = previous
      if (previous) previous.nextSibling = child
      this.childNodes.push(child)
      return child
    },
    getAttribute(name) { return this.attributes[name] ?? null },
    setAttribute(name, value) { this.attributes[name] = String(value) },
    matches(selector) {
      return selector === 'img[data-original-src]' && this.tagName === 'IMG' && Boolean(this.dataset.originalSrc)
    }
  }
  children.forEach(child => node.appendChild(child))
  return node
}

const documentRef = {
  createDocumentFragment: () => element('#fragment'),
  createElement: tag => element(tag),
  createTextNode: value => text(value)
}

test('clamps drag width to the supported integer percentage', () => {
  assert.equal(calculateImageWidth(140, 100, 400), 10)
  assert.equal(calculateImageWidth(260, 100, 400), 40)
  assert.equal(calculateImageWidth(700, 100, 400), 100)
  assert.equal(calculateImageWidth(200, 100, 0), 25)
})

test('serializes supported DOM and ignores editor-only controls', () => {
  const image = element('img', [], {
    alt: '图',
    dataset: { originalSrc: 'https://a.test/a.png', imageWidth: '40' }
  })
  const controls = element('span', [], { dataset: { editorUi: 'image-controls' } })
  const root = element('div', [
    element('h2', [text('标题')]),
    element('p', [text('正文'), element('strong', [text('粗体')])]),
    image,
    controls
  ])

  assert.equal(serializeEditor(root), '## 标题\n\n正文**粗体**\n\n![图](https://a.test/a.png){width=40%}')
})

test('serializes lists, code blocks, links and tables as markdown', () => {
  const root = element('div', [
    element('ul', [element('li', [text('甲')]), element('li', [text('乙')])]),
    element('pre', [text('const x = 1')]),
    element('p', [element('a', [text('官网')], { href: 'https://a.test' })]),
    element('table', [
      element('thead', [element('tr', [element('th', [text('A')]), element('th', [text('B')])])]),
      element('tbody', [element('tr', [element('td', [text('1')]), element('td', [text('2')])])])
    ])
  ])

  const markdown = serializeEditor(root)
  assert.match(markdown, /- 甲\n- 乙/)
  assert.match(markdown, /```\nconst x = 1\n```/)
  assert.match(markdown, /\[官网\]\(https:\/\/a.test\)/)
  assert.match(markdown, /\| A \| B \|\n\| --- \| --- \|\n\| 1 \| 2 \|/)
})

test('finds images adjacent to a collapsed caret in element or text containers', () => {
  const image = element('img', [], { dataset: { originalSrc: 'https://a.test/a.png' } })
  const trailingText = text('')
  const parent = element('p', [image, trailingText])

  assert.equal(adjacentImageFromSelection({ isCollapsed: true, anchorNode: parent, anchorOffset: 1 }, 'backward'), image)
  assert.equal(adjacentImageFromSelection({ isCollapsed: true, anchorNode: trailingText, anchorOffset: 0 }, 'backward'), image)
  assert.equal(adjacentImageFromSelection({ isCollapsed: true, anchorNode: parent, anchorOffset: 0 }, 'forward'), image)
  assert.equal(adjacentImageFromSelection({ isCollapsed: false, anchorNode: parent, anchorOffset: 1 }, 'backward'), null)
})

test('sanitizes pasted nodes into a new allowlisted fragment', () => {
  const source = element('div', [
    element('script', [text('alert(1)')]),
    element('a', [text('危险链接')], { href: 'javascript:alert(1)', onclick: 'alert(2)' }),
    element('strong', [text('安全文本')], { onclick: 'alert(3)' }),
    element('img', [], {
      src: 'https://a.test/a.png',
      onerror: 'alert(4)',
      dataset: { originalSrc: 'https://a.test/a.png', imageWidth: '999' }
    })
  ])

  const clean = sanitizeEditorFragment(source, documentRef)
  assert.equal(clean.childNodes.length, 3)
  assert.equal(clean.childNodes[0].nodeType, 3)
  assert.equal(clean.childNodes[0].textContent, '危险链接')
  assert.equal(clean.childNodes[1].tagName, 'STRONG')
  assert.equal(clean.childNodes[1].getAttribute('onclick'), null)
  assert.equal(clean.childNodes[2].tagName, 'IMG')
  assert.equal(clean.childNodes[2].dataset.imageWidth, '25')
  assert.equal(clean.childNodes[2].getAttribute('onerror'), null)
})
