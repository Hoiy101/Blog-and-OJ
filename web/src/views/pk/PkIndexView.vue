<template>
    <div class="container content-field">
        <!-- 博客列表卡片 -->
        <div v-if="!showDetail" class="card blog-card">
            <!-- 搜索框在卡片顶部 -->
            <div class="card-header border-bottom bg-light">
                <div class="row align-items-center">
                    <div class="col-md-8">
                        <h5 class="mb-0">博客文章</h5>
                        <p class="text-muted mb-0 small">共 {{ records.length }} 篇文章</p>
                    </div>
                    <div class="col-md-4">
                        <div class="input-group">
                            <input 
                                type="text" 
                                class="form-control" 
                                placeholder="搜索文章标题或内容..." 
                                aria-label="搜索博客文章"
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

            <!-- 博客列表部分 -->
            <div class="card-body p-0 blog-list-container">
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
                        <button class="btn btn-primary mt-2" @click="getBlogList">重试</button>
                    </div>
                </div>

                <!-- 无数据提示 -->
                <div v-else-if="records.length === 0" class="text-center py-5">
                    <div class="empty-state">
                        <i class="bi bi-journal-text display-4 text-muted"></i>
                        <p class="mt-3 text-muted">暂无博客文章</p>
                        <button class="btn btn-outline-primary mt-2" @click="getBlogList">刷新</button>
                    </div>
                </div>

                <!-- 博客列表 -->
                <div v-else class="blog-list">
                    <div v-for="record in filteredRecords" :key="record.id" class="blog-item border-bottom p-4">
                        <div class="d-flex justify-content-between align-items-start mb-2">
                            <h5 class="card-title mb-0">{{ record.title || '无标题' }}</h5>
                            <span class="badge bg-light text-dark small">#{{ record.id || '未知' }}</span>
                        </div>
                        <h6 class="card-subtitle mb-2 text-muted">
                            <small>
                                <i class="bi bi-calendar-plus"></i> 创建于: {{ formatTime(record.createtime) }} | 
                                <i class="bi bi-pencil"></i> 更新于: {{ formatTime(record.modifytime) }}
                            </small>
                        </h6>
                        <p class="card-text text-secondary mb-3">{{ record.description || '暂无简介' }}</p>
                        <div class="d-flex justify-content-end">
                            <button 
                                class="btn btn-sm btn-outline-primary me-2"
                                @click="viewBlogDetail(record.id)"
                            >
                                查看详情
                            </button>
                        </div>
                    </div>
                </div>
            </div>

            <!-- 卡片底部 -->
            <div class="card-footer text-muted text-center">
                共 {{ filteredRecords.length }} 篇文章
            </div>
        </div>

        <!-- 博客详情页面 -->
        <div v-else class="card blog-detail-card">
            <!-- 详情页头部 -->
            <div class="card-header border-bottom bg-light d-flex justify-content-between align-items-center">
                <div>
                    <button class="btn btn-outline-secondary btn-sm" @click="backToList">
                        <i class="bi bi-arrow-left"></i> 返回列表
                    </button>
                </div>
                <h5 class="mb-0">博客详情</h5>
                <div>
                    <span class="badge bg-info">ID: {{ currentBlog.id }}</span>
                </div>
            </div>

            <!-- 博客内容区域 -->
            <div class="card-body blog-detail-container">
                <!-- 加载状态 -->
                <div v-if="detailLoading" class="text-center py-5">
                    <div class="loading-spinner"></div>
                    <p class="mt-3 text-muted">加载博客内容中...</p>
                </div>

                <!-- 错误状态 -->
                <div v-else-if="detailError" class="text-center py-5">
                    <div class="error-state text-danger">
                        <i class="bi bi-exclamation-triangle display-4"></i>
                        <p class="mt-3">加载失败: {{ detailError }}</p>
                        <button class="btn btn-primary mt-2" @click="backToList">返回列表</button>
                    </div>
                </div>

                <!-- 博客详情内容 -->
                <div v-else class="blog-detail-content">
                    <!-- 博客标题 -->
                    <div class="blog-header mb-4">
                        <h1 class="blog-title">{{ currentBlog.title }}</h1>
                        <div class="blog-meta text-muted mb-3">
                            <span class="me-3">
                                <i class="bi bi-calendar-plus"></i> 创建时间: {{ formatTime(currentBlog.createtime) }}
                            </span>
                            <span>
                                <i class="bi bi-pencil"></i> 更新时间: {{ formatTime(currentBlog.modifytime) }}
                            </span>
                        </div>
                        <div v-if="currentBlog.description" class="blog-description bg-light p-3 rounded mb-4">
                            <p class="lead mb-0">{{ currentBlog.description }}</p>
                        </div>
                    </div>

                    <!-- 博客内容 -->
                    <div class="blog-body">
                        <div class="blog-content" v-html="formatContent(currentBlog.content)"></div>
                    </div>

                    <!-- 操作按钮 -->
                    <div class="blog-actions mt-4 pt-4 border-top">
                        <button class="btn btn-outline-secondary" @click="backToList">
                            <i class="bi bi-arrow-left"></i> 返回列表
                        </button>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import { ref, onMounted, computed } from 'vue'
import $ from 'jquery'
import { useStore } from 'vuex'

export default {
    name: 'BlogHome',
    
    setup() {
        const store = useStore()
        const records = ref([])
        const loading = ref(false)
        const searchKeyword = ref('')
        const error = ref(null)
        
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

        // 计算属性：根据搜索关键词过滤记录
        const filteredRecords = computed(() => {
            if (!searchKeyword.value.trim()) {
                return records.value
            }
            
            const keyword = searchKeyword.value.toLowerCase().trim()
            return records.value.filter(record => {
                return (
                    (record.title && record.title.toLowerCase().includes(keyword)) ||
                    (record.description && record.description.toLowerCase().includes(keyword))
                )
            })
        })

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

        // 格式化博客内容
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
        const getBlogList = () => {
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
                success(resp) {
                    console.log('博客列表API响应原始数据:', resp)
                    
                    if (Array.isArray(resp)) {
                        records.value = resp
                        console.log('成功获取到', resp.length, '篇博客')
                    } else if (resp && resp.data && Array.isArray(resp.data)) {
                        records.value = resp.data
                        console.log('成功获取到', resp.data.length, '篇博客')
                    } else if (resp && resp.records && Array.isArray(resp.records)) {
                        records.value = resp.records
                        console.log('成功获取到', resp.records.length, '篇博客')
                    } else {
                        console.warn('API返回数据格式异常，使用模拟数据:', resp)
                    }
                    
                    if (records.value.length === 0) {
                        console.log('没有获取到博客数据')
                    }
                },
                error(jqXHR, textStatus, errorThrown) {
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
                    loading.value = false
                }
            })
        }

        // 搜索功能
        const handleSearch = () => {
            console.log('搜索关键词:', searchKeyword.value)
        }

        onMounted(() => {
            getBlogList()
        })

        return {
            records,
            filteredRecords,
            loading,
            searchKeyword,
            error,
            showDetail,
            currentBlog,
            detailLoading,
            detailError,
            formatTime,
            formatContent,
            viewBlogDetail,
            backToList,
            getBlogList,
            handleSearch
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

/* 博客卡片样式 */
.blog-card, .blog-detail-card {
    border: 1px solid #e0e0e0;
    border-radius: 12px;
    overflow: hidden;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.05);
    transition: all 0.3s ease;
    background-color: #fff;
}

.blog-card:hover, .blog-detail-card:hover {
    box-shadow: 0 6px 20px rgba(0, 0, 0, 0.08);
}

/* 卡片头部样式 */
.blog-card .card-header, .blog-detail-card .card-header {
    background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
    padding: 1.5rem 2rem;
    border-bottom: 2px solid #dee2e6;
}

.blog-card .card-header h5, .blog-detail-card .card-header h5 {
    font-weight: 600;
    color: #2c3e50;
}

/* 博客详情头部 */
.blog-detail-card .card-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
}

/* 搜索框样式 */
.blog-card .input-group {
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.05);
}

.blog-card .form-control {
    border-color: #ced4da;
    border-right: none;
    padding: 0.75rem 1rem;
    font-size: 0.95rem;
    height: 42px;
}

.blog-card .form-control:focus {
    border-color: #86b7fe;
    box-shadow: 0 0 0 0.2rem rgba(13, 110, 253, 0.15);
    z-index: 1;
}

.blog-card .btn-outline-primary {
    border-color: #0d6efd;
    color: #0d6efd;
    padding: 0.5rem 1.5rem;
    height: 42px;
    font-weight: 500;
    transition: all 0.2s;
}

.blog-card .btn-outline-primary:hover {
    background-color: #0d6efd;
    color: white;
    transform: translateY(-1px);
}

/* 博客列表项样式 */
.blog-item {
    transition: all 0.2s ease;
    background-color: #fff;
    cursor: pointer;
}

.blog-item:hover {
    background-color: #f8fafc;
    transform: translateX(4px);
}

.blog-item:last-child {
    border-bottom: none !important;
}

.blog-item .card-title {
    font-weight: 600;
    color: #2c3e50;
    font-size: 1.25rem;
    line-height: 1.4;
}

.blog-item .card-text {
    font-size: 0.95rem;
    line-height: 1.6;
    color: #5a6268;
    min-height: 60px;
}

.blog-item .badge {
    background-color: #f8f9fa;
    color: #6c757d;
    font-weight: 500;
    padding: 0.35em 0.65em;
    border: 1px solid #dee2e6;
}

/* 博客详情样式 */
.blog-detail-container {
    padding: 2rem;
}

.blog-header {
    border-bottom: 2px solid #f0f0f0;
    padding-bottom: 1.5rem;
    margin-bottom: 2rem;
}

.blog-title {
    font-size: 2.5rem;
    font-weight: 700;
    color: #2c3e50;
    margin-bottom: 1rem;
    line-height: 1.2;
}

.blog-meta {
    font-size: 0.9rem;
    color: #6c757d;
    margin-bottom: 1.5rem;
}

.blog-meta i {
    margin-right: 0.5rem;
}

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

/* 博客内容样式 */
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
.blog-card .card-footer, .blog-detail-card .card-footer {
    background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
    padding: 1rem 2rem;
    border-top: 1px solid #dee2e6;
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
    
    .blog-card .card-header, .blog-detail-card .card-header {
        padding: 1rem;
    }
    
    .blog-card .card-header .row > div, 
    .blog-detail-card .card-header .row > div {
        width: 100%;
        margin-bottom: 1rem;
    }
    
    .blog-item, .blog-detail-container {
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