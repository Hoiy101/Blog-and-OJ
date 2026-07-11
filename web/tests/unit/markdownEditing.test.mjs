import test from 'node:test'
import assert from 'node:assert/strict'
import { imageMarkdown, insertAtSelection } from '../../src/utils/markdownEditing.mjs'

test('inserts uploaded image markdown at selection', () => {
  const snippet = imageMarkdown('示意图', 'https://a.test/x.png')
  assert.equal(insertAtSelection('前后', snippet, 1, 1).value, '前\n![示意图](https://a.test/x.png)\n后')
})

test('only appends width metadata for a non-default image width', () => {
  assert.equal(imageMarkdown('图', 'https://a.test/a.png', 25), '\n![图](https://a.test/a.png)\n')
  assert.equal(imageMarkdown('图', 'https://a.test/a.png', 40), '\n![图](https://a.test/a.png){width=40%}\n')
})
