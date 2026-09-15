// 后端统一入口（Spring Cloud Gateway）。可在 .env.development 里用 VUE_APP_API_BASE 覆盖。
export const API_BASE = process.env.VUE_APP_API_BASE || 'http://127.0.0.1:8000'
// WebSocket 与 HTTP 同源：http → ws、https → wss
export const WS_BASE = API_BASE.replace(/^http/, 'ws')
