<template>
  <main class="admin-page-shell">
    <div class="container admin-page-container">
      <header class="status-toolbar">
        <div>
          <h1 class="h5 mb-1">运行状态</h1>
          <p class="text-muted small mb-0">
            判题服务的实时负载，以及 backend 调用判题服务时的熔断器状态
          </p>
        </div>
        <div class="status-toolbar-actions">
          <span class="text-muted small">{{ updatedAtText }}</span>
          <div class="form-check form-switch mb-0">
            <input
              id="auto-refresh"
              v-model="autoRefresh"
              class="form-check-input"
              type="checkbox"
            >
            <label class="form-check-label small" for="auto-refresh">自动刷新</label>
          </div>
          <button class="btn btn-outline-primary btn-sm" :disabled="loading" @click="loadStatus">
            <span v-if="loading" class="spinner-border spinner-border-sm me-1" aria-hidden="true"></span>
            刷新
          </button>
        </div>
      </header>

      <div v-if="error" class="alert alert-danger" role="alert">
        {{ error }}
      </div>

      <div class="row g-3">
        <div class="col-12 col-lg-6">
          <section class="card admin-card h-100">
            <header class="card-header">
              <h2 class="h6 mb-0">
                <i class="bi bi-cpu me-2"></i>判题服务
              </h2>
            </header>

            <div v-if="initialLoading" class="state-panel text-muted">
              <span class="spinner-border spinner-border-sm me-2" aria-hidden="true"></span>
              正在读取判题服务状态...
            </div>

            <div v-else class="card-body">
              <div class="status-headline">
                <span class="badge" :class="judgeOnline ? 'bg-success' : 'bg-danger'">
                  {{ judgeOnline ? '在线' : '离线' }}
                </span>
                <span v-if="judge.message" class="text-muted small">{{ judge.message }}</span>
              </div>

              <dl class="status-metrics">
                <div>
                  <dt>排队任务</dt>
                  <dd>{{ metricText(judge.queues) }}</dd>
                </div>
                <div>
                  <dt>运行中</dt>
                  <dd>{{ metricText(judge.running) }}</dd>
                </div>
              </dl>

              <p class="text-muted small mb-0">
                数据来自判题服务的 /status/ 接口，经 OpenFeign 调用；服务不可用时由降级方法返回离线信息。
              </p>
            </div>
          </section>
        </div>

        <div class="col-12 col-lg-6">
          <section class="card admin-card h-100">
            <header class="card-header">
              <h2 class="h6 mb-0">
                <i class="bi bi-shield-exclamation me-2"></i>熔断器
              </h2>
            </header>

            <div v-if="initialLoading" class="state-panel text-muted">
              <span class="spinner-border spinner-border-sm me-2" aria-hidden="true"></span>
              正在读取熔断器状态...
            </div>

            <div v-else-if="breakers.length === 0" class="state-panel text-muted">
              尚未产生远程调用，熔断器还没有被创建
            </div>

            <div v-else class="card-body">
              <div v-for="breaker in breakers" :key="breaker.name" class="breaker-block">
                <div class="status-headline">
                  <span class="badge" :class="circuitBreakerState(breaker.state).badge">
                    {{ circuitBreakerState(breaker.state).label }}
                  </span>
                  <code class="small text-muted">{{ breaker.name }}</code>
                </div>

                <p class="text-muted small mb-2">{{ circuitBreakerState(breaker.state).hint }}</p>

                <dl class="status-metrics">
                  <div>
                    <dt>失败率</dt>
                    <dd>{{ formatFailureRate(breaker.failure_rate) }}</dd>
                  </div>
                  <div>
                    <dt>窗口内调用</dt>
                    <dd>{{ metricText(breaker.buffered_calls) }}</dd>
                  </div>
                  <div>
                    <dt>失败 / 成功</dt>
                    <dd>{{ metricText(breaker.failed_calls) }} / {{ metricText(breaker.successful_calls) }}</dd>
                  </div>
                  <div>
                    <dt>已拦截</dt>
                    <dd>{{ metricText(breaker.not_permitted_calls) }}</dd>
                  </div>
                </dl>

                <p class="text-muted small mb-0">
                  阈值：窗口 {{ breaker.sliding_window_size }} 次，
                  至少 {{ breaker.minimum_number_of_calls }} 次调用后，
                  失败率超过 {{ breaker.failure_rate_threshold }}% 熔断
                  {{ breaker.wait_duration_in_open_state }} 秒
                </p>
              </div>
            </div>
          </section>
        </div>
      </div>
    </div>
  </main>
</template>

<script>
import { computed, onMounted, onUnmounted, ref, watch } from 'vue'
import { useStore } from 'vuex'
import { adminApi, requestErrorMessage } from '@/api/admin.mjs'
import { circuitBreakerState, formatFailureRate } from '@/utils/admin.mjs'

const REFRESH_INTERVAL = 5000

export default {
  name: 'ManageRuntimeStatusView',
  setup() {
    const store = useStore()
    const judge = ref({})
    const breakers = ref([])
    const loading = ref(false)
    const initialLoading = ref(true)
    const error = ref('')
    const updatedAt = ref(null)
    const autoRefresh = ref(false)
    let timer = null

    const judgeOnline = computed(() => judge.value.online === true || judge.value.online === 'true')

    const updatedAtText = computed(() =>
      updatedAt.value ? `更新于 ${updatedAt.value.toLocaleTimeString()}` : '尚未更新'
    )

    const metricText = value => value === undefined || value === null ? '—' : value

    const loadStatus = async () => {
      loading.value = true
      error.value = ''
      try {
        const [judgeResp, breakerResp] = await Promise.all([
          adminApi.judgeStatus(store.state.user.token),
          adminApi.judgeCircuitBreaker(store.state.user.token)
        ])
        judge.value = judgeResp && typeof judgeResp === 'object' ? judgeResp : {}
        breakers.value = Array.isArray(breakerResp) ? breakerResp : []
        updatedAt.value = new Date()
      } catch (requestError) {
        error.value = requestErrorMessage(requestError)
      } finally {
        loading.value = false
        initialLoading.value = false
      }
    }

    const stopTimer = () => {
      if (timer !== null) {
        clearInterval(timer)
        timer = null
      }
    }

    watch(autoRefresh, enabled => {
      stopTimer()
      if (enabled) {
        timer = setInterval(loadStatus, REFRESH_INTERVAL)
      }
    })

    onMounted(loadStatus)
    onUnmounted(stopTimer)

    return {
      judge,
      breakers,
      loading,
      initialLoading,
      error,
      autoRefresh,
      judgeOnline,
      updatedAtText,
      metricText,
      loadStatus,
      circuitBreakerState,
      formatFailureRate
    }
  }
}
</script>

<style scoped>
.admin-page-shell { min-height: calc(100vh - 56px); }
.admin-page-container { padding-top: 1.5rem; padding-bottom: 2.5rem; }
.admin-card { border: 1px solid rgba(148, 163, 184, .24); border-radius: 14px; box-shadow: 0 10px 30px rgba(15, 23, 42, .08); overflow: hidden; }
.card-header { padding: 1rem 1.25rem; background: #fff; }
.card-body { padding: 1.25rem; }
.state-panel { padding: 3rem 1.25rem; text-align: center; }
.status-toolbar { display: flex; flex-wrap: wrap; gap: .75rem; justify-content: space-between; align-items: flex-end; margin-bottom: 1rem; }
.status-toolbar-actions { display: flex; align-items: center; gap: .9rem; }
.status-headline { display: flex; align-items: center; gap: .6rem; flex-wrap: wrap; margin-bottom: .5rem; }
.status-metrics { display: grid; grid-template-columns: repeat(2, minmax(0, 1fr)); gap: .75rem 1rem; margin: 0 0 1rem; }
.status-metrics dt { font-size: .8rem; font-weight: 500; color: var(--app-text-muted, #64748b); }
.status-metrics dd { margin: 0; font-size: 1.25rem; font-weight: 600; }
.breaker-block + .breaker-block { margin-top: 1.25rem; padding-top: 1.25rem; border-top: 1px solid rgba(148, 163, 184, .24); }
@media (max-width: 575.98px) {
  .admin-page-container { padding-top: .75rem; padding-bottom: 1.25rem; }
  .status-toolbar-actions { width: 100%; justify-content: space-between; }
}
</style>
