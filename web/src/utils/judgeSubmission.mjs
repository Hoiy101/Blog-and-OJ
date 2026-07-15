const FALLBACK_SUBMISSION_ERROR = '提交失败，请稍后重试'

function nonEmptyString(value) {
  return typeof value === 'string' && value.trim() ? value.trim() : ''
}

export function extractSubmissionError(xhr = {}) {
  return nonEmptyString(xhr?.responseJSON?.message)
    || nonEmptyString(xhr?.responseJSON?.error)
    || nonEmptyString(xhr?.responseText)
    || FALLBACK_SUBMISSION_ERROR
}

export function isJudgeResult(data) {
  return Boolean(data && data.score !== undefined && data.state !== undefined)
}

export function toJudgeModalResult(data) {
  return {
    user_id: data.user_id,
    evaluation_id: data.evaluation_id,
    score: data.score,
    state: data.state,
    message: ''
  }
}

export function toSubmissionErrorResult(message) {
  return {
    user_id: null,
    evaluation_id: null,
    score: null,
    state: 'submission_error',
    message: nonEmptyString(message) || FALLBACK_SUBMISSION_ERROR
  }
}
