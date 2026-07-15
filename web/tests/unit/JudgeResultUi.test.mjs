import test from 'node:test'
import assert from 'node:assert/strict'
import { readFile } from 'node:fs/promises'

const questionSource = await readFile(
  new URL('../../src/views/ranklist/QuestionDetails.vue', import.meta.url),
  'utf8'
)
const modalSource = await readFile(
  new URL('../../src/components/ResultModal.vue', import.meta.url),
  'utf8'
)

test('uses only the submit button as the pending judge indicator', () => {
  assert.match(questionSource, /v-if="isSubmitting"/)
  assert.doesNotMatch(questionSource, /submissionResult/)
  assert.doesNotMatch(questionSource, /提交结果/)
  assert.doesNotMatch(questionSource, /getResultAlertClass/)
})

test('keeps waiting after HTTP success and stops on HTTP error or a judge result', () => {
  assert.doesNotMatch(questionSource, /complete\(\)[\s\S]{0,100}isSubmitting\.value = false/)
  assert.match(questionSource, /error\(xhr\)[\s\S]{0,240}isSubmitting\.value = false/)
  assert.match(questionSource, /isJudgeResult\(data\)[\s\S]{0,240}isSubmitting\.value = false/)
  assert.match(questionSource, /toSubmissionErrorResult\(extractSubmissionError\(xhr\)\)/)
  assert.match(questionSource, /toJudgeModalResult\(data\)/)
})

test('renders submission errors without score, record details, or solution action', () => {
  assert.match(modalSource, /isSubmissionError/)
  assert.match(modalSource, /v-if="!isSubmissionError" class="result-score"/)
  assert.match(modalSource, /v-if="!isSubmissionError" class="result-details"/)
  assert.match(modalSource, /resultData\.state !== 'accepted' && !isSubmissionError/)
  assert.match(modalSource, /submission_error: '提交失败'/)
  assert.match(modalSource, /props\.resultData\.message/)
})
