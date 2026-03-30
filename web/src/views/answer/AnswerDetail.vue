<template>
    <div class="container content-field">
        <!-- 题解详情页面 -->
        <div class="card blog-detail-card">
            <!-- 详情页头部 -->
            <div class="card-header border-bottom bg-light d-flex justify-content-between align-items-center">
                <div>
                    <button class="btn btn-outline-secondary btn-sm" @click="backToProblem">
                        <i class="bi bi-arrow-left"></i> 返回题目
                    </button>
                </div>
                <h5 class="mb-0">题解详情</h5>
                <div>
                    <span class="badge bg-info">题目 ID: {{ answer.problemId }}</span>
                </div>
            </div>

            <!-- 题解内容区域 -->
            <div class="card-body blog-detail-container">
                <!-- 加载状态 -->
                <div v-if="loading" class="text-center py-5">
                    <div class="loading-spinner"></div>
                    <p class="mt-3 text-muted">加载题解内容中...</p>
                </div>

                <!-- 错误状态 -->
                <div v-else-if="error" class="text-center py-5">
                    <div class="error-state text-danger">
                        <i class="bi bi-exclamation-triangle display-4"></i>
                        <p class="mt-3">加载失败: {{ error }}</p>
                        <button class="btn btn-primary mt-2" @click="backToProblem">返回题目</button>
                    </div>
                </div>

                <!-- 题解详情内容 -->
                <div v-else class="blog-detail-content">
                    <!-- 题解标题 -->
                    <div class="blog-header mb-4">
                        <h1 class="blog-title">{{ answer.title || '无标题' }}</h1>
                    </div>

                    <!-- 题解内容 -->
                    <div class="blog-body">
                        <div class="blog-content" v-html="formatContent(answer.content)"></div>
                    </div>

                    <!-- 操作按钮 -->
                    <div class="blog-actions mt-4 pt-4 border-top">
                        <button class="btn btn-outline-secondary" @click="backToProblem">
                            <i class="bi bi-arrow-left"></i> 返回题目
                        </button>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import { ref, onMounted } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import $ from 'jquery'
import { useStore } from 'vuex'

export default {
    name: 'AnswerDetail',
    
    setup() {
        const router = useRouter()
        const route = useRoute()
        const store = useStore()
        
        // 题解相关状态
        const answer = ref({
            problemId: null,
            title: '',
            content: ''
        })
        const loading = ref(false)
        const error = ref(null)



        // 格式化题解内容
        const formatContent = (content) => {
            if (!content) return '<p class="text-muted fst-italic">暂无内容</p>'
            
            // 将换行符转换为HTML换行
            let formattedContent = content.replace(/\n/g, '<br>')
            
            // 将连续的多个换行转换为段落
            formattedContent = formattedContent.replace(/(<br>\s*){2,}/g, '</p><p>')
            
            // 确保内容被包裹在段落中
            if (!formattedContent.startsWith('<p>')) {
                formattedContent = '<p>' + formattedContent
            }
            if (!formattedContent.endsWith('</p>')) {
                formattedContent = formattedContent + '</p>'
            }
            
            return formattedContent
        }

        // 获取题解详情
        const getAnswerDetail = () => {
            const problemId = route.params.id
            
            if (!problemId) {
                error.value = '题目ID不存在'
                return
            }

            loading.value = true
            error.value = null
            
            console.log('获取题解详情，题目ID:', problemId)
            
            // 发送请求获取题解详情
            $.ajax({
                url: "http://127.0.0.1:3000/oj/answer/get/",
                type: "GET",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                data: {
                    topic_id: problemId,
                },
                success(resp) {
                    console.log('题解详情API响应:', resp)
                    loading.value = false
                    
                    if (resp && resp.error_message === 'success') {
                        answer.value = {
                            problemId: problemId,
                            title: resp.title || '无标题',
                            content: resp.content || ''
                        }
                        console.log('题解详情加载成功:', answer.value.title)
                    } else {
                        error.value = resp.error_message || '获取的题解数据格式不正确'
                        console.error('题解数据格式错误:', resp)
                    }
                },
                error(jqXHR, textStatus, errorThrown) {
                    console.error('获取题解详情失败:', jqXHR.status, textStatus, errorThrown)
                    loading.value = false
                    
                    let errorMsg = '网络请求失败'
                    if (jqXHR.status === 0) {
                        errorMsg = '无法连接到服务器，请检查网络连接'
                    } else if (jqXHR.status === 401) {
                        errorMsg = '登录已过期，请重新登录'
                    } else if (jqXHR.status === 404) {
                        errorMsg = '题解不存在'
                    } else if (jqXHR.status === 500) {
                        errorMsg = '服务器内部错误，请稍后重试'
                    } else {
                        errorMsg = `请求失败: ${jqXHR.status} ${errorThrown}`
                    }
                    
                    error.value = errorMsg
                }
            })
        }

        // 返回题目详情页
        const backToProblem = () => {
            const problemId = answer.value.problemId || route.params.id
            store.commit('topic/updatetopicid', problemId)
            router.push('/oj/details/')
        }

        onMounted(() => {
            getAnswerDetail()
        })

        return {
            answer,
            loading,
            error,
            formatContent,
            backToProblem
        }
    }
}
</script>

<style scoped>
/* 主容器间距 */
.content-field {
    margin-top: 2rem;
    padding-top: 1rem;
    padding-bottom: 3rem;
    min-height: calc(100vh - 200px);
}

/* 题解卡片样式 */
.blog-detail-card {
    border: 1px solid #e0e0e0;
    border-radius: 12px;
    overflow: hidden;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.05);
    transition: all 0.3s ease;
    background-color: #fff;
}

.blog-detail-card:hover {
    box-shadow: 0 6px 20px rgba(0, 0, 0, 0.08);
}

/* 卡片头部样式 */
.blog-detail-card .card-header {
    background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
    padding: 1.5rem 2rem;
    border-bottom: 2px solid #dee2e6;
}

.blog-detail-card .card-header h5 {
    font-weight: 600;
    color: #2c3e50;
}

/* 题解详情容器 */
.blog-detail-container {
    padding: 2rem;
}

/* 题解头部 */
.blog-header {
    border-bottom: 2px solid #f0f0f0;
    padding-bottom: 1.5rem;
    margin-bottom: 2rem;
}

/* 题解标题 */
.blog-title {
    font-size: 2.5rem;
    font-weight: 700;
    color: #2c3e50;
    margin-bottom: 1rem;
    line-height: 1.2;
}

/* 题解元信息 */
.blog-meta {
    font-size: 0.9rem;
    color: #6c757d;
    margin-bottom: 1.5rem;
}

.blog-meta i {
    margin-right: 0.5rem;
}

/* 题解描述 */
.blog-description {
    background-color: #f8f9fa;
    border-left: 4px solid #0d6efd;
    padding: 1.5rem;
    font-size: 1.1rem;
    line-height: 1.6;
    color: #495057;
}

.blog-description .lead {
    font-size: 1.2rem;
    font-weight: 400;
    color: #495057;
}

/* 题解内容 */
.blog-content {
    font-size: 1.1rem;
    line-height: 1.8;
    color: #333;
}

.blog-content h1, .blog-content h2, .blog-content h3 {
    margin-top: 2rem;
    margin-bottom: 1rem;
    color: #2c3e50;
    font-weight: 600;
}

.blog-content h1 {
    font-size: 2rem;
    border-bottom: 2px solid #f0f0f0;
    padding-bottom: 0.5rem;
}

.blog-content h2 {
    font-size: 1.75rem;
    border-left: 4px solid #0d6efd;
    padding-left: 1rem;
}

.blog-content h3 {
    font-size: 1.5rem;
    color: #495057;
}

.blog-content p {
    margin-bottom: 1.5rem;
    text-align: justify;
}

.blog-content ul, .blog-content ol {
    margin-bottom: 1.5rem;
    padding-left: 2rem;
}

.blog-content li {
    margin-bottom: 0.5rem;
}

.blog-content pre {
    background-color: #f8f9fa;
    border: 1px solid #e9ecef;
    border-radius: 0.5rem;
    padding: 1rem;
    margin: 1.5rem 0;
    overflow-x: auto;
    font-family: 'Courier New', Courier, monospace;
    font-size: 0.9rem;
    line-height: 1.4;
}

.blog-content code {
    background-color: #f8f9fa;
    padding: 0.2rem 0.4rem;
    border-radius: 0.25rem;
    font-family: 'Courier New', Courier, monospace;
    font-size: 0.9em;
    color: #e83e8c;
}

/* 操作按钮 */
.blog-actions {
    display: flex;
    justify-content: flex-start;
    gap: 1rem;
}

/* 错误状态样式 */
.error-state {
    padding: 4rem 2rem;
    color: #6c757d;
}

.error-state i {
    opacity: 0.5;
    font-size: 4rem;
    color: #dc3545;
}

/* 加载动画 */
.loading-spinner {
    width: 40px;
    height: 40px;
    border: 3px solid #f3f3f3;
    border-top: 3px solid #0d6efd;
    border-radius: 50%;
    animation: spin 1s linear infinite;
    margin: 2rem auto;
}

@keyframes spin {
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
}

/* 响应式调整 */
@media (max-width: 768px) {
    .content-field {
        margin-top: 1rem;
        padding: 0.5rem;
    }
    
    .blog-detail-card .card-header {
        padding: 1rem;
    }
    
    .blog-detail-container {
        padding: 1.5rem;
    }
    
    .blog-title {
        font-size: 1.8rem;
    }
    
    .blog-description {
        padding: 1rem;
        font-size: 1rem;
    }
    
    .blog-content {
        font-size: 1rem;
    }
    
    .blog-content h1 {
        font-size: 1.6rem;
    }
    
    .blog-content h2 {
        font-size: 1.4rem;
    }
    
    .blog-content h3 {
        font-size: 1.2rem;
    }
}
</style>