<template>
    <div class="container page-shell">
        <!-- 题解详情页面 -->
        <article class="page-card answer-card">
            <!-- 详情页头部 -->
            <div class="page-card-header">
                <button class="btn btn-outline-secondary btn-sm" @click="backToProblem">
                    <i class="bi bi-arrow-left"></i> 返回题目
                </button>
                <div class="d-flex align-items-center gap-2">
                    <span class="tag"><i class="bi bi-lightbulb"></i> 题解</span>
                    <span class="tag tag-mono">题目 #{{ answer.problemId }}</span>
                </div>
            </div>

            <!-- 题解内容区域 -->
            <div class="answer-container">
                <!-- 加载状态 -->
                <div v-if="loading" class="state-panel">
                    <div class="loading-spinner"></div>
                    <p class="state-text">加载题解内容中…</p>
                </div>

                <!-- 错误状态 -->
                <div v-else-if="error" class="state-panel is-error">
                    <span class="state-icon"><i class="bi bi-exclamation-triangle"></i></span>
                    <p class="state-title">加载失败</p>
                    <p class="state-text">{{ error }}</p>
                    <button class="btn btn-primary" @click="backToProblem">返回题目</button>
                </div>

                <!-- 题解详情内容 -->
                <div v-else class="answer-content">
                    <header class="answer-header">
                        <h1 class="answer-title">{{ answer.title || '无标题' }}</h1>
                        <p class="answer-tip">
                            <i class="bi bi-info-circle"></i>
                            建议先独立思考并提交后再阅读题解，效果更好。
                        </p>
                    </header>

                    <!-- 题解内容 -->
                    <div class="article-body" v-html="formatContent(answer.content)"></div>

                    <!-- 操作按钮 -->
                    <footer class="answer-actions">
                        <button class="btn btn-primary" @click="backToProblem">
                            <i class="bi bi-code-slash"></i> 回去继续做题
                        </button>
                    </footer>
                </div>
            </div>
        </article>
    </div>
</template>

<script>
import { ref, onMounted, watch } from 'vue'
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
                loading.value = false
                return
            }

            loading.value = true
            error.value = null
            
            console.log('获取题解详情，题目ID:', problemId)
            
            // 发送请求获取题解详情
            const headers = {}
            if (store.state.user.token && store.state.user.token.trim().length > 0) {
                headers.Authorization = "Bearer " + store.state.user.token
            }
            
            $.ajax({
                url: "http://127.0.0.1:3000/oj/answer/get/",
                type: "GET",
                headers: headers,
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
            router.push('/oj/details/' + problemId)
        }

        // 监听路由参数变化
        watch(() => route.params.id, (newId) => {
            if (newId) {
                getAnswerDetail()
            }
        }, { immediate: true })

        onMounted(() => {
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
.answer-container {
    padding: 2.5rem 3rem 2rem;
}

.answer-header {
    padding-bottom: 1.5rem;
    margin-bottom: 2rem;
    border-bottom: 1px solid var(--app-border);
}

.answer-title {
    margin: 0 0 1rem;
    font-size: 2rem;
    font-weight: 800;
    line-height: 1.3;
    color: var(--app-text);
}

.answer-tip {
    display: inline-flex;
    align-items: center;
    gap: 0.45rem;
    margin: 0;
    padding: 0.5rem 0.9rem;
    border-radius: 999px;
    background: rgba(217, 119, 6, 0.1);
    color: #b45309;
    font-size: 0.88rem;
}

.answer-actions {
    display: flex;
    flex-wrap: wrap;
    gap: 0.75rem;
    margin-top: 2.5rem;
    padding-top: 1.5rem;
    border-top: 1px solid var(--app-border);
}

@media (max-width: 767.98px) {
    .answer-container {
        padding: 1.5rem 1.25rem;
    }

    .answer-title {
        font-size: 1.5rem;
    }
}
</style>
