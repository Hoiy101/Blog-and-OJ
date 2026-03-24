<template>
    <div class="container content-field">
        <div class="card oj-card">
            <!-- 卡片头部 -->
            <div class="card-header border-bottom bg-light">
                <div class="row align-items-center">
                    <div class="col-md-8">
                        <h5 class="mb-0">题库列表</h5>
                        <p class="text-muted mb-0 small">共 {{ problems.length }} 道题目</p>
                    </div>
                    <div class="col-md-4">
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
                        <button class="btn btn-primary mt-2" @click="getProblemList">重试</button>
                    </div>
                </div>

                <!-- 无数据提示 -->
                <div v-else-if="problems.length === 0" class="text-center py-5">
                    <div class="empty-state">
                        <i class="bi bi-clipboard-data display-4 text-muted"></i>
                        <p class="mt-3 text-muted">暂无题目</p>
                        <button class="btn btn-outline-primary mt-2" @click="getProblemList">刷新</button>
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
                                <th scope="col" style="width: 100px;" class="text-center">操作</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr v-for="problem in filteredProblems" :key="problem.id" class="problem-row">
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
                                <td class="text-center">  
                                    <button 
                                        class="btn btn-sm btn-outline-primary"
                                        @click="handleView(problem.id)">
                                        查看
                                    </button>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                </div>
            </div>

            <!-- 卡片底部 -->
            <div class="card-footer text-muted text-center">
                共 {{ filteredProblems.length }} 道题目
            </div>
        </div>
    </div>
</template>

<script>
import { ref, onMounted, computed } from 'vue'
import $ from 'jquery'
import { useStore } from 'vuex'
import { useRouter } from 'vue-router';

export default {
    name: 'OJProblemList',
    
    setup() {
        const store = useStore()
        const problems = ref([])
        const loading = ref(false)
        const searchKeyword = ref('')
        const error = ref(null)
        const router = useRouter();

        // 计算属性：根据搜索关键词过滤题目
        const filteredProblems = computed(() => {
            if (!searchKeyword.value.trim()) {
                return problems.value
            }
            
            const keyword = searchKeyword.value.toLowerCase().trim()
            return problems.value.filter(problem => {
                return (
                    (problem.id && problem.id.toString().includes(keyword)) ||
                    (problem.title && problem.title.toLowerCase().includes(keyword)) ||
                    (problem.description && problem.description.toLowerCase().includes(keyword))
                )
            })
        })
        const handleView = (id) => {
            store.commit("updatetopicid", id);
            router.push({ name: 'Details' });
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
        const getProblemList = () => {
            loading.value = true
            error.value = null
            
            console.log('开始获取题目列表...')
            
            $.ajax({
                url: "http://127.0.0.1:3000/oj/topic/getlist/",
                type: "GET",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                success(resp) {
                    console.log('题目列表API响应:', resp)
                    
                    if (Array.isArray(resp)) {
                        problems.value = resp
                        console.log('成功获取到', resp.length, '道题目')
                    } else if (resp && resp.data && Array.isArray(resp.data)) {
                        problems.value = resp.data
                        console.log('成功获取到', resp.data.length, '道题目')
                    } else if (resp && resp.list && Array.isArray(resp.list)) {
                        problems.value = resp.list
                        console.log('成功获取到', resp.list.length, '道题目')
                    } else {
                        console.warn('API返回数据格式异常，使用模拟数据')
                        // 使用模拟数据
                        problems.value = generateMockProblems(15)
                    }
                    
                    if (problems.value.length === 0) {
                        console.log('没有获取到题目数据')
                    }
                },
                error(jqXHR, textStatus, errorThrown) {
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
                    
                    // 使用模拟数据
                    console.log('使用模拟数据进行测试')
                    problems.value = generateMockProblems(10)
                },
                complete() {
                    loading.value = false
                }
            })
        }

        // 生成模拟题目数据
        const generateMockProblems = (count) => {
            const mockTitles = [
                '两数之和', '反转链表', '有效的括号', '合并两个有序链表', 
                '最大子数组和', '爬楼梯', '二叉树的层序遍历', '最长公共前缀',
                '罗马数字转整数', '回文数', '合并区间', '三数之和',
                '盛最多水的容器', '括号生成', '全排列', '旋转图像'
            ]
            
            const mockDescriptions = [
                '给定一个整数数组和一个目标值，找出数组中和为目标值的两个数。',
                '反转一个单链表。',
                '给定一个只包括括号的字符串，判断字符串是否有效。',
                '将两个有序链表合并为一个新的有序链表并返回。',
                '给定一个整数数组，找到一个具有最大和的连续子数组。',
                '假设你正在爬楼梯。需要n阶你才能到达楼顶。',
                '给你一个二叉树，请你返回其按层序遍历得到的节点值。',
                '编写一个函数来查找字符串数组中的最长公共前缀。',
                '罗马数字包含七种字符，将其转换成整数。',
                '判断一个整数是否是回文数。',
                '给出一个区间的集合，请合并所有重叠的区间。',
                '给你一个包含n个整数的数组，判断是否存在三个元素使和为0。',
                '给你n个非负整数，每个数代表坐标中的一个点，找出其中两个点，使得它们与x轴共同构成的容器可以容纳最多的水。',
                '数字n代表生成括号的对数，请你设计一个函数，生成所有可能的并且有效的括号组合。',
                '给定一个没有重复数字的序列，返回其所有可能的全排列。',
                '给定一个n×n的二维矩阵表示一个图像，将图像顺时针旋转90度。'
            ]
            
            const problems = []
            for (let i = 1; i <= count; i++) {
                const titleIndex = (i - 1) % mockTitles.length
                const descriptionIndex = (i - 1) % mockDescriptions.length
                const star = Math.floor(Math.random() * 5) + 1 // 1-5星难度
                
                problems.push({
                    id: 1000 + i,
                    testPoint: Math.floor(Math.random() * 10) + 1,
                    title: mockTitles[titleIndex],
                    description: mockDescriptions[descriptionIndex],
                    star: star.toString(),
                    timeLimit: Math.floor(Math.random() * 3) + 1, // 1-3秒
                    memLimit: Math.floor(Math.random() * 256) + 64 // 64-320MB
                })
            }
            
            return problems
        }

        // 搜索功能（占位，实际功能可后续实现）
        const handleSearch = () => {
            console.log('搜索关键词:', searchKeyword.value)
            // 搜索功能已通过computed属性filteredProblems实现
        }

        onMounted(() => {
            getProblemList()
        })

        return {
            problems,
            filteredProblems,
            loading,
            searchKeyword,
            error,
            getDifficultyClass,
            getDifficultyText,
            getProblemList,
            handleSearch,
            handleView
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

/* 查看按钮样式 */
.problem-table-container .btn-outline-primary {
    padding: 0.25rem 0.75rem;
    font-size: 0.85rem;
    border-radius: 20px;
    transition: all 0.2s;
}

.problem-table-container .btn-outline-primary:hover {
    transform: translateY(-2px);
    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
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
}
</style>