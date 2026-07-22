<template>
  <template v-if="visible">
    <div class="modal d-block" tabindex="-1" role="dialog" aria-modal="true" aria-labelledby="topic-form-title">
      <div class="modal-dialog modal-xl modal-dialog-scrollable">
        <div class="modal-content">
          <div class="modal-header">
            <div>
              <h2 id="topic-form-title" class="modal-title fs-5">
                {{ mode === 'create' ? '新增题目' : '修改题目' }}
              </h2>
              <p v-if="mode === 'edit'" class="text-muted small mb-0 mt-1">题目 ID：{{ topicId }}</p>
            </div>
            <button type="button" class="btn-close" :disabled="submitting" aria-label="关闭" @click="requestClose"></button>
          </div>

          <form class="admin-modal-form" @submit.prevent="submitForm">
            <div class="modal-body admin-modal-body">
              <div v-if="loading" class="modal-state">
                <span class="spinner-border spinner-border-sm me-2" aria-hidden="true"></span>
                正在加载题目信息...
              </div>

              <div v-else class="row g-3">
                <div v-if="error" class="col-12">
                  <div class="alert alert-danger mb-0" role="alert">{{ error }}</div>
                </div>

                <div class="col-md-4">
                  <label class="form-label" for="topic-test-point">测试点数量</label>
                  <input id="topic-test-point" v-model="form.test_point" class="form-control" type="number" min="1" required>
                </div>
                <div class="col-md-4">
                  <label class="form-label" for="topic-star">难度</label>
                  <input id="topic-star" v-model="form.star" class="form-control" type="number" min="1" max="5">
                </div>
                <div class="col-md-4">
                  <label class="form-label" for="topic-time-limit">时间限制</label>
                  <input id="topic-time-limit" v-model="form.time_limit" class="form-control" type="number" min="1" required>
                </div>
                <div class="col-md-4">
                  <label class="form-label" for="topic-mem-limit">内存限制</label>
                  <input id="topic-mem-limit" v-model="form.mem_limit" class="form-control" type="number" min="1" required>
                </div>
                <div class="col-md-8">
                  <label class="form-label" for="topic-title">标题</label>
                  <input id="topic-title" v-model="form.title" class="form-control" maxlength="100" required>
                </div>
                <div class="col-12">
                  <label class="form-label" for="topic-description">题目描述</label>
                  <textarea id="topic-description" v-model="form.description" class="form-control" rows="5" required></textarea>
                </div>
                <div class="col-md-6">
                  <label class="form-label" for="topic-input-format">输入格式</label>
                  <textarea id="topic-input-format" v-model="form.input_format" class="form-control" rows="3"></textarea>
                </div>
                <div class="col-md-6">
                  <label class="form-label" for="topic-output-format">输出格式</label>
                  <textarea id="topic-output-format" v-model="form.output_format" class="form-control" rows="3"></textarea>
                </div>
                <div class="col-md-6">
                  <label class="form-label" for="topic-sample-input">样例输入</label>
                  <textarea id="topic-sample-input" v-model="form.sample_input" class="form-control font-monospace" rows="4"></textarea>
                </div>
                <div class="col-md-6">
                  <label class="form-label" for="topic-sample-output">样例输出</label>
                  <textarea id="topic-sample-output" v-model="form.sample_output" class="form-control font-monospace" rows="4"></textarea>
                </div>
                <div class="col-12">
                  <label class="form-label" for="topic-hint">提示</label>
                  <textarea id="topic-hint" v-model="form.hint" class="form-control" rows="3"></textarea>
                </div>
              </div>
            </div>

            <div class="modal-footer">
              <button type="button" class="btn btn-outline-secondary" :disabled="submitting" @click="requestClose">取消</button>
              <button type="submit" class="btn btn-primary" :disabled="submitting || loading">
                <span v-if="submitting" class="spinner-border spinner-border-sm me-2" aria-hidden="true"></span>
                {{ mode === 'create' ? '创建题目' : '保存修改' }}
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
import { emptyTopicForm } from '@/utils/admin.mjs'

export default {
  name: 'TopicFormModal',
  props: {
    visible: { type: Boolean, default: false },
    mode: { type: String, default: 'create' },
    topicId: { type: [String, Number], default: '' },
    initialValue: { type: Object, default: () => emptyTopicForm() },
    loading: { type: Boolean, default: false },
    submitting: { type: Boolean, default: false },
    error: { type: String, default: '' }
  },
  emits: ['close', 'submit'],
  setup(props, { emit }) {
    const form = ref(emptyTopicForm())

    const resetForm = () => {
      form.value = { ...emptyTopicForm(), ...props.initialValue }
    }

    watch(
      () => [props.visible, props.initialValue],
      () => {
        if (props.visible) resetForm()
      },
      { immediate: true, deep: true }
    )

    const requestClose = () => {
      if (!props.submitting) emit('close')
    }

    const submitForm = () => {
      if (!props.loading && !props.submitting) {
        emit('submit', { ...form.value })
      }
    }

    return { form, requestClose, submitForm }
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
.modal-state { min-height: 18rem; display: grid; place-items: center; color: #64748b; }
textarea { resize: vertical; }
</style>
