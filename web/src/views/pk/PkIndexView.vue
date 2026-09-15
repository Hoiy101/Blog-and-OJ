<template>
    <div class="container page-shell">
        <!-- 博客列表卡片 -->
        <div v-if="!showDetail" class="page-card blog-card">
            <!-- 搜索框在卡片顶部 -->
            <div class="page-card-header">
                <div class="d-flex align-items-center gap-3">
                    <span class="page-title-icon"><i class="bi bi-journal-richtext"></i></span>
                    <div>
                        <h1>博客文章</h1>
                        <p class="page-card-subtitle">共 {{ total }} 篇文章，按更新时间排序</p>
                    </div>
                </div>
                <div class="search-box">
                    <i class="bi bi-search"></i>
                    <input
                        type="text"
                        class="form-control"
                        placeholder="搜索文章标题或内容..."
                        aria-label="搜索博客文章"
                        v-model="searchKeyword"
                        @keyup.enter="handleSearch"
                    >
                    <button class="btn btn-primary search-btn" type="button" @click="handleSearch">
                        搜索
                    </button>
                </div>
            </div>

            <!-- 博客列表部分 -->
            <div class="blog-list-container">
                <!-- 加载状态 -->
                <div v-if="loading" class="blog-skeleton" aria-busy="true" aria-label="加载中">
                    <div v-for="n in 4" :key="n" class="blog-skeleton-item">
                        <span class="skeleton-line skeleton-title"></span>
                        <span class="skeleton-line skeleton-meta"></span>
                        <span class="skeleton-line"></span>
                        <span class="skeleton-line skeleton-short"></span>
                    </div>
                </div>

                <!-- 错误状态 -->
                <div v-else-if="error" class="state-panel is-error">
                    <span class="state-icon"><i class="bi bi-exclamation-triangle"></i></span>
                    <p class="state-title">加载失败</p>
                    <p class="state-text">{{ error }}</p>
                    <button class="btn btn-primary" @click="retryBlogList"><i class="bi bi-arrow-clockwise"></i> 重试</button>
                </div>

                <!-- 无数据提示 -->
                <div v-else-if="records.length === 0" class="state-panel">
                    <span class="state-icon"><i class="bi bi-journal-x"></i></span>
                    <p class="state-title">暂无博客文章</p>
                    <p class="state-text">换个关键词试试，或者去个人空间发布第一篇博客。</p>
                    <button class="btn btn-outline-primary" @click="getBlogList(1)"><i class="bi bi-arrow-clockwise"></i> 刷新</button>
                </div>

                <!-- 博客列表 -->
                <div v-else class="blog-list">
                    <div
                        v-for="record in records"
                        :key="record.id"
                        class="blog-item border-bottom p-4"
                        role="link"
                        tabindex="0"
                        @click="viewBlogDetail(record.id)"
                        @keydown.enter="viewBlogDetail(record.id)"
                        @keydown.space.prevent="viewBlogDetail(record.id)"
                    >
                        <div class="blog-item-main">
                            <h2 class="blog-item-title">{{ record.title || '无标题' }}</h2>
                            <p class="blog-item-desc">{{ record.description || '暂无简介' }}</p>
                            <div class="blog-item-meta">
                                <span><i class="bi bi-calendar-plus"></i> 创建于 {{ formatTime(record.createtime) }}</span>
                                <span><i class="bi bi-pencil"></i> 更新于 {{ formatTime(record.modifytime) }}</span>
                            </div>
                        </div>
                        <div class="blog-item-side">
                            <span class="tag tag-mono">#{{ record.id || '未知' }}</span>
                            <i class="bi bi-chevron-right blog-item-arrow"></i>
                        </div>
                    </div>
                </div>
            </div>

            <!-- 卡片底部 -->
            <div class="page-card-footer pagination-footer">
                <span class="text-muted">
                    <template v-if="totalPages > 0">第 {{ currentPage }} / {{ totalPages }} 页 · </template>共 {{ total }} 篇文章
                </span>
                <div class="pagination-controls" aria-label="博客分页">
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

        <!-- 博客详情页面 -->
        <article v-else class="page-card blog-detail-card">
            <!-- 详情页头部 -->
            <div class="page-card-header blog-detail-header">
                <button class="btn btn-outline-secondary btn-sm" @click="backToList">
                    <i class="bi bi-arrow-left"></i> 返回列表
                </button>
                <h2 class="blog-detail-heading">博客详情</h2>
                <span class="tag tag-mono">#{{ currentBlog.id }}</span>
            </div>

            <!-- 博客内容区域 -->
            <div class="blog-detail-container">
                <!-- 加载状态 -->
                <div v-if="detailLoading" class="state-panel">
                    <div class="loading-spinner"></div>
                    <p class="state-text">加载博客内容中…</p>
                </div>

                <!-- 错误状态 -->
                <div v-else-if="detailError" class="state-panel is-error">
                    <span class="state-icon"><i class="bi bi-exclamation-triangle"></i></span>
                    <p class="state-title">加载失败</p>
                    <p class="state-text">{{ detailError }}</p>
                    <button class="btn btn-primary" @click="backToList">返回列表</button>
                </div>

                <!-- 博客详情内容 -->
                <div v-else class="blog-detail-content">
                    <header class="blog-header">
                        <h1 class="blog-title">{{ currentBlog.title }}</h1>
                        <div class="blog-meta">
                            <span><i class="bi bi-calendar-plus"></i> 创建时间 {{ formatTime(currentBlog.createtime) }}</span>
                            <span><i class="bi bi-pencil"></i> 更新时间 {{ formatTime(currentBlog.modifytime) }}</span>
                        </div>
                        <p v-if="currentBlog.description" class="blog-description">{{ currentBlog.description }}</p>
                    </header>

                    <!-- 博客内容 -->
                    <MarkdownContent class="article-body blog-content" :source="currentBlog.content" />

                    <!-- 操作按钮 -->
                    <footer class="blog-actions">
                        <button class="btn btn-outline-secondary" @click="backToList">
                            <i class="bi bi-arrow-left"></i> 返回列表
                        </button>
                    </footer>
                </div>
            </div>
        </article>
    </div>
</template>

<script>
import { ref, onMounted } from 'vue'
import $ from 'jquery'
import { useStore } from 'vuex'
import MarkdownContent from '../../components/MarkdownContent.vue'
import { clampPage, normalizePageResponse, paginationQuery } from '../../utils/pagination.mjs'

export default {
    name: 'BlogHome',
    components: { MarkdownContent },
    
    setup() {
        const store = useStore()
        const records = ref([])
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
        const blogListRequestId = ref(0)
        
        // 博客详情相关状态
        const showDetail = ref(false)
        const currentBlog = ref({
            id: null,
            title: '',
            description: '',
            content: '',
            createtime: '',
            modifytime: ''
        })
        const detailLoading = ref(false)
        const detailError = ref(null)

        // 时间格式化函数
        const formatTime = (time) => {
            if (!time || time === 'null' || time === 'undefined' || time === '') return '未设置'
            try {
                const date = new Date(time)
                if (isNaN(date.getTime())) {
                    return '时间格式错误'
                }
                return date.toLocaleDateString('zh-CN', {
                    year: 'numeric',
                    month: '2-digit',
                    day: '2-digit',
                    hour: '2-digit',
                    minute: '2-digit',
                    second: '2-digit'
                })
            } catch (e) {
                console.error('时间格式化错误:', e)
                return '时间格式错误'
            }
        }

        // 查看博客详情
        const viewBlogDetail = (blogId) => {
            if (!blogId) {
                console.error('博客ID不能为空')
                alert('获取博客ID失败')
                return
            }

            console.log('获取博客详情，ID:', blogId)
            
            // 重置详情状态
            showDetail.value = true
            detailLoading.value = true
            detailError.value = null
            
            // 先清空当前博客数据
            currentBlog.value = {
                id: null,
                title: '',
                description: '',
                content: '',
                createtime: '',
                modifytime: ''
            }
            
            // 发送请求获取博客详情
            $.ajax({
                url: "http://127.0.0.1:3000/user/bot/get/",
                type: "GET",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                data: {
                    id: blogId,
                },
                success(resp) {
                    console.log('博客详情API响应:', resp)
                    detailLoading.value = false
                    
                    if (resp && (resp.id || resp.title)) {
                        // 根据您提供的后端数据格式
                        currentBlog.value = {
                            id: resp.id || blogId,
                            title: resp.title || '无标题',
                            description: resp.description || '暂无简介',
                            content: resp.content || '',
                            createtime: resp.createtime || '',
                            modifytime: resp.modifytime || ''
                        }
                        console.log('博客详情加载成功:', currentBlog.value.title)
                    } else {
                        detailError.value = '获取的博客数据格式不正确'
                        console.error('博客数据格式错误:', resp)
                    }
                },
                error(jqXHR, textStatus, errorThrown) {
                    console.error('获取博客详情失败:', jqXHR.status, textStatus, errorThrown)
                    console.error('响应内容:', jqXHR.responseText)
                    detailLoading.value = false
                    
                    let errorMsg = '网络请求失败'
                    if (jqXHR.status === 0) {
                        errorMsg = '无法连接到服务器，请检查网络连接'
                    } else if (jqXHR.status === 401) {
                        errorMsg = '登录已过期，请重新登录'
                    } else if (jqXHR.status === 404) {
                        errorMsg = '博客不存在或已被删除'
                    } else if (jqXHR.status === 500) {
                        errorMsg = '服务器内部错误，请稍后重试'
                    } else {
                        errorMsg = `请求失败: ${jqXHR.status} ${errorThrown}`
                    }
                    
                    detailError.value = errorMsg
                    
                    // 测试用的模拟数据
                    console.log('使用模拟数据测试...')
                    setTimeout(() => {
                        detailError.value = null
                    }, 500)
                }
            })
        }

        // 返回博客列表
        const backToList = () => {
            showDetail.value = false
            currentBlog.value = {
                id: null,
                title: '',
                description: '',
                content: '',
                createtime: '',
                modifytime: ''
            }
        }

        // 获取博客列表
        const resetBlogPage = () => {
            records.value = []
            currentPage.value = 1
            pageInput.value = 1
            total.value = 0
            totalPages.value = 0
        }

        const applyBlogPage = (resp) => {
            const page = normalizePageResponse(resp, 10)
            records.value = page.records
            currentPage.value = page.currentPage
            pageInput.value = page.currentPage
            total.value = page.total
            totalPages.value = page.totalPages
        }

        const getBlogList = (
            requestedPage = currentPage.value,
            requestedKeyword = activeKeyword.value
        ) => {
            const requestData = paginationQuery(requestedPage, requestedKeyword)
            const requestId = ++blogListRequestId.value
            retryPage.value = requestData.page
            retryKeyword.value = requestData.keyword
            loading.value = true
            error.value = null
            
            console.log('开始获取博客列表...')
            console.log('Token状态:', store.state.user.token ? '有token' : '无token')
            
            $.ajax({
                url: "http://127.0.0.1:3000/user/bot/all/getlist/",
                type: "GET",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                data: requestData,
                success(resp) {
                    if (requestId !== blogListRequestId.value) return
                    console.log('博客列表API响应原始数据:', resp)
                    try {
                        applyBlogPage(resp)
                        console.log('成功获取到', records.value.length, '篇当前页博客')
                    } catch (responseError) {
                        resetBlogPage()
                        error.value = responseError.message
                    }
                },
                error(jqXHR, textStatus, errorThrown) {
                    if (requestId !== blogListRequestId.value) return
                    console.error("获取博客列表失败:", jqXHR.status, textStatus, errorThrown)
                    
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
                    if (requestId !== blogListRequestId.value) return
                    loading.value = false
                }
            })
        }

        // 搜索功能
        const handleSearch = () => {
            activeKeyword.value = searchKeyword.value.trim()
            getBlogList(1)
        }

        const changePage = (offset) => {
            getBlogList(currentPage.value + offset)
        }

        const goToPage = (value) => {
            getBlogList(clampPage(value, totalPages.value))
        }

        const retryBlogList = () => {
            getBlogList(retryPage.value, retryKeyword.value)
        }

        onMounted(() => {
            getBlogList()
        })

        return {
            records,
            loading,
            searchKeyword,
            error,
            currentPage,
            pageInput,
            total,
            totalPages,
            showDetail,
            currentBlog,
            detailLoading,
            detailError,
            formatTime,
            viewBlogDetail,
            backToList,
            getBlogList,
            handleSearch,
            changePage,
            goToPage,
            retryBlogList
        }
    }
}
</script>

<style scoped>
/* 博客列表项 */
.blog-item {
    display: flex;
    align-items: flex-start;
    justify-content: space-between;
    gap: 1.25rem;
    transition: background-color 0.2s ease;
    background-color: #fff;
    cursor: pointer;
}

.blog-item:hover {
    background-color: #f8fafc;
}

.blog-item:hover .blog-item-title {
    color: var(--app-primary);
}

.blog-item:hover .blog-item-arrow {
    transform: translateX(4px);
    color: var(--app-primary);
}

.blog-item:focus-visible {
    outline: 2px solid var(--app-primary);
    outline-offset: -2px;
}

.blog-item:last-child {
    border-bottom: none !important;
}

.blog-item-main {
    min-width: 0;
    flex: 1;
}

.blog-item-title {
    margin: 0 0 0.45rem;
    font-size: 1.2rem;
    font-weight: 700;
    line-height: 1.4;
    color: var(--app-text);
    transition: color 0.15s ease;
}

.blog-item-desc {
    margin: 0 0 0.75rem;
    color: var(--app-text-secondary);
    font-size: 0.95rem;
    line-height: 1.65;
    display: -webkit-box;
    -webkit-line-clamp: 2;
    -webkit-box-orient: vertical;
    overflow: hidden;
}

.blog-item-meta {
    display: flex;
    flex-wrap: wrap;
    gap: 0.4rem 1.25rem;
    font-size: 0.82rem;
    color: var(--app-text-muted);
}

.blog-item-meta i {
    margin-right: 0.25rem;
}

.blog-item-side {
    display: flex;
    flex-direction: column;
    align-items: flex-end;
    gap: 0.75rem;
    flex-shrink: 0;
}

.blog-item-arrow {
    color: #cbd5e1;
    font-size: 1.1rem;
    transition: transform 0.2s ease, color 0.2s ease;
}

/* 骨架屏 */
.blog-skeleton-item {
    display: flex;
    flex-direction: column;
    gap: 0.7rem;
    padding: 1.5rem;
    border-bottom: 1px solid var(--app-border);
}

.blog-skeleton-item:last-child {
    border-bottom: 0;
}

.skeleton-line {
    display: block;
    height: 12px;
    border-radius: 6px;
    background: linear-gradient(90deg, #eef2f7 25%, #f8fafc 50%, #eef2f7 75%);
    background-size: 200% 100%;
    animation: skeleton-shimmer 1.4s ease infinite;
}

.skeleton-title {
    width: 45%;
    height: 18px;
}

.skeleton-meta {
    width: 30%;
    height: 10px;
}

.skeleton-short {
    width: 65%;
}

@keyframes skeleton-shimmer {
    0% { background-position: 200% 0; }
    100% { background-position: -200% 0; }
}

/* 分页 */
.pagination-controls {
    justify-content: flex-end;
}

/* 博客详情 */
.blog-detail-header {
    padding-top: 0.9rem;
    padding-bottom: 0.9rem;
}

.blog-detail-heading {
    margin: 0;
    font-size: 1rem;
    font-weight: 600;
    color: var(--app-text-secondary);
}

.blog-detail-container {
    padding: 2.5rem 3rem 2rem;
}

.blog-header {
    padding-bottom: 1.5rem;
    margin-bottom: 2rem;
    border-bottom: 1px solid var(--app-border);
}

.blog-title {
    margin: 0 0 1rem;
    font-size: 2.2rem;
    font-weight: 800;
    line-height: 1.25;
    letter-spacing: -0.01em;
    color: var(--app-text);
}

.blog-meta {
    display: flex;
    flex-wrap: wrap;
    gap: 0.4rem 1.5rem;
    font-size: 0.88rem;
    color: var(--app-text-muted);
}

.blog-meta i {
    margin-right: 0.3rem;
}

.blog-description {
    margin: 1.5rem 0 0;
    padding: 1rem 1.25rem;
    border-left: 4px solid var(--app-primary);
    border-radius: 0 var(--app-radius-sm) var(--app-radius-sm) 0;
    background: var(--app-surface-soft);
    color: var(--app-text-secondary);
    font-size: 1.05rem;
    line-height: 1.7;
}

.blog-actions {
    display: flex;
    flex-wrap: wrap;
    gap: 0.75rem;
    margin-top: 2.5rem;
    padding-top: 1.5rem;
    border-top: 1px solid var(--app-border);
}

@media (max-width: 767.98px) {
    .blog-item {
        flex-direction: column;
        gap: 0.75rem;
        padding: 1.25rem !important;
    }

    .blog-item-side {
        flex-direction: row;
        align-items: center;
        width: 100%;
        justify-content: space-between;
    }

    .blog-detail-container {
        padding: 1.5rem 1.25rem;
    }

    .blog-title {
        font-size: 1.6rem;
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
