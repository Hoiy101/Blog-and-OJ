<template>
  <main class="admin-page-shell">
    <div class="container admin-page-container">
      <section class="card admin-card">
      <header class="card-header d-flex justify-content-between align-items-center">
        <div>
          <h1 class="h5 mb-1">用户管理</h1>
          <p class="text-muted small mb-0">查看用户权限并管理账号封禁状态</p>
        </div>
        <button class="btn btn-outline-primary btn-sm" :disabled="loading" @click="loadUsers">
          刷新
        </button>
      </header>

      <div v-if="loading" class="state-panel">
        <span class="spinner-border spinner-border-sm me-2" aria-hidden="true"></span>
        正在加载用户数据...
      </div>

      <div v-else-if="error" class="state-panel text-danger">
        <p class="mb-3">{{ error }}</p>
        <button class="btn btn-primary btn-sm" @click="loadUsers">重试</button>
      </div>

      <div v-else-if="users.length === 0" class="state-panel text-muted">
        暂无用户数据
      </div>

      <div v-else class="table-responsive">
        <table class="table table-hover align-middle mb-0">
          <thead class="table-light">
            <tr>
              <th>用户名</th>
              <th>身份</th>
              <th>账号状态</th>
              <th class="text-end">操作</th>
            </tr>
          </thead>
          <tbody>
            <tr v-for="user in users" :key="user.username">
              <td class="fw-semibold">{{ user.username }}</td>
              <td>
                <span class="badge" :class="user.root === 'true' ? 'bg-primary' : 'bg-secondary'">
                  {{ user.root === 'true' ? '管理员' : '普通用户' }}
                </span>
              </td>
              <td>
                <span class="badge" :class="user.banned === 'true' ? 'bg-danger' : 'bg-success'">
                  {{ user.banned === 'true' ? '已封禁' : '正常' }}
                </span>
              </td>
              <td class="text-end">
                <button
                  class="btn btn-sm"
                  :class="user.banned === 'true' ? 'btn-outline-success' : 'btn-outline-danger'"
                  :disabled="updatingUsername === user.username"
                  @click="changeBannedState(user)"
                >
                  <span
                    v-if="updatingUsername === user.username"
                    class="spinner-border spinner-border-sm me-1"
                    aria-hidden="true"
                  ></span>
                  {{ user.banned === 'true' ? '解除封禁' : '封禁用户' }}
                </button>
              </td>
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
import { statusForBannedState } from '@/utils/admin.mjs'

export default {
  name: 'ManageUsersView',
  setup() {
    const store = useStore()
    const users = ref([])
    const loading = ref(false)
    const error = ref('')
    const updatingUsername = ref('')

    const loadUsers = async () => {
      loading.value = true
      error.value = ''
      try {
        const resp = await adminApi.listUsers(store.state.user.token)
        users.value = Array.isArray(resp) ? resp : []
      } catch (requestError) {
        error.value = requestErrorMessage(requestError)
      } finally {
        loading.value = false
      }
    }

    const changeBannedState = async user => {
      const banned = user.banned === 'true'
      const status = statusForBannedState(banned)
      updatingUsername.value = user.username
      error.value = ''

      try {
        const resp = await adminApi.updateBanned(
          store.state.user.token,
          user.username,
          status
        )
        if (resp.error_message !== 'success') {
          error.value = resp.error_message || '修改用户状态失败'
          return
        }
        await loadUsers()
      } catch (requestError) {
        error.value = requestErrorMessage(requestError)
      } finally {
        updatingUsername.value = ''
      }
    }

    onMounted(loadUsers)

    return { users, loading, error, updatingUsername, loadUsers, changeBannedState }
  }
}
</script>

<style scoped>
.admin-page-shell { min-height: calc(100vh - 56px); background: #f4f7fb; }
.admin-page-container { padding-top: 1.5rem; padding-bottom: 2.5rem; }
.admin-card { border: 1px solid rgba(148, 163, 184, .24); border-radius: 14px; box-shadow: 0 10px 30px rgba(15, 23, 42, .08); overflow: hidden; }
.card-header { padding: 1.25rem 1.5rem; background: #fff; }
.state-panel { padding: 4rem 1.5rem; text-align: center; }
th, td { padding: 1rem 1.25rem; }
@media (max-width: 575.98px) {
  .admin-page-container { padding-top: .75rem; padding-bottom: 1.25rem; }
}
</style>
