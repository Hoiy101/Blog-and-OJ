<template>
  <template v-if="visible">
    <div class="modal d-block" tabindex="-1" role="alertdialog" aria-modal="true" aria-labelledby="delete-topic-title">
      <div class="modal-dialog modal-dialog-centered">
        <div class="modal-content">
          <div class="modal-header">
            <h2 id="delete-topic-title" class="modal-title fs-5">确认删除题目</h2>
            <button type="button" class="btn-close" :disabled="submitting" aria-label="关闭" @click="requestClose"></button>
          </div>
          <div class="modal-body">
            <div v-if="error" class="alert alert-danger" role="alert">{{ error }}</div>
            <p>此操作将删除以下题目，且无法通过本界面撤销：</p>
            <div class="topic-summary">
              <span class="badge bg-light text-dark">ID: {{ topic.id }}</span>
              <strong>{{ topic.title || '无标题' }}</strong>
            </div>
          </div>
          <div class="modal-footer">
            <button type="button" class="btn btn-outline-secondary" :disabled="submitting" @click="requestClose">取消</button>
            <button type="button" class="btn btn-danger" :disabled="submitting" @click="confirmDelete">
              <span v-if="submitting" class="spinner-border spinner-border-sm me-2" aria-hidden="true"></span>
              确认删除
            </button>
          </div>
        </div>
      </div>
    </div>
    <div class="modal-backdrop show"></div>
  </template>
</template>

<script>
export default {
  name: 'DeleteTopicModal',
  props: {
    visible: { type: Boolean, default: false },
    topic: { type: Object, default: () => ({ id: '', title: '' }) },
    submitting: { type: Boolean, default: false },
    error: { type: String, default: '' }
  },
  emits: ['close', 'confirm'],
  setup(props, { emit }) {
    const requestClose = () => {
      if (!props.submitting) emit('close')
    }
    const confirmDelete = () => {
      if (!props.submitting) emit('confirm')
    }
    return { requestClose, confirmDelete }
  }
}
</script>

<style scoped>
.modal { z-index: 1060; }
.modal-backdrop { z-index: 1055; }
.topic-summary { display: flex; gap: .75rem; align-items: center; padding: 1rem; border-radius: 10px; background: #f8fafc; }
</style>
