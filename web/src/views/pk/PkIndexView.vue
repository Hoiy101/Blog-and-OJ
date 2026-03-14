<template>
    <div class="container content-field">
        <!-- 融合搜索框的博客卡片 -->
        <div class="card blog-card">
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
                            >
                            <button class="btn btn-outline-primary" type="button">
                                搜索
                            </button>
                        </div>
                    </div>
                </div>
            </div>

            <!-- 博客列表部分 -->
            <div class="card-body p-0 blog-list-container">
                <!-- 无数据提示 -->
                <div v-if="records.length === 0" class="text-center py-5">
                    <div class="empty-state">
                        <i class="bi bi-journal-text display-4 text-muted"></i>
                        <p class="mt-3 text-muted">暂无博客文章</p>
                    </div>
                </div>

                <!-- 博客列表 -->
                <div v-else class="blog-list">
                    <div v-for="record in records" :key="record.id" class="blog-item border-bottom p-4">
                        <div class="d-flex justify-content-between align-items-start mb-2">
                            <h5 class="card-title mb-0">{{ record.title }}</h5>
                            <span class="badge bg-light text-dark small">#{{ record.id }}</span>
                        </div>
                        <h6 class="card-subtitle mb-2 text-muted">
                            <small>
                                <i class="bi bi-calendar-plus"></i> 创建于: {{ formatTime(record.createtime) }} | 
                                <i class="bi bi-pencil"></i> 更新于: {{ formatTime(record.modifytime) }}
                            </small>
                        </h6>
                        <p class="card-text text-secondary mb-3">{{ record.description || '暂无简介' }}</p>
                        <div class="d-flex justify-content-end">
                            <button class="btn btn-sm btn-outline-primary me-2">查看详情</button>
                        </div>
                    </div>
                </div>
            </div>

            <!-- 卡片底部（可选） -->
            <div class="card-footer text-muted text-center">
                共 {{ records.length }} 篇文章
            </div>
        </div>
    </div>
</template>

<script>
import { ref, onMounted } from 'vue'
import $ from 'jquery'
import { useStore } from 'vuex'

export default {
    name: 'BlogHome',
    
    setup() {
        const store = useStore()
        const records = ref([])
        const loading = ref(false)

        // 时间格式化函数
        const formatTime = (time) => {
            if (!time || time === 'null' || time === 'undefined') return '未更新'
            try {
                const date = new Date(time)
                return date.toLocaleDateString('zh-CN', {
                    year: 'numeric',
                    month: '2-digit',
                    day: '2-digit',
                    hour: '2-digit',
                    minute: '2-digit'
                })
            } catch (e) {
                return '时间格式错误'
            }
        }

        const getlist_record = () => {
            loading.value = true
            $.ajax({
                url: "http://127.0.0.1:3000/user/bot/all/getlist/",
                type: "get",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                success(resp) {
                    records.value = resp || []
                },
                error(jqXHR, textStatus, errorThrown) {
                    console.error("获取记录失败:", textStatus, errorThrown)
                },
                complete() {
                    loading.value = false
                }
            })
        }

        onMounted(() => {
            getlist_record()
        })

        return {
            records,
            loading,
            formatTime
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
.blog-card {
    border: 1px solid #e0e0e0;
    border-radius: 12px;
    overflow: hidden;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.05);
    transition: all 0.3s ease;
    background-color: #fff;
}

.blog-card:hover {
    box-shadow: 0 6px 20px rgba(0, 0, 0, 0.08);
}

/* 卡片头部样式 */
.blog-card .card-header {
    background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
    padding: 1.5rem 2rem;
    border-bottom: 2px solid #dee2e6;
}

.blog-card .card-header h5 {
    font-weight: 600;
    color: #2c3e50;
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

/* 空状态样式 */
.empty-state {
    padding: 4rem 2rem;
    color: #6c757d;
}

.empty-state i {
    opacity: 0.5;
    font-size: 4rem;
}

/* 卡片底部样式 */
.blog-card .card-footer {
    background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
    padding: 1rem 2rem;
    border-top: 1px solid #dee2e6;
}

/* 响应式调整 */
@media (max-width: 768px) {
    .content-field {
        margin-top: 1rem;
        padding: 0.5rem;
    }
    
    .blog-card .card-header {
        padding: 1rem;
    }
    
    .blog-card .card-header .row > div {
        width: 100%;
        margin-bottom: 1rem;
    }
    
    .blog-item {
        padding: 1.5rem;
    }
    
    .blog-item .d-flex {
        flex-wrap: wrap;
    }
    
    .blog-item .d-flex .btn {
        margin-top: 0.5rem;
    }
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
</style>