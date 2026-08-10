<template>
    <div class="container content-field">
        <div class="card oj-card">
            <!-- 卡片头部 -->
            <div class="card-header border-bottom bg-light">
                <div class="row align-items-center">
                    <div class="col-md-6">
                        <h5 class="mb-0">题库列表</h5>
                        <p class="text-muted mb-0 small">共 {{ total }} 道题目</p>
                    </div>
                    <div class="col-md-6">
                        <div class="input-group">
                            <input 
                                type="text" 
                                class="form-control" 
                                placeholder="搜索题目标题或题号..." 
                                aria-label="搜索题目"
                                v-model="searchKeyword"
                                @keyup.enter="handleSearch"
                            >
                            <button class="btn btn-outline-primary" type="button" @click="handleSearch">
                                搜索
                            </button>
                        </div>
                    </div>
                </div>
            </div>
            <!-- 题目列表 -->
            <div class="card-body p-0 problem-list-container">
                <!-- 加载状态 -->
                <div v-if="loading" class="text-center py-5">
                    <div class="loading-spinner"></div>
                    <p class="mt-3 text-muted">加载中...</p>
                </div>

                <!-- 错误状态 -->
                <div v-else-if="error" class="text-center py-5">
                    <div class="error-state text-danger">
                        <i class="bi bi-exclamation-triangle display-4"></i>
                        <p class="mt-3">加载失败: {{ error }}</p>
                        <button class="btn btn-primary mt-2" @click="retryProblemList">重试</button>
                    </div>
                </div>

                <!-- 无数据提示 -->
                <div v-else-if="problems.length === 0" class="text-center py-5">
                    <div class="empty-state">
                        <i class="bi bi-clipboard-data display-4 text-muted"></i>
                        <p class="mt-3 text-muted">暂无题目</p>
                        <button class="btn btn-outline-primary mt-2" @click="getProblemList(1)">刷新</button>
                    </div>
                </div>

                <!-- 题目表格 -->
                <div v-else class="problem-table-container">
                    <table class="table table-hover table-borderless mb-0">
                        <thead>
                            <tr class="table-light">
                                <th scope="col" style="width: 80px;">
                                    <span class="d-flex align-items-center">
                                        <i class="bi bi-hash me-1"></i> 题号
                                    </span>
                                </th>
                                <th scope="col">
                                    <span class="d-flex align-items-center">
                                        <i class="bi bi-journal-text me-1"></i> 标题
                                    </span>
                                </th>
                                <th scope="col" style="width: 120px;">
                                    <span class="d-flex align-items-center">
                                        <i class="bi bi-star me-1"></i> 难度
                                    </span>
                                </th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr
                                v-for="problem in problems"
                                :key="problem.id"
                                class="problem-row"
                                role="link"
                                tabindex="0"
                                @click="handleView(problem.id)"
                                @keydown.enter="handleView(problem.id)"
                                @keydown.space.prevent="handleView(problem.id)"
                            >
                                <td class="text-center">
                                    <span class="badge bg-light text-dark fw-normal problem-id">#{{ problem.id }}</span>
                                </td>
                                <td>
                                    <div class="problem-title-wrapper">
                                        <h6 class="mb-1">{{ problem.title || '无标题' }}</h6>
                                        <p class="text-muted mb-0 small text-truncate">
                                            {{ problem.description || '暂无描述' }}
                                        </p>
                                    </div>
                                </td>
                                <td>
                                    <span class="badge difficulty-badge" :class="getDifficultyClass(problem.star)">
                                        {{ getDifficultyText(problem.star) }}
                                    </span>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>

            <!-- 卡片底部 -->
            <div class="card-footer pagination-footer">
                <span class="text-muted">共 {{ total }} 道题目</span>
                <div class="pagination-controls" aria-label="题库分页">
                    <button
                        type="button"
                        class="page-arrow"
                        aria-label="上一页"
                        :disabled="loading || currentPage <= 1"
                        @click="changePage(-1)"
                    >←</button>
                    <input
                        v-model.number="pageInput"
                        type="number"
                        min="1"
                        :max="Math.max(totalPages, 1)"
                        aria-label="跳转页码"
                        :disabled="loading || totalPages === 0"
                        @change="goToPage(pageInput)"
                        @keyup.enter="$event.target.blur()"
                    >
                    <button
                        type="button"
                        class="page-arrow"
                        aria-label="下一页"
                        :disabled="loading || totalPages === 0 || currentPage >= totalPages"
                        @click="changePage(1)"
                    >→</button>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import { ref, onMounted } from 'vue'
import $ from 'jquery'
import { useStore } from 'vuex'
import { useRouter } from 'vue-router'
import { clampPage, normalizePageResponse, paginationQuery } from '../../utils/pagination.mjs'

export default {
    name: 'OJProblemList',
    
    setup() {
        const store = useStore()
        const problems = ref([])
        const loading = ref(false)
        const searchKeyword = ref('')
        const error = ref(null)
        const currentPage = ref(1)
        const pageInput = ref(1)
        const total = ref(0)
        const totalPages = ref(0)
        const activeKeyword = ref('')
        const retryPage = ref(1)
        const retryKeyword = ref('')
        const topicListRequestId = ref(0)
        const router = useRouter()

        const handleView = (id) => {
            router.push({ name: 'Details', params: { id: id } })
        }
        // 获取难度对应的CSS类
        const getDifficultyClass = (star) => {
            if (!star) return 'difficulty-unknown'
            
            const starNum = parseInt(star)
            if (isNaN(starNum)) return 'difficulty-unknown'
            
            if (starNum <= 2) return 'difficulty-easy'
            if (starNum <= 4) return 'difficulty-medium'
            return 'difficulty-hard'
        }

        // 获取难度文本
        const getDifficultyText = (star) => {
            if (!star) return '未知'
            
            const starNum = parseInt(star)
            if (isNaN(starNum)) return '未知'
            
            if (starNum === 1) return '简单'
            if (starNum === 2) return '较易'
            if (starNum === 3) return '中等'
            if (starNum === 4) return '较难'
            if (starNum === 5) return '困难'
            return `${starNum}星`
        }

        // 获取题目列表
        const resetProblemPage = () => {
            problems.value = []
            currentPage.value = 1
            pageInput.value = 1
            total.value = 0
            totalPages.value = 0
        }

        const applyProblemPage = (resp) => {
            const page = normalizePageResponse(resp, 20)
            problems.value = page.records
            currentPage.value = page.currentPage
            pageInput.value = page.currentPage
            total.value = page.total
            totalPages.value = page.totalPages
        }

        const getProblemList = (
            requestedPage = currentPage.value,
            requestedKeyword = activeKeyword.value
        ) => {
            const requestData = paginationQuery(requestedPage, requestedKeyword)
            const requestId = ++topicListRequestId.value
            retryPage.value = requestData.page
            retryKeyword.value = requestData.keyword
            loading.value = true
            error.value = null
            
            console.log('开始获取题目列表...')
            const headers = {}
            if (store.state.user.token && store.state.user.token.trim().length > 0) {
                headers.Authorization = "Bearer " + store.state.user.token
            }
            
            $.ajax({
                url: "http://127.0.0.1:3000/oj/topic/getlist/",
                type: "GET",
                headers: headers,
                data: requestData,
                success(resp) {
                    if (requestId !== topicListRequestId.value) return
                    console.log('题目列表API响应:', resp)
                    try {
                        applyProblemPage(resp)
                        console.log('成功获取到', problems.value.length, '道当前页题目')
                    } catch (responseError) {
                        resetProblemPage()
                        error.value = responseError.message
                    }
                },
                error(jqXHR, textStatus, errorThrown) {
                    if (requestId !== topicListRequestId.value) return
                    console.error("获取题目列表失败:", jqXHR.status, textStatus, errorThrown)
                    
                    let errorMsg = '网络请求失败'
                    if (jqXHR.status === 0) {
                        errorMsg = '无法连接到服务器'
                    } else if (jqXHR.status === 401) {
                        errorMsg = '登录已过期'
                    } else if (jqXHR.status === 404) {
                        errorMsg = 'API接口不存在'
                    } else {
                        errorMsg = `错误: ${jqXHR.status}`
                    }
                    
                    error.value = errorMsg
                },
                complete() {
                    if (requestId !== topicListRequestId.value) return
                    loading.value = false
                }
            })
        }

        const handleSearch = () => {
            activeKeyword.value = searchKeyword.value.trim()
            getProblemList(1)
        }

        const changePage = (offset) => {
            getProblemList(currentPage.value + offset)
        }

        const goToPage = (value) => {
            getProblemList(clampPage(value, totalPages.value))
        }

        const retryProblemList = () => {
            getProblemList(retryPage.value, retryKeyword.value)
        }

        onMounted(() => {
            getProblemList()
        })

        return {
            problems,
            loading,
            searchKeyword,
            error,
            currentPage,
            pageInput,
            total,
            totalPages,
            getDifficultyClass,
            getDifficultyText,
            getProblemList,
            handleSearch,
            handleView,
            changePage,
            goToPage,
            retryProblemList
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

/* 卡片样式 */
.oj-card {
    border: 1px solid #e0e0e0;
    border-radius: 12px;
    overflow: hidden;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.05);
    transition: all 0.3s ease;
    background-color: #fff;
}

.oj-card:hover {
    box-shadow: 0 6px 20px rgba(0, 0, 0, 0.08);
}

/* 卡片头部样式 */
.oj-card .card-header {
    background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
    padding: 1.5rem 2rem;
    border-bottom: 2px solid #dee2e6;
}

.oj-card .card-header h5 {
    font-weight: 600;
    color: #2c3e50;
}

/* 搜索框样式 */
.oj-card .input-group {
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
}

.oj-card .form-control {
    border-color: #ced4da;
    border-right: none;
    padding: 0.75rem 1rem;
    font-size: 0.95rem;
    height: 42px;
}

.oj-card .form-control:focus {
    border-color: #86b7fe;
    box-shadow: 0 0 0 0.2rem rgba(13, 110, 253, 0.15);
    z-index: 1;
}

.oj-card .btn-outline-primary {
    border-color: #0d6efd;
    color: #0d6efd;
    padding: 0.5rem 1.5rem;
    height: 42px;
    font-weight: 500;
    transition: all 0.2s;
}

.oj-card .btn-outline-primary:hover {
    background-color: #0d6efd;
    color: white;
    transform: translateY(-1px);
}

/* 题目表格样式 */
.problem-table-container {
    overflow-x: auto;
}

.problem-table-container table {
    margin-bottom: 0;
}

.problem-table-container thead th {
    border-bottom: 2px solid #dee2e6;
    background-color: #f8f9fa;
    font-weight: 600;
    color: #495057;
    padding: 1rem 1.5rem;
    white-space: nowrap;
}

.problem-table-container tbody tr {
    transition: all 0.2s ease;
    border-bottom: 1px solid #f0f0f0;
}

.problem-row {
    cursor: pointer;
}

.problem-row:focus-visible {
    outline: 2px solid #0d6efd;
    outline-offset: -2px;
}

.problem-table-container tbody tr:hover {
    background-color: #f8fafc;
    transform: translateX(2px);
}

.problem-table-container tbody tr:last-child {
    border-bottom: none;
}

.problem-table-container tbody td {
    padding: 1.25rem 1.5rem;
    vertical-align: middle;
}

.input-group {
    margin: auto;
}
/* 题号样式 */
.problem-id {
    font-size: 0.9rem;
    padding: 0.4em 0.8em;
    border: 1px solid #dee2e6;
    border-radius: 20px;
    font-weight: 500;
}

/* 题目标题样式 */
.problem-title-wrapper h6 {
    font-weight: 600;
    color: #2c3e50;
    margin-bottom: 0.5rem;
}

.problem-title-wrapper p {
    font-size: 0.85rem;
    line-height: 1.4;
    max-width: 600px;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

/* 难度标签样式 */
.difficulty-badge {
    font-size: 0.85rem;
    padding: 0.4em 0.8em;
    border-radius: 20px;
    font-weight: 500;
    display: inline-block;
    min-width: 60px;
    text-align: center;
}

.difficulty-easy {
    background-color: rgba(40, 167, 69, 0.1);
    color: #28a745;
    border: 1px solid rgba(40, 167, 69, 0.3);
}

.difficulty-medium {
    background-color: rgba(255, 193, 7, 0.1);
    color: #ffc107;
    border: 1px solid rgba(255, 193, 7, 0.3);
}

.difficulty-hard {
    background-color: rgba(220, 53, 69, 0.1);
    color: #dc3545;
    border: 1px solid rgba(220, 53, 69, 0.3);
}

.difficulty-unknown {
    background-color: rgba(108, 117, 125, 0.1);
    color: #6c757d;
    border: 1px solid rgba(108, 117, 125, 0.3);
}

/* 空状态样式 */
.empty-state, .error-state {
    padding: 4rem 2rem;
    color: #6c757d;
}

.empty-state i, .error-state i {
    opacity: 0.5;
    font-size: 4rem;
}

.error-state i {
    color: #dc3545;
}

/* 卡片底部样式 */
.oj-card .card-footer {
    background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
    padding: 1rem 2rem;
    border-top: 1px solid #dee2e6;
}

.pagination-footer {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: 1rem;
}

.pagination-controls {
    display: flex;
    justify-content: flex-end;
    align-items: center;
    gap: 0.5rem;
}

.pagination-controls input {
    width: 72px;
    height: 36px;
    text-align: center;
    border: 1px solid #ced4da;
    border-radius: 6px;
}

.page-arrow {
    width: 36px;
    height: 36px;
    border: 1px solid #0d6efd;
    border-radius: 6px;
    background: #fff;
    color: #0d6efd;
}

.page-arrow:disabled {
    border-color: #adb5bd;
    color: #adb5bd;
    cursor: not-allowed;
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
    
    .oj-card .card-header {
        padding: 1rem;
    }
    
    .oj-card .card-header .row > div {
        width: 100%;
        margin-bottom: 1rem;
    }
    
    .problem-table-container tbody td {
        padding: 0.75rem 0.5rem;
    }
    
    .problem-table-container thead th {
        padding: 0.75rem 0.5rem;
    }
    
    .problem-title-wrapper p {
        max-width: 200px;
    }
    
    .difficulty-badge {
        min-width: 50px;
        font-size: 0.8rem;
    }

    .pagination-footer {
        flex-wrap: wrap;
    }

    .pagination-controls {
        width: 100%;
    }
}
</style>
