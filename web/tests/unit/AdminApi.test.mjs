import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(
  new URL('../../src/api/admin.mjs', import.meta.url),
  'utf8'
)

const endpointContracts = [
  ['listUsers', '/manage/user/gitlist/', 'GET'],
  ['updateBanned', '/user/account/banned/', 'GET'],
  ['listLoginRecords', '/manage/record/login/', 'GET'],
  ['listTopics', '/oj/topic/getlist/', 'GET'],
  ['getTopic', '/oj/topic/get/', 'GET'],
  ['addTopic', '/oj/topic/add/', 'POST'],
  ['updateTopic', '/oj/topic/updata/', 'POST'],
  ['removeTopic', '/oj/topic/remove/', 'POST'],
  ['getEvaluates', '/manage/evaluate/get/', 'POST'],
  ['updateEvaluates', '/manage/evaluate/updata/', 'POST']
]

test('declares every management endpoint and HTTP method', () => {
  for (const [name, endpoint, method] of endpointContracts) {
    assert.match(source, new RegExp(`${name}:.*?${endpoint.replaceAll('/', '\\/')}.*?${method}`, 's'))
  }
})

test('attaches bearer authentication without forwarding the internal token option', () => {
  assert.match(source, /Authorization:\s*`Bearer \$\{token\}`/)
  assert.match(source, /const \{ token, \.\.\.ajaxOptions \} = options/)
  assert.match(source, /\$\.ajax\(\{[\s\S]*\.\.\.ajaxOptions/)
})

test('uses JSON only for evaluate updates and form data for other mutations', () => {
  assert.match(source, /updateBanned:[\s\S]*data:\s*\{ username, status \}/)
  assert.match(source, /removeTopic:[\s\S]*data:\s*\{ topic_id: topicId \}/)
  assert.match(source, /getEvaluates:[\s\S]*data:\s*\{ topic_id: topicId \}/)
  assert.match(source, /updateEvaluates:[\s\S]*contentType:\s*['"]application\/json; charset=UTF-8['"]/)
  assert.match(source, /updateEvaluates:[\s\S]*data:\s*JSON\.stringify\(payload\)/)
})

test('normalizes backend, authentication, and connection errors', () => {
  assert.match(source, /error\?\.responseJSON\?\.error_message/)
  assert.match(source, /error\?\.status === 401/)
  assert.match(source, /登录状态已失效/)
  assert.match(source, /无法连接到服务器/)
})
