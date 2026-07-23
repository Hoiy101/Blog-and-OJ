<template>
  <template v-if="visible">
    <div class="modal d-block" tabindex="-1" role="dialog" aria-modal="true" aria-labelledby="evaluate-title">
      <div class="modal-dialog modal-xl modal-dialog-scrollable">
        <div class="modal-content">
          <div class="modal-header">
            <div>
              <h2 id="evaluate-title" class="modal-title fs-5">修改判例</h2>
              <p class="text-muted small mb-0 mt-1">题目 ID：{{ topicId }}</p>
            </div>
            <button type="button" class="btn-close" :disabled="submitting" aria-label="关闭" @click="requestClose"></button>
          </div>

          <form class="admin-modal-form" @submit.prevent="submitRows">
            <div class="modal-body admin-modal-body">
              <div v-if="loading" class="modal-state">
                <span class="spinner-border spinner-border-sm me-2" aria-hidden="true"></span>
                正在加载判例...
              </div>

              <template v-else>
                <div v-if="error" class="alert alert-danger" role="alert">{{ error }}</div>

                <div v-if="rows.length === 0" class="empty-cases">
                  暂无判例，请点击“添加判例”创建第一条。
                </div>

                <article v-for="(row, index) in rows" :key="row.id || `new-${index}`" class="case-card">
                  <div class="d-flex justify-content-between align-items-center mb-3">
                    <h3 class="h6 mb-0">判例 {{ index + 1 }}</h3>
                    <div class="d-flex align-items-center gap-2">
                      <span v-if="row.id" class="badge bg-light text-dark">ID: {{ row.id }}</span>
                      <span v-else class="badge bg-info text-dark">新增</span>
                      <button
                        type="button"
                        class="btn btn-sm btn-link text-danger p-0 case-remove"
                        :aria-label="`删除判例 ${index + 1}`"
                        :disabled="submitting || loading"
                        @click="removeRow(index)"
                      >
                        &times;
                      </button>
                    </div>
                  </div>
                  <div class="row g-3">
                    <div class="col-md-6">
                      <label class="form-label" :for="`case-input-${index}`">输入</label>
                      <textarea
                        :id="`case-input-${index}`"
                        v-model="row.input"
                        class="form-control font-monospace"
                        rows="5"
                        required
                      ></textarea>
                    </div>
                    <div class="col-md-6">
                      <label class="form-label" :for="`case-output-${index}`">输出</label>
                      <textarea
                        :id="`case-output-${index}`"
                        v-model="row.output"
                        class="form-control font-monospace"
                        rows="5"
                        required
                      ></textarea>
                    </div>
                  </div>
                </article>

                <button type="button" class="btn btn-outline-primary w-100" :disabled="submitting" @click="addRow">
                  添加判例
                </button>
              </template>
            </div>

            <div class="modal-footer">
              <button type="button" class="btn btn-outline-secondary" :disabled="submitting" @click="requestClose">取消</button>
              <button type="submit" class="btn btn-primary" :disabled="submitting || loading">
                <span v-if="submitting" class="spinner-border spinner-border-sm me-2" aria-hidden="true"></span>
                保存判例
              </button>
            </div>
          </form>
        </div>
      </div>
    </div>
    <div class="modal-backdrop show"></div>
  </template>
</template>

<script>
import { ref, watch } from 'vue'

export default {
  name: 'EvaluateModal',
  props: {
    visible: { type: Boolean, default: false },
    topicId: { type: [String, Number], default: '' },
    initialValue: { type: Array, default: () => [] },
    loading: { type: Boolean, default: false },
    submitting: { type: Boolean, default: false },
    error: { type: String, default: '' }
  },
  emits: ['close', 'submit'],
  setup(props, { emit }) {
    const rows = ref([])

    const resetRows = () => {
      rows.value = props.initialValue.map(row => ({ ...row }))
    }

    watch(
      () => [props.visible, props.initialValue],
      () => {
        if (props.visible) resetRows()
      },
      { immediate: true, deep: true }
    )

    const addRow = () => {
      rows.value.push({ id: '', input: '', output: '' })
    }

    const removeRow = index => {
      rows.value.splice(index, 1)
    }

    const requestClose = () => {
      if (!props.submitting) emit('close')
    }

    const submitRows = () => {
      if (!props.loading && !props.submitting) {
        emit('submit', rows.value.map(row => ({ ...row })))
      }
    }

    return { rows, addRow, removeRow, requestClose, submitRows }
  }
}
</script>

<style scoped>
.modal { z-index: 1060; }
.modal-backdrop { z-index: 1055; }
.modal-dialog { margin-top: 1rem; margin-bottom: 1rem; }
.modal-content { max-height: calc(100vh - 2rem); overflow: hidden; }
.admin-modal-form { display: flex; min-height: 0; flex: 1 1 auto; flex-direction: column; }
.admin-modal-body { min-height: 0; overflow-y: auto; overscroll-behavior: contain; }
.modal-header, .modal-footer { flex: 0 0 auto; }
.modal-state, .empty-cases { padding: 4rem 1rem; text-align: center; color: #64748b; }
.case-card { padding: 1rem; margin-bottom: 1rem; border: 1px solid #e2e8f0; border-radius: 12px; background: #f8fafc; }
.case-remove { width: 1.75rem; height: 1.75rem; font-size: 1.6rem; line-height: 1; text-decoration: none; }
textarea { resize: vertical; }
</style>
