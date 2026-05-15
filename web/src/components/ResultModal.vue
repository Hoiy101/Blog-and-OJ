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
        
        <div class="result-score">
          <div class="score-circle">
            <span class="score-value">{{ resultData.score }}</span>
            <span class="score-label">分</span>
          </div>
        </div>
        
        <div class="result-details">
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
          {{ resultData.state === 'accepted' ? '继续挑战' : '修改代码' }}
        </button>
        <button v-if="resultData.state !== 'accepted'" class="btn btn-outline-secondary" @click="viewSolution">
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
        score: 0,
        state: ''
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
    
    const resultClass = computed(() => {
      const classes = {
        'result-success': props.resultData.state === 'accepted',
        'result-warning': props.resultData.state === 'wrong_answer',
        'result-danger': props.resultData.state === 'time_limit' || props.resultData.state === 'runtime_error',
        'result-info': !['accepted', 'wrong_answer', 'time_limit', 'runtime_error'].includes(props.resultData.state)
      }
      return classes
    })
    
    const resultTitle = computed(() => {
      const titles = {
        accepted: '恭喜通过！',
        wrong_answer: '答案错误',
        time_limit: '超时',
        runtime_error: '运行错误',
        compile_error: '编译错误'
      }
      return titles[props.resultData.state] || '判题完成'
    })
    
    const resultDesc = computed(() => {
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
  background-color: rgba(0, 0, 0, 0.5);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 1000;
  animation: fadeIn 0.3s ease;
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
  background: white;
  border-radius: 20px;
  padding: 2rem;
  min-width: 400px;
  max-width: 90%;
  text-align: center;
  animation: slideUp 0.4s ease;
  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
}

@keyframes slideUp {
  from {
    opacity: 0;
    transform: translateY(30px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.modal-container.result-success {
  border-top: 6px solid #28a745;
}

.modal-container.result-warning {
  border-top: 6px solid #ffc107;
}

.modal-container.result-danger {
  border-top: 6px solid #dc3545;
}

.modal-container.result-info {
  border-top: 6px solid #17a2b8;
}

.modal-header {
  margin-bottom: 1.5rem;
}

.result-icon {
  font-size: 6rem;
}

.result-success .result-icon i {
  color: #28a745;
}

.result-warning .result-icon i {
  color: #ffc107;
}

.result-danger .result-icon i {
  color: #dc3545;
}

.result-info .result-icon i {
  color: #17a2b8;
}

.modal-body {
  margin-bottom: 1.5rem;
}

.result-title {
  font-size: 1.8rem;
  font-weight: 700;
  margin-bottom: 0.5rem;
}

.result-success .result-title {
  color: #28a745;
}

.result-warning .result-title {
  color: #ffc107;
}

.result-danger .result-title {
  color: #dc3545;
}

.result-info .result-title {
  color: #17a2b8;
}

.result-desc {
  color: #6c757d;
  font-size: 1rem;
  margin-bottom: 1.5rem;
}

.result-score {
  margin-bottom: 1.5rem;
}

.score-circle {
  display: inline-flex;
  align-items: baseline;
  justify-content: center;
  width: 120px;
  height: 120px;
  border-radius: 50%;
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
  padding: 1rem;
}

.result-success .score-circle {
  background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
}

.result-warning .score-circle {
  background: linear-gradient(135deg, #fc4a1a 0%, #f7b733 100%);
}

.result-danger .score-circle {
  background: linear-gradient(135deg, #ef473a 0%, #cb2d3e 100%);
}

.score-value {
  font-size: 3rem;
  font-weight: 700;
}

.score-label {
  font-size: 1rem;
  margin-left: 0.25rem;
}

.result-details {
  background-color: #f8f9fa;
  border-radius: 10px;
  padding: 1rem;
}

.detail-item {
  display: flex;
  justify-content: space-between;
  padding: 0.5rem 0;
  border-bottom: 1px solid #e9ecef;
}

.detail-item:last-child {
  border-bottom: none;
}

.detail-label {
  color: #6c757d;
  font-weight: 500;
}

.detail-value {
  font-weight: 600;
  color: #2c3e50;
}

.modal-footer {
  display: flex;
  justify-content: center;
  gap: 1rem;
}

.modal-footer .btn {
  padding: 0.75rem 2rem;
  border-radius: 25px;
  font-weight: 600;
  transition: all 0.3s ease;
}

.modal-footer .btn-primary {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  border: none;
}

.result-success .modal-footer .btn-primary {
  background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
}

.modal-footer .btn:hover {
  transform: translateY(-2px);
  box-shadow: 0 5px 20px rgba(0, 0, 0, 0.2);
}

@media (max-width: 576px) {
  .modal-container {
    min-width: auto;
    padding: 1.5rem;
  }
  
  .result-icon {
    font-size: 4rem;
  }
  
  .result-title {
    font-size: 1.4rem;
  }
  
  .score-circle {
    width: 100px;
    height: 100px;
  }
  
  .score-value {
    font-size: 2.5rem;
  }
}
</style>