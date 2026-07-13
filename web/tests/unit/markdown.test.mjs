import test from 'node:test'
import assert from 'node:assert/strict'
import { normalizeImageWidth, renderMarkdown } from '../../src/utils/markdown.mjs'

test('renders common markdown while escaping raw html', () => {
  const html = renderMarkdown('# 标题\n\n**粗体** <script>alert(1)</script>')
  assert.match(html, /<h1>标题<\/h1>/)
  assert.match(html, /<strong>粗体<\/strong>/)
  assert.doesNotMatch(html, /<script>/)
  assert.match(html, /&lt;script&gt;/)
})

test('rejects unsafe links and emits placeholder image metadata', () => {
  const html = renderMarkdown('[x](javascript:alert(1)) ![图](https://a.test/a.png)', {
    imagePlaceholder: '/placeholder.png'
  })
  assert.doesNotMatch(html, /javascript:/)
  assert.match(html, /src="\/placeholder.png"/)
  assert.match(html, /data-original-src="https:\/\/a.test\/a.png"/)
  assert.match(html, /data-image-state="loading"/)
})

test('renders persisted image width and defaults invalid widths to 25 percent', () => {
  const resized = renderMarkdown('![图](https://a.test/a.png){width=40%}')
  assert.match(resized, /data-image-width="40"/)
  assert.match(resized, /--markdown-image-width:40%/)

  const unsafe = renderMarkdown('![图](https://a.test/a.png){width=999%}')
  assert.match(unsafe, /data-image-width="25"/)
  assert.doesNotMatch(unsafe, /999/)
  assert.equal(normalizeImageWidth('10'), 10)
  assert.equal(normalizeImageWidth('100'), 100)
  assert.equal(normalizeImageWidth('9'), 25)
  assert.equal(normalizeImageWidth('calc(100)'), 25)
})

test('preserves escaped markdown text and editable metadata', () => {
  const escaped = renderMarkdown('\\*普通星号\\*')
  assert.doesNotMatch(escaped, /<em>/)
  assert.match(escaped, /\*普通星号\*/)

  const link = renderMarkdown('[官网](https://a.test "站点")')
  assert.match(link, /data-markdown-title="站点"/)
  const image = renderMarkdown('![图](https://a.test/a.png "说明"){width=40%}')
  assert.match(image, /data-markdown-title="说明"/)
  assert.match(image, /data-image-width="40"/)

  const list = renderMarkdown('3. 第三项\n4. 第四项')
  assert.match(list, /<ol start="3" data-markdown-start="3">/)
  const table = renderMarkdown('| 左 | 右 |\n| :--- | ---: |\n| 1 | 2 |')
  assert.match(table, /data-markdown-align="left"/)
  assert.match(table, /data-markdown-align="right"/)

  const escapedPipeTable = renderMarkdown('| A\\|B | C |\n| --- | --- |\n| 1 | 2 |')
  assert.equal((escapedPipeTable.match(/<th(?:\s|>)/g) || []).length, 2)
  assert.match(escapedPipeTable, /<th>A\|B<\/th>/)

  const code = renderMarkdown('```java\nclass Main {}\n```')
  assert.match(code, /<pre data-markdown-language="java">/)
})
