<template>
  <main class="container admin-page">
    <section class="card admin-card">
      <header class="card-header">
        <div class="d-flex flex-column flex-lg-row gap-3 justify-content-between align-items-lg-center">
          <div>
            <h1 class="h5 mb-1">题库管理</h1>
            <p class="text-muted small mb-0">新增、修改、删除题目并维护判例</p>
          </div>
          <div class="d-flex flex-column flex-sm-row gap-2">
            <input
              v-model="searchKeyword"
              class="form-control"
              type="search"
              placeholder="搜索题目 ID 或标题"
              aria-label="搜索题目"
            >
            <button class="btn btn-primary text-nowrap" @click="openCreate">新增题目</button>
            <button class="btn btn-outline-primary" :disabled="loading" @click="loadTopics">刷新</button>
          </div>
        </div>
      </header>

      <div v-if="loading" class="state-panel">
        <span class="spinner-border spinner-border-sm me-2" aria-hidden="true"></span>
        正在加载题库...
      </div>

      <div v-else-if="error" class="state-panel text-danger">
        <p class="mb-3">{{ error }}</p>
        <button class="btn btn-primary btn-sm" @click="loadTopics">重试</button>
      </div>

      <div v-else-if="filteredTopics.length === 0" class="state-panel text-muted">
        {{ topics.length === 0 ? '暂无题目' : '没有匹配的题目' }}
      </div>

      <div v-else class="table-responsive">
        <table class="table table-hover align-middle mb-0">
          <thead class="table-light">
            <tr>
              <th style="width: 100px">ID</th>
              <th>标题</th>
              <th style="width: 120px">难度</th>
              <th class="text-end" style="min-width: 310px">操作</th>
            </tr>
          </thead>
          <tbody>
            <tr v-for="topic in filteredTopics" :key="topic.id">
              <td><span class="badge bg-light text-dark">#{{ topic.id }}</span></td>
              <td class="fw-semibold">{{ topic.title || '无标题' }}</td>
              <td><span class="badge bg-info text-dark">{{ topic.star || '未设置' }}</span></td>
              <td class="text-end">
                <div class="d-inline-flex flex-wrap justify-content-end gap-2">
                  <button class="btn btn-outline-primary btn-sm" @click="openEdit(topic)">修改题目</button>
                  <button class="btn btn-outline-secondary btn-sm" @click="openEvaluates(topic)">修改判例</button>
                  <button class="btn btn-outline-danger btn-sm" @click="openDelete(topic)">删除题目</button>
                </div>
              </td>
            </tr>
          </tbody>
        </table>
      </div>

      <footer class="card-footer text-muted small">
        显示 {{ filteredTopics.length }} / {{ topics.length }} 道题目
      </footer>
    </section>

    <TopicFormModal
      :visible="showTopicModal"
      :mode="topicMode"
      :topic-id="activeTopic.id"
      :initial-value="topicForm"
      :loading="topicLoading"
      :submitting="topicSubmitting"
      :error="topicModalError"
      @close="closeTopicModal"
      @submit="saveTopic"
    />

    <EvaluateModal
      :visible="showEvaluateModal"
      :topic-id="activeTopic.id"
      :initial-value="evaluateRows"
      :loading="evaluateLoading"
      :submitting="evaluateSubmitting"
      :error="evaluateModalError"
      @close="closeEvaluateModal"
      @submit="saveEvaluates"
    />

    <DeleteTopicModal
      :visible="showDeleteModal"
      :topic="activeTopic"
      :submitting="deleteSubmitting"
      :error="deleteModalError"
      @close="closeDeleteModal"
      @confirm="deleteTopic"
    />
  </main>
</template>

<script>
import { computed, onMounted, ref } from 'vue'
import { useStore } from 'vuex'
import { adminApi, requestErrorMessage } from '@/api/admin.mjs'
import {
  emptyTopicForm,
  evaluatePayload,
  topicPayload,
  topicToForm
} from '@/utils/admin.mjs'
import TopicFormModal from '@/components/manage/TopicFormModal.vue'
import EvaluateModal from '@/components/manage/EvaluateModal.vue'
import DeleteTopicModal from '@/components/manage/DeleteTopicModal.vue'

export default {
  name: 'ManageTopicsView',
  components: { TopicFormModal, EvaluateModal, DeleteTopicModal },
  setup() {
    const store = useStore()
    const topics = ref([])
    const loading = ref(false)
    const error = ref('')
    const searchKeyword = ref('')
    const activeTopic = ref({ id: '', title: '' })

    const showTopicModal = ref(false)
    const topicMode = ref('create')
    const topicForm = ref(emptyTopicForm())
    const topicLoading = ref(false)
    const topicSubmitting = ref(false)
    const topicModalError = ref('')
    const topicRequestId = ref(0)

    const showEvaluateModal = ref(false)
    const evaluateRows = ref([])
    const evaluateLoading = ref(false)
    const evaluateSubmitting = ref(false)
    const evaluateModalError = ref('')
    const evaluateRequestId = ref(0)

    const showDeleteModal = ref(false)
    const deleteSubmitting = ref(false)
    const deleteModalError = ref('')

    const filteredTopics = computed(() => {
      const keyword = searchKeyword.value.trim().toLowerCase()
      if (!keyword) return topics.value
      return topics.value.filter(topic => (
        String(topic.id ?? '').includes(keyword)
        || String(topic.title ?? '').toLowerCase().includes(keyword)
      ))
    })

    const loadTopics = async () => {
      loading.value = true
      error.value = ''
      try {
        const resp = await adminApi.listTopics(store.state.user.token)
        topics.value = Array.isArray(resp) ? resp : []
      } catch (requestError) {
        error.value = requestErrorMessage(requestError)
      } finally {
        loading.value = false
      }
    }

    const openCreate = () => {
      topicRequestId.value += 1
      activeTopic.value = { id: '', title: '' }
      topicMode.value = 'create'
      topicForm.value = emptyTopicForm()
      topicModalError.value = ''
      topicLoading.value = false
      showTopicModal.value = true
    }

    const openEdit = async topic => {
      const requestId = ++topicRequestId.value
      activeTopic.value = { id: topic.id, title: topic.title }
      topicMode.value = 'edit'
      topicForm.value = emptyTopicForm()
      topicModalError.value = ''
      topicLoading.value = true
      showTopicModal.value = true
      try {
        const resp = await adminApi.getTopic(store.state.user.token, topic.id)
        if (requestId !== topicRequestId.value) return
        topicForm.value = topicToForm(resp)
      } catch (requestError) {
        if (requestId !== topicRequestId.value) return
        topicModalError.value = requestErrorMessage(requestError)
      } finally {
        if (requestId === topicRequestId.value) topicLoading.value = false
      }
    }

    const closeTopicModal = () => {
      if (!topicSubmitting.value) {
        topicRequestId.value += 1
        showTopicModal.value = false
      }
    }

    const saveTopic = async form => {
      topicSubmitting.value = true
      topicModalError.value = ''
      try {
        const payload = topicMode.value === 'create'
          ? topicPayload(form)
          : topicPayload(form, activeTopic.value.id)
        const resp = topicMode.value === 'create'
          ? await adminApi.addTopic(store.state.user.token, payload)
          : await adminApi.updateTopic(store.state.user.token, payload)

        if (resp.error_message !== 'success') {
          topicModalError.value = resp.error_message || '保存题目失败'
          return
        }
        showTopicModal.value = false
        await loadTopics()
      } catch (requestError) {
        topicModalError.value = requestErrorMessage(requestError)
      } finally {
        topicSubmitting.value = false
      }
    }

    const openEvaluates = async topic => {
      const requestId = ++evaluateRequestId.value
      activeTopic.value = { id: topic.id, title: topic.title }
      evaluateRows.value = []
      evaluateModalError.value = ''
      evaluateLoading.value = true
      showEvaluateModal.value = true
      try {
        const resp = await adminApi.getEvaluates(store.state.user.token, topic.id)
        if (requestId !== evaluateRequestId.value) return
        evaluateRows.value = Array.isArray(resp)
          ? resp.map(row => ({
            id: row.id == null ? '' : String(row.id),
            input: row.input == null ? '' : String(row.input),
            output: row.output == null ? '' : String(row.output)
          }))
          : []
      } catch (requestError) {
        if (requestId !== evaluateRequestId.value) return
        evaluateModalError.value = requestErrorMessage(requestError)
      } finally {
        if (requestId === evaluateRequestId.value) evaluateLoading.value = false
      }
    }

    const closeEvaluateModal = () => {
      if (!evaluateSubmitting.value) {
        evaluateRequestId.value += 1
        showEvaluateModal.value = false
      }
    }

    const saveEvaluates = async rows => {
      if (rows.some(row => !String(row.input).trim() || !String(row.output).trim())) {
        evaluateModalError.value = '判例输入和输出不能为空'
        return
      }

      evaluateSubmitting.value = true
      evaluateModalError.value = ''
      try {
        const payload = evaluatePayload(rows, activeTopic.value.id)
        const resp = await adminApi.updateEvaluates(store.state.user.token, payload)
        if (resp.error_message !== 'success') {
          evaluateModalError.value = resp.error_message || '保存判例失败'
          return
        }
        showEvaluateModal.value = false
      } catch (requestError) {
        evaluateModalError.value = requestErrorMessage(requestError)
      } finally {
        evaluateSubmitting.value = false
      }
    }

    const openDelete = topic => {
      activeTopic.value = { id: topic.id, title: topic.title }
      deleteModalError.value = ''
      showDeleteModal.value = true
    }

    const closeDeleteModal = () => {
      if (!deleteSubmitting.value) showDeleteModal.value = false
    }

    const deleteTopic = async () => {
      deleteSubmitting.value = true
      deleteModalError.value = ''
      try {
        const resp = await adminApi.removeTopic(store.state.user.token, activeTopic.value.id)
        if (resp.error_message !== 'success') {
          deleteModalError.value = resp.error_message || '删除题目失败'
          return
        }
        showDeleteModal.value = false
        await loadTopics()
      } catch (requestError) {
        deleteModalError.value = requestErrorMessage(requestError)
      } finally {
        deleteSubmitting.value = false
      }
    }

    onMounted(loadTopics)

    return {
      topics, filteredTopics, loading, error, searchKeyword, activeTopic,
      showTopicModal, topicMode, topicForm, topicLoading, topicSubmitting, topicModalError,
      showEvaluateModal, evaluateRows, evaluateLoading, evaluateSubmitting, evaluateModalError,
      showDeleteModal, deleteSubmitting, deleteModalError,
      loadTopics, openCreate, openEdit, closeTopicModal, saveTopic,
      openEvaluates, closeEvaluateModal, saveEvaluates,
      openDelete, closeDeleteModal, deleteTopic
    }
  }
}
</script>

<style scoped>
.admin-page { padding-top: 2rem; padding-bottom: 3rem; }
.admin-card { border: 0; border-radius: 14px; box-shadow: 0 10px 30px rgba(15, 23, 42, .08); overflow: hidden; }
.card-header { padding: 1.25rem 1.5rem; background: #fff; }
.card-footer { padding: .85rem 1.25rem; background: #fff; }
.state-panel { padding: 4rem 1.5rem; text-align: center; }
th, td { padding: 1rem 1.25rem; }
</style>
