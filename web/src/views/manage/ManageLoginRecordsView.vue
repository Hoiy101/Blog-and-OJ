<template>
  <main class="admin-page-shell">
    <div class="container admin-page-container">
      <section class="card admin-card">
      <header class="card-header d-flex justify-content-between align-items-center">
        <div>
          <h1 class="h5 mb-1">用户登录信息</h1>
          <p class="text-muted small mb-0">按时间展示最近 100 条登录记录</p>
        </div>
        <button class="btn btn-outline-primary btn-sm" :disabled="loading" @click="loadRecords">
          刷新
        </button>
      </header>

      <div v-if="loading" class="state-panel">
        <span class="spinner-border spinner-border-sm me-2" aria-hidden="true"></span>
        正在加载登录记录...
      </div>

      <div v-else-if="error" class="state-panel text-danger">
        <p class="mb-3">{{ error }}</p>
        <button class="btn btn-primary btn-sm" @click="loadRecords">重试</button>
      </div>

      <div v-else-if="records.length === 0" class="state-panel text-muted">
        暂无登录记录
      </div>

      <div v-else class="table-responsive">
        <table class="table table-hover align-middle mb-0">
          <thead class="table-light">
            <tr>
              <th>用户名</th>
              <th>登录 IP</th>
              <th>登录时间</th>
            </tr>
          </thead>
          <tbody>
            <tr v-for="record in records" :key="record.id">
              <td class="fw-semibold">{{ record.username }}</td>
              <td><code>{{ record.ip || '-' }}</code></td>
              <td>{{ record.time || '-' }}</td>
            </tr>
          </tbody>
        </table>
      </div>
      </section>
    </div>
  </main>
</template>

<script>
import { onMounted, ref } from 'vue'
import { useStore } from 'vuex'
import { adminApi, requestErrorMessage } from '@/api/admin.mjs'
import { latestLoginRecords } from '@/utils/admin.mjs'

export default {
  name: 'ManageLoginRecordsView',
  setup() {
    const store = useStore()
    const records = ref([])
    const loading = ref(false)
    const error = ref('')

    const loadRecords = async () => {
      loading.value = true
      error.value = ''
      try {
        const resp = await adminApi.listLoginRecords(store.state.user.token)
        records.value = latestLoginRecords(resp, 100)
      } catch (requestError) {
        error.value = requestErrorMessage(requestError)
      } finally {
        loading.value = false
      }
    }

    onMounted(loadRecords)

    return { records, loading, error, loadRecords }
  }
}
</script>

<style scoped>
.admin-page-shell { min-height: calc(100vh - 56px); background: #f4f7fb; }
.admin-page-container { padding-top: 1.5rem; padding-bottom: 2.5rem; }
.admin-card { border: 1px solid rgba(148, 163, 184, .24); border-radius: 14px; box-shadow: 0 10px 30px rgba(15, 23, 42, .08); overflow: hidden; }
.card-header { padding: 1.25rem 1.5rem; background: #fff; }
.state-panel { padding: 4rem 1.5rem; text-align: center; }
th, td { padding: 1rem 1.25rem; white-space: nowrap; }
@media (max-width: 575.98px) {
  .admin-page-container { padding-top: .75rem; padding-bottom: 1.25rem; }
}
</style>
