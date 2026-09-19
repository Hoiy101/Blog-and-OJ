import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const source = await readFile(
  new URL('../../src/views/user/bot/UserBotIndexView.vue', import.meta.url),
  'utf8'
)

// 编辑和删除的 modal 都写在 v-for 里面，id 必须按 bot.id 区分。
// 静态 id 会让 Bootstrap 的 querySelector 永远命中文档里第一个，
// 于是点任何一篇的删除都会弹出并删掉第一篇。
test('scopes the delete modal and its trigger to each blog id', () => {
  assert.match(source, /:data-bs-target="'#remove-bot-modal-' \+ bot\.id"/)
  assert.match(source, /class="modal fade" :id="'remove-bot-modal-' \+ bot\.id"/)
})

test('keeps the edit modal scoped to each blog id as well', () => {
  assert.match(source, /:data-bs-target="'#update-bot-modal-' \+ bot\.id"/)
  assert.match(source, /class="modal fade" :id="'update-bot-modal-' \+ bot\.id"/)
})

test('never targets a modal inside the blog list by a static id', () => {
  assert.doesNotMatch(source, /#romver/)
  assert.doesNotMatch(source, /data-bs-target="#(remove|update)-bot-modal/)
})

test('hides the modal belonging to the blog that was just deleted', () => {
  assert.match(
    source,
    /const remove_bot = \(bot\) => \{[\s\S]*Modal\.getInstance\('#remove-bot-modal-' \+ bot\.id\)\.hide\(\)/
  )
  assert.match(source, /const remove_bot = \(bot\) => \{[\s\S]*bot_id: bot\.id/)
})

test('confirms the deletion against the blog the user picked', () => {
  assert.match(source, /是否确认删除博客 <strong>\{\{ bot\.title \}\}<\/strong>/)
  assert.match(source, /@click="remove_bot\(bot\)"/)
})
