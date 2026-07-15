import test from 'node:test'
import assert from 'node:assert/strict'
import {
  extractSubmissionError,
  isJudgeResult,
  toJudgeModalResult,
  toSubmissionErrorResult
} from '../../src/utils/judgeSubmission.mjs'

test('extracts an HTTP submission error using the documented priority', () => {
  assert.equal(extractSubmissionError({
    responseJSON: { message: '登录已过期', error: 'ignored' },
    responseText: 'also ignored'
  }), '登录已过期')
  assert.equal(extractSubmissionError({ responseJSON: { error: '请求被拒绝' } }), '请求被拒绝')
  assert.equal(extractSubmissionError({ responseText: '网络错误' }), '网络错误')
  assert.equal(extractSubmissionError({ responseJSON: { message: '   ' } }), '提交失败，请稍后重试')
})

test('accepts only WebSocket messages containing score and state', () => {
  assert.equal(isJudgeResult({ score: 100, state: 'accepted' }), true)
  assert.equal(isJudgeResult({ score: 0 }), false)
  assert.equal(isJudgeResult(null), false)
})

test('creates normalized modal data for judge results and submission errors', () => {
  assert.deepEqual(toJudgeModalResult({
    user_id: 7,
    evaluation_id: 9,
    score: 100,
    state: 'accepted'
  }), {
    user_id: 7,
    evaluation_id: 9,
    score: 100,
    state: 'accepted',
    message: ''
  })
  assert.deepEqual(toSubmissionErrorResult('服务不可用'), {
    user_id: null,
    evaluation_id: null,
    score: null,
    state: 'submission_error',
    message: '服务不可用'
  })
})
