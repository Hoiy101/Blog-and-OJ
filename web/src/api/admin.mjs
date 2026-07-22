import $ from 'jquery'

const API_BASE = 'http://127.0.0.1:3000'

const request = options => new Promise((resolve, reject) => {
  const { token, ...ajaxOptions } = options

  $.ajax({
    ...ajaxOptions,
    url: `${API_BASE}${ajaxOptions.url}`,
    headers: {
      ...ajaxOptions.headers,
      Authorization: `Bearer ${token}`
    },
    success: resolve,
    error: reject
  })
})

export const requestErrorMessage = error => {
  if (error?.responseJSON?.error_message) {
    return error.responseJSON.error_message
  }
  if (error?.status === 401) {
    return '登录状态已失效，请重新登录'
  }
  if (error?.status === 0) {
    return '无法连接到服务器'
  }
  return error?.statusText || '请求失败，请稍后重试'
}

export const adminApi = {
  listUsers: token => request({
    url: '/manage/user/gitlist/',
    type: 'GET',
    token
  }),

  updateBanned: (token, username, status) => request({
    url: '/user/account/banned/',
    type: 'GET',
    token,
    data: { username, status }
  }),

  listLoginRecords: token => request({
    url: '/manage/record/login/',
    type: 'GET',
    token
  }),

  listTopics: token => request({
    url: '/oj/topic/getlist/',
    type: 'GET',
    token
  }),

  getTopic: (token, id) => request({
    url: '/oj/topic/get/',
    type: 'GET',
    token,
    data: { id }
  }),

  addTopic: (token, payload) => request({
    url: '/oj/topic/add/',
    type: 'POST',
    token,
    data: payload
  }),

  updateTopic: (token, payload) => request({
    url: '/oj/topic/updata/',
    type: 'POST',
    token,
    data: payload
  }),

  removeTopic: (token, topicId) => request({
    url: '/oj/topic/remove/',
    type: 'POST',
    token,
    data: { topic_id: topicId }
  }),

  getEvaluates: (token, topicId) => request({
    url: '/manage/evaluate/get/',
    type: 'POST',
    token,
    data: { topic_id: topicId }
  }),

  updateEvaluates: (token, payload) => request({
    url: '/manage/evaluate/updata/',
    type: 'POST',
    token,
    contentType: 'application/json; charset=UTF-8',
    data: JSON.stringify(payload)
  })
}
