<template>
    <div class="container content-field">
        <div class="card oj-card">
            <!-- 卡片头部 -->
            <div class="card-header border-bottom bg-light">
                <div class="row align-items-center">
                    <div class="col-md-4">
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
                    <div class="col-md-4">
                        <button type="button" class="btn btn-primary" style="margin-left: auto; right: 0;" 
                                data-bs-toggle="modal" data-bs-target="#add-bot-btn">
                            添加题目
                        </button>
                        <div class="modal fade" id="add-bot-btn" tabindex="-1">
                            <div class="modal-dialog modal-xl">
                                <div class="modal-content">
                                <div class="modal-header">
                                    <h5 class="modal-title" id="exampleModalLabel">创建题目</h5>
                                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                </div>
                                <div class="modal-body">
                                    <div class="mb-3">
                                        <label for="add-bot-title" class="form-label">测试点数量</label>
                                        <input v-model="topicadd.test_point" type="text" class="form-control" id="add-bot-title" placeholder="请填写测试点数量">
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">题目标题</label>
                                        <textarea v-model="topicadd.title" class="form-control" id="add-bot-description" placeholder="请填写题目标题" rows="2"></textarea>
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">题目简介</label>
                                        <textarea v-model="topicadd.description" class="form-control" id="add-bot-description" placeholder="请填写题目简介" rows="2"></textarea>
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">题目难度</label>
                                        <textarea v-model="topicadd.star" class="form-control" id="add-bot-description" placeholder="难度为1-5星，1为简单，5为困难" rows="2"></textarea>
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">时间限制</label>
                                        <textarea v-model="topicadd.time_limit" class="form-control" id="add-bot-description" placeholder="请填写时间限制" rows="2"></textarea>
                                    </div>
                                   <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">内存限制</label>
                                        <textarea v-model="topicadd.mem_limit" class="form-control" id="add-bot-description" placeholder="请填写内存限制" rows="2"></textarea>
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">输入格式</label>
                                        <textarea v-model="topicadd.input_format" class="form-control" id="add-bot-description" placeholder="请填写输入格式" rows="2"></textarea>
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">输出格式</label>
                                        <textarea v-model="topicadd.output_format" class="form-control" id="add-bot-description" placeholder="请填写输出格式" rows="2"></textarea>
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">样例输入</label>
                                        <textarea v-model="topicadd.sample_input" class="form-control" id="add-bot-description" placeholder="请填写样例输入" rows="2"></textarea>
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">样例输出</label>
                                        <textarea v-model="topicadd.sample_output" class="form-control" id="add-bot-description" placeholder="请填写样例输出" rows="2"></textarea>
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">题目提示</label>
                                        <textarea v-model="topicadd.hint" class="form-control" id="add-bot-description" placeholder="请填写样例输出" rows="2"></textarea>
                                    </div>
                                </div>
                                <div class="modal-footer">
                                    <div class="error-message">{{topicadd.error_message}}</div>
                                    <button type="button" class="btn btn-primary btn-lg" @click="addtopic">创建</button>
                                    <button type="button" class="btn btn-secondary btn-lg" data-bs-dismiss="modal">取消</button>
                                </div>
                                </div>
                        </div>
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
                                <th scope="col" style="width: 100px;" class="text-center">查看</th>
                                <th scope="col" style="width: 100px;" class="text-remove"> 删除</th>
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
                                <td class="text-remove">
                                    <button class="btn btn-sm btn-outline-danger" @click="removetopic(problem.id)">
                                        删除
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
import { ref, onMounted, computed, reactive} from 'vue'
import $ from 'jquery'
import { Modal } from 'bootstrap/dist/js/bootstrap';
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

        const errortopic="";

        const topicadd = reactive({
            test_point: "",
            title: "",  
            description: "",
            star:"",
            time_limit:"",
            mem_limit:"",
            input_format:"",
            output_format:"",
            sample_input:"",
            sample_output:"",
            hint:"",
            error_message: "",
        });

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
            topicadd.error_message = ""
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
                },
                complete() {
                    loading.value = false
                }
            })
        }

        const addtopic = () =>{
            $.ajax({
                url: "http://127.0.0.1:3000/oj/topic/add/",
                type: "post",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                data: {
                    test_point: topicadd.test_point,
                    title: topicadd.title,
                    description: topicadd.description,
                    star:topicadd.star,
                    time_limit:topicadd.time_limit,
                    mem_limit:topicadd.mem_limit,
                    input_format:topicadd.input_format,
                    output_format:topicadd.output_format,
                    sample_input:topicadd.sample_input,
                    sample_output:topicadd.sample_output,
                    hint:topicadd.hint,
                },
                success(resp){
                    if(resp.error_message == "success"){
                      topicadd.test_point = "",
                      topicadd.title = "",
                      topicadd.description = "",
                      topicadd.star = "",
                      topicadd.time_limit = "",
                      topicadd.mem_limit = "",
                      topicadd.input_format = "",
                      topicadd.output_format = "",
                      topicadd.sample_input = "",
                      topicadd.sample_output = "",
                      topicadd.hint = "",
                      Modal.getInstance("#add-bot-btn").hide();  
                      getProblemList()
                    }
                    else{
                        topicadd.error_message = resp.error_message
                    }
                }
            })
        }
        const removetopic = (id) =>{
            $.ajax({
                url: "http://127.0.0.1:3000/oj/topic/remove/",
                type: "post",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                data: {
                    topic_id : id,
                },
                success(resp){
                    if(resp.error_message === "success"){
                        errortopic == "删除成功"
                    }else{
                        errortopic == resp.error_message
                    }
                }
            })
            getProblemList()
        }
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
            handleView,
            topicadd,
            addtopic,
            removetopic,
            errortopic
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
div.error-message{
    color: red;
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