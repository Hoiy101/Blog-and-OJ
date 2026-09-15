<template>
    <div class="container page-shell">
        <div class="page-card oj-card">
            <!-- 卡片头部 -->
            <div class="page-card-header">
                <div class="d-flex align-items-center gap-3">
                    <span class="page-title-icon"><i class="bi bi-code-square"></i></span>
                    <div>
                        <h1>题库</h1>
                        <p class="page-card-subtitle">共 {{ total }} 道题目</p>
                    </div>
                </div>
                <div class="search-box">
                    <i class="bi bi-search"></i>
                    <input
                        type="text"
                        class="form-control"
                        placeholder="搜索题目标题或题号..."
                        aria-label="搜索题目"
                        v-model="searchKeyword"
                        @keyup.enter="handleSearch"
                    >
                    <button class="btn btn-primary search-btn" type="button" @click="handleSearch">
                        搜索
                    </button>
                </div>
            </div>
            <!-- 题目列表 -->
            <div class="problem-list-container">
                <!-- 加载状态 -->
                <div v-if="loading" class="state-panel">
                    <div class="loading-spinner"></div>
                    <p class="state-text">加载题目中…</p>
                </div>

                <!-- 错误状态 -->
                <div v-else-if="error" class="state-panel is-error">
                    <span class="state-icon"><i class="bi bi-exclamation-triangle"></i></span>
                    <p class="state-title">加载失败</p>
                    <p class="state-text">{{ error }}</p>
                    <button class="btn btn-primary" @click="retryProblemList"><i class="bi bi-arrow-clockwise"></i> 重试</button>
                </div>

                <!-- 无数据提示 -->
                <div v-else-if="problems.length === 0" class="state-panel">
                    <span class="state-icon"><i class="bi bi-clipboard-x"></i></span>
                    <p class="state-title">暂无题目</p>
                    <p class="state-text">换个关键词试试，或者稍后再来看看。</p>
                    <button class="btn btn-outline-primary" @click="getProblemList(1)"><i class="bi bi-arrow-clockwise"></i> 刷新</button>
                </div>

                <!-- 题目表格 -->
                <div v-else class="table-responsive">
                    <table class="table table-hover app-table problem-table">
                        <thead>
                            <tr>
                                <th scope="col" class="col-id">题号</th>
                                <th scope="col">题目</th>
                                <th scope="col" class="col-difficulty">难度</th>
                                <th scope="col" class="col-action"><span class="visually-hidden">操作</span></th>
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
                                <td class="col-id">
                                    <span class="tag tag-mono">#{{ problem.id }}</span>
                                </td>
                                <td>
                                    <div class="problem-title-wrapper">
                                        <span class="problem-title">{{ problem.title || '无标题' }}</span>
                                        <span class="problem-desc">{{ problem.description || '暂无描述' }}</span>
                                    </div>
                                </td>
                                <td class="col-difficulty">
                                    <span class="difficulty-badge" :class="getDifficultyClass(problem.star)">
                                        {{ getDifficultyText(problem.star) }}
                                    </span>
                                </td>
                                <td class="col-action">
                                    <i class="bi bi-chevron-right problem-arrow"></i>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>

            <!-- 卡片底部 -->
            <div class="page-card-footer pagination-footer">
                <span class="text-muted">
                    <template v-if="totalPages > 0">第 {{ currentPage }} / {{ totalPages }} 页 · </template>共 {{ total }} 道题目
                </span>
                <div class="pagination-controls" aria-label="题库分页">
                    <button
                        type="button"
                        class="page-arrow"
                        aria-label="上一页"
                        :disabled="loading || currentPage <= 1"
                        @click="changePage(-1)"
                    ><i class="bi bi-chevron-left"></i></button>
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
                    <span class="page-total">/ {{ Math.max(totalPages, 1) }}</span>
                    <button
                        type="button"
                        class="page-arrow"
                        aria-label="下一页"
                        :disabled="loading || totalPages === 0 || currentPage >= totalPages"
                        @click="changePage(1)"
                    ><i class="bi bi-chevron-right"></i></button>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import { ref, onMounted } from 'vue'
import $ from 'jquery'
import { API_BASE } from '@/config.mjs'
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
                url: `${API_BASE}/oj/topic/getlist/`,
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
.col-id {
    width: 96px;
}

.col-difficulty {
    width: 120px;
}

.col-action {
    width: 48px;
    text-align: right;
}

.problem-row {
    cursor: pointer;
}

.problem-row:focus-visible {
    outline: 2px solid var(--app-primary);
    outline-offset: -2px;
}

.problem-row:hover .problem-title {
    color: var(--app-primary);
}

.problem-row:hover .problem-arrow {
    transform: translateX(3px);
    color: var(--app-primary);
}

.problem-title-wrapper {
    display: flex;
    flex-direction: column;
    gap: 0.25rem;
    min-width: 0;
}

.problem-title {
    font-weight: 600;
    color: var(--app-text);
    transition: color 0.15s ease;
}

.problem-desc {
    font-size: 0.85rem;
    line-height: 1.5;
    color: var(--app-text-muted);
    max-width: 640px;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

.problem-arrow {
    color: #cbd5e1;
    transition: transform 0.2s ease, color 0.2s ease;
}

.pagination-controls {
    justify-content: flex-end;
}

@media (max-width: 767.98px) {
    .col-action {
        display: none;
    }

    .problem-desc {
        max-width: 220px;
    }

    .pagination-footer {
        flex-direction: column;
        align-items: stretch;
    }

    .pagination-controls {
        justify-content: center;
    }
}
</style>
