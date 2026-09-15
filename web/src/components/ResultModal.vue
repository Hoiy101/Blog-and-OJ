<template>
  <div class="modal-overlay" v-if="visible" @click.self="closeModal">
    <div class="modal-container" :class="resultClass">
      <div class="modal-header">
        <div class="result-icon">
          <i v-if="resultData.state === 'accepted'" class="bi bi-check-circle"></i>
          <i v-else-if="resultData.state === 'wrong_answer'" class="bi bi-x-circle"></i>
          <i v-else-if="resultData.state === 'time_limit'" class="bi bi-clock"></i>
          <i v-else-if="resultData.state === 'runtime_error'" class="bi bi-exclamation-triangle"></i>
          <i v-else class="bi bi-info-circle"></i>
        </div>
      </div>
      
      <div class="modal-body">
        <h3 class="result-title">{{ resultTitle }}</h3>
        <p class="result-desc">{{ resultDesc }}</p>
        
        <div v-if="!isSubmissionError" class="result-score">
          <div class="score-circle">
            <span class="score-value">{{ resultData.score }}</span>
            <span class="score-label">分</span>
          </div>
        </div>
        
        <div v-if="!isSubmissionError" class="result-details">
          <div class="detail-item">
            <span class="detail-label">题目ID</span>
            <span class="detail-value">#{{ resultData.evaluation_id }}</span>
          </div>
          <div class="detail-item">
            <span class="detail-label">状态</span>
            <span class="detail-value">{{ stateText }}</span>
          </div>
        </div>
      </div>
      
      <div class="modal-footer">
        <button class="btn btn-primary" @click="closeModal">
          {{ isSubmissionError ? '返回修改' : (resultData.state === 'accepted' ? '继续挑战' : '修改代码') }}
        </button>
        <button v-if="resultData.state !== 'accepted' && !isSubmissionError" class="btn btn-outline-secondary" @click="viewSolution">
          查看题解
        </button>
      </div>
    </div>
  </div>
</template>

<script>
import { computed } from 'vue'

export default {
  name: 'ResultModal',
  
  props: {
    visible: {
      type: Boolean,
      default: false
    },
    resultData: {
      type: Object,
      default: () => ({
        user_id: null,
        evaluation_id: null,
        score: null,
        state: '',
        message: ''
      })
    }
  },
  
  emits: ['close', 'viewSolution'],
  
  setup(props, { emit }) {
    const closeModal = () => {
      emit('close')
    }
    
    const viewSolution = () => {
      emit('viewSolution', props.resultData.evaluation_id)
    }

    const isSubmissionError = computed(() => props.resultData.state === 'submission_error')
    
    const resultClass = computed(() => {
      const classes = {
        'result-success': props.resultData.state === 'accepted',
        'result-warning': props.resultData.state === 'wrong_answer',
        'result-danger': ['time_limit', 'runtime_error', 'submission_error'].includes(props.resultData.state),
        'result-info': !['accepted', 'wrong_answer', 'time_limit', 'runtime_error', 'submission_error'].includes(props.resultData.state)
      }
      return classes
    })
    
    const resultTitle = computed(() => {
      const titles = {
        accepted: '恭喜通过！',
        wrong_answer: '答案错误',
        time_limit: '超时',
        runtime_error: '运行错误',
        compile_error: '编译错误',
        submission_error: '提交失败'
      }
      return titles[props.resultData.state] || '判题完成'
    })
    
    const resultDesc = computed(() => {
      if (isSubmissionError.value) {
        return props.resultData.message || '提交失败，请稍后重试'
      }

      const descs = {
        accepted: '你的代码通过了所有测试用例，表现出色！',
        wrong_answer: '你的代码输出与预期不符，请检查逻辑',
        time_limit: '代码运行时间过长，尝试优化算法',
        runtime_error: '程序运行时发生错误，请检查代码',
        compile_error: '代码编译失败，请检查语法错误'
      }
      return descs[props.resultData.state] || '判题已完成'
    })
    
    const stateText = computed(() => {
      const texts = {
        accepted: '通过',
        wrong_answer: '答案错误',
        time_limit: '超时',
        runtime_error: '运行错误',
        compile_error: '编译错误'
      }
      return texts[props.resultData.state] || props.resultData.state
    })
    
    return {
      closeModal,
      viewSolution,
      isSubmissionError,
      resultClass,
      resultTitle,
      resultDesc,
      stateText
    }
  }
}
</script>

<style scoped>
.modal-overlay {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: rgba(15, 23, 42, 0.55);
  backdrop-filter: blur(4px);
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 1rem;
  z-index: 1600;
  animation: fadeIn 0.2s ease;
}

@keyframes fadeIn {
  from {
    opacity: 0;
  }
  to {
    opacity: 1;
  }
}

.modal-container {
  position: relative;
  background: white;
  border-radius: 24px;
  padding: 2.25rem 2rem 1.75rem;
  width: 100%;
  max-width: 440px;
  text-align: center;
  animation: slideUp 0.3s ease;
  box-shadow: var(--app-shadow-lg);
  overflow: hidden;
  --tone-color: #0891b2;
  --tone-soft: rgba(8, 145, 178, 0.12);
}

.modal-container::before {
  content: "";
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  height: 6px;
  background: var(--tone-color);
}

@keyframes slideUp {
  from {
    opacity: 0;
    transform: translateY(20px) scale(0.96);
  }
  to {
    opacity: 1;
    transform: translateY(0) scale(1);
  }
}

.modal-container.result-success {
  --tone-color: #16a34a;
  --tone-soft: rgba(22, 163, 74, 0.12);
}

.modal-container.result-warning {
  --tone-color: #d97706;
  --tone-soft: rgba(217, 119, 6, 0.14);
}

.modal-container.result-danger {
  --tone-color: #dc2626;
  --tone-soft: rgba(220, 38, 38, 0.1);
}

.modal-container.result-info {
  --tone-color: #0891b2;
  --tone-soft: rgba(8, 145, 178, 0.12);
}

.modal-header {
  margin-bottom: 1rem;
}

.result-icon {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  width: 88px;
  height: 88px;
  border-radius: 50%;
  background: var(--tone-soft);
  color: var(--tone-color);
  font-size: 2.8rem;
  line-height: 1;
  animation: result-pop 0.4s cubic-bezier(0.34, 1.56, 0.64, 1);
}

.result-icon i {
  color: var(--tone-color);
}

@keyframes result-pop {
  from {
    transform: scale(0.6);
    opacity: 0;
  }
  to {
    transform: scale(1);
    opacity: 1;
  }
}

.modal-body {
  margin-bottom: 1.5rem;
}

.result-title {
  font-size: 1.6rem;
  font-weight: 800;
  margin-bottom: 0.4rem;
  color: var(--tone-color);
}

.result-desc {
  color: var(--app-text-secondary);
  font-size: 0.95rem;
  line-height: 1.6;
  margin-bottom: 1.5rem;
}

.result-score {
  margin-bottom: 1.5rem;
}

.score-circle {
  display: inline-flex;
  align-items: baseline;
  justify-content: center;
  width: 124px;
  height: 124px;
  border-radius: 50%;
  border: 8px solid var(--tone-color);
  background: #fff;
  color: var(--tone-color);
  padding-top: 34px;
}

.score-value {
  font-size: 2.4rem;
  font-weight: 800;
  line-height: 1;
  font-variant-numeric: tabular-nums;
}

.score-label {
  font-size: 0.95rem;
  font-weight: 600;
  margin-left: 0.2rem;
  color: var(--app-text-muted);
}

.result-details {
  background-color: var(--app-surface-soft);
  border-radius: 12px;
  padding: 0.5rem 1rem;
}

.detail-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0.6rem 0;
  border-bottom: 1px solid var(--app-border);
}

.detail-item:last-child {
  border-bottom: none;
}

.detail-label {
  color: var(--app-text-muted);
  font-size: 0.9rem;
}

.detail-value {
  font-weight: 600;
  color: var(--app-text);
}

.modal-footer {
  display: flex;
  flex-wrap: wrap;
  justify-content: center;
  gap: 0.75rem;
}

.modal-footer .btn {
  padding: 0.6rem 1.5rem;
  border-radius: 999px;
  font-weight: 600;
  transition: all 0.2s ease;
}

.modal-footer .btn-primary {
  background: var(--tone-color);
  border-color: var(--tone-color);
}

.modal-footer .btn-primary:hover {
  filter: brightness(0.92);
}

.modal-footer .btn:hover {
  transform: translateY(-1px);
}

@media (max-width: 576px) {
  .modal-container {
    padding: 2rem 1.25rem 1.5rem;
  }

  .result-icon {
    width: 72px;
    height: 72px;
    font-size: 2.2rem;
  }

  .result-title {
    font-size: 1.35rem;
  }

  .score-circle {
    width: 104px;
    height: 104px;
    padding-top: 26px;
  }

  .score-value {
    font-size: 2rem;
  }
}
</style>
