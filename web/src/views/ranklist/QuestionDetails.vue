<template>
    <div class="container content-field">
        <!-- 题目详情和编辑界面 -->
        <div class="card oj-detail-card">
            <!-- 头部：返回按钮和题目信息 -->
            <div class="card-header border-bottom bg-light">
                <div class="d-flex justify-content-between align-items-center">
                    <button class="btn btn-outline-secondary btn-sm" @click="backToProblemList">
                        <i class="bi bi-arrow-left"></i> 返回题库
                    </button>
                    <h5 class="mb-0 text-center">题目详情</h5>
                    <div>
                        <span class="badge bg-info me-2">ID: {{ problem.id }}</span>
                        <span class="badge difficulty-badge" :class="getDifficultyClass(problem.star)">
                            {{ getDifficultyText(problem.star) }}
                        </span>
                    </div>
                </div>
            </div>

            <div class="card-body p-0">
                <div class="row g-0">
                    <!-- 左侧：题目描述区域 -->
                    <div class="col-md-6 border-end problem-description-area">
                        <div class="p-4 problem-description-content">
                            <!-- 加载状态 -->
                            <div v-if="loading" class="text-center py-5">
                                <div class="loading-spinner"></div>
                                <p class="mt-3 text-muted">加载题目中...</p>
                            </div>

                            <!-- 错误状态 -->
                            <div v-else-if="error" class="text-center py-5">
                                <div class="error-state text-danger">
                                    <i class="bi bi-exclamation-triangle display-4"></i>
                                    <p class="mt-3">加载失败: {{ error }}</p>
                                    <button class="btn btn-primary mt-2" @click="backToProblemList">返回题库</button>
                                </div>
                            </div>

                            <!-- 题目内容 -->
                            <div v-else>
                                <!-- 题目标题 -->
                                <h2 class="problem-title mb-3">{{ problem.title || '加载中...' }}</h2>
                                
                                <!-- 题目元信息 -->
                                <div class="problem-meta mb-4">
                                    <span class="badge bg-light text-dark me-2">
                                        <i class="bi bi-clock"></i> 时间限制: {{ problem.timeLimit || 0 }}s
                                    </span>
                                    <span class="badge bg-light text-dark me-2">
                                        <i class="bi bi-memory"></i> 内存限制: {{ problem.memLimit || 0 }}MB
                                    </span>
                                    <span class="badge bg-light text-dark me-2">
                                        <i class="bi bi-check-circle"></i> 测试点: {{ problem.testPoint || 0 }}
                                    </span>
                                </div>

                                <!-- 题目描述 -->
                                <div class="problem-section">
                                    <h5 class="section-title">题目描述</h5>
                                    <div class="section-content" v-html="formatContent(problem.description)"></div>
                                </div>

                                <!-- 输入格式 -->
                                <div class="problem-section">
                                    <h5 class="section-title">输入格式</h5>
                                    <div class="section-content">
                                        <p v-if="problem.inputFormat">{{ problem.inputFormat }}</p>
                                        <p v-else class="text-muted fst-italic">题目未提供输入格式说明</p>
                                    </div>
                                </div>

                                <!-- 输出格式 -->
                                <div class="problem-section">
                                    <h5 class="section-title">输出格式</h5>
                                    <div class="section-content">
                                        <p v-if="problem.outputFormat">{{ problem.outputFormat }}</p>
                                        <p v-else class="text-muted fst-italic">题目未提供输出格式说明</p>
                                    </div>
                                </div>

                                <!-- 样例输入输出 -->
                                <div class="problem-section">
                                    <h5 class="section-title">样例</h5>
                                    <div class="section-content">
                                        <div v-if="problem.sampleInput || problem.sampleOutput" class="row g-3">
                                            <div class="col-md-6">
                                                <h6 class="sample-title">输入</h6>
                                                <pre class="sample-code"><code>{{ problem.sampleInput || '无' }}</code></pre>
                                            </div>
                                            <div class="col-md-6">
                                                <h6 class="sample-title">输出</h6>
                                                <pre class="sample-code"><code>{{ problem.sampleOutput || '无' }}</code></pre>
                                            </div>
                                        </div>
                                        <p v-else class="text-muted fst-italic">题目未提供样例</p>
                                    </div>
                                </div>

                                <!-- 提示 -->
                                <div v-if="problem.hint" class="problem-section">
                                    <h5 class="section-title">提示</h5>
                                    <div class="section-content" v-html="formatContent(problem.hint)"></div>
                                </div>
                            </div>
                        </div>
                    </div>

                    <!-- 右侧：代码编辑区域 -->
                    <div class="col-md-6 code-editor-area">
                        <div class="p-4 h-100 d-flex flex-column">
                            <!-- 语言选择 -->
                            <div class="mb-3">
                                <label for="language-select" class="form-label">选择编程语言</label>
                                <select id="language-select" class="form-select" v-model="selectedLanguage">
                                    <option value="c">C</option>
                                    <option value="cpp">C++</option>
                                    <option value="java">Java</option>
                                    <option value="python">Python</option>
                                    <option value="javascript">JavaScript</option>
                                </select>
                            </div>

                            <!-- 代码编辑器容器 -->
                            <div class="editor-container flex-grow-1 mb-3 border rounded">
                                <div ref="editor" class="code-editor"></div>
                            </div>

                            <!-- 编辑器工具栏 -->
                            <div class="editor-toolbar mb-3 d-flex justify-content-between align-items-center">
                                <div>
                                    <button class="btn btn-sm btn-outline-secondary me-2" @click="resetCode">
                                        <i class="bi bi-arrow-clockwise"></i> 重置代码
                                    </button>
                                    <button class="btn btn-sm btn-outline-secondary" @click="copyCode">
                                        <i class="bi bi-clipboard"></i> 复制代码
                                    </button>
                                </div>
                                <div>
                                    <span class="text-muted small me-3">行数: {{ editorLines }}</span>
                                    <span class="text-muted small">字符数: {{ editorChars }}</span>
                                </div>
                            </div>

                            <!-- 提交按钮区域 -->
                            <div class="submit-area">
                                <div class="d-grid">
                                    <button class="btn btn-primary btn-lg" :disabled="isSubmitting" @click="submitCode">
                                        <span v-if="isSubmitting">
                                            <span class="spinner-border spinner-border-sm me-2" role="status" aria-hidden="true"></span>
                                            提交中...
                                        </span>
                                        <span v-else>
                                            <i class="bi bi-send me-2"></i> 提交代码
                                        </span>
                                    </button>
                                </div>
                                
                                <!-- 提交结果 -->
                                <div v-if="submissionResult" class="mt-3">
                                    <div class="alert" :class="getResultAlertClass(submissionResult.status)">
                                        <h6 class="alert-heading">提交结果</h6>
                                        <p class="mb-1">状态: <strong>{{ submissionResult.message || '未知' }}</strong></p>
                                        <p v-if="submissionResult.time" class="mb-1 small">运行时间: {{ submissionResult.time }}ms</p>
                                        <p v-if="submissionResult.memory" class="mb-1 small">内存使用: {{ submissionResult.memory }}KB</p>
                                        <p v-if="submissionResult.passRate" class="mb-0 small">通过率: {{ submissionResult.passRate }}</p>
                                    </div>
                                </div>

                                <!-- 测试用例区域 -->
                                <div v-if="testCases && testCases.length > 0" class="mt-3">
                                    <h6 class="mb-2">测试用例</h6>
                                    <div class="list-group">
                                        <div v-for="(testCase, index) in testCases" :key="index" 
                                             class="list-group-item list-group-item-action">
                                            <div class="d-flex justify-content-between align-items-center">
                                                <div>
                                                    <span class="badge bg-light text-dark me-2">用例 {{ index + 1 }}</span>
                                                    <span class="small">{{ testCase.input ? testCase.input.substring(0, 30) + '...' : '无输入' }}</span>
                                                </div>
                                                <span class="badge" :class="getTestCaseBadgeClass(testCase.status)">
                                                    {{ testCase.status || '未测试' }}
                                                </span>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import { ref, onMounted, onUnmounted, watch } from 'vue'
import { useRouter } from 'vue-router'
import $ from 'jquery'
import { useStore } from 'vuex'
import ace from 'ace-builds'
import 'ace-builds/src-noconflict/mode-c_cpp'
import 'ace-builds/src-noconflict/mode-java'
import 'ace-builds/src-noconflict/mode-python'
import 'ace-builds/src-noconflict/mode-javascript'
import 'ace-builds/src-noconflict/theme-monokai'
import 'ace-builds/src-noconflict/ext-language_tools'

export default {
    name: 'OJProblemDetail',
    
    setup() {
        const router = useRouter()
        const store = useStore()
        
        // 题目相关状态
        const problem = ref({
            id: null,
            title: '',
            description: '',
            star: '',
            timeLimit: 0,
            memLimit: 0,
            testPoint: 0,
            inputFormat: '',
            outputFormat: '',
            sampleInput: '',
            sampleOutput: '',
            hint: ''
        })
        
        const loading = ref(false)
        const error = ref(null)
        
        // 编辑器相关状态
        const editor = ref(null)
        const aceEditor = ref(null)
        const selectedLanguage = ref('cpp')
        const editorLines = ref(0)
        const editorChars = ref(0)
        
        // 提交相关状态
        const isSubmitting = ref(false)
        const submissionResult = ref(null)
        const testCases = ref([])
        
        // 默认代码模板
        const codeTemplates = {
            c: `#include <stdio.h>\n\nint main() {\n    // 在这里编写你的代码\n    int n;\n    scanf("%d", &n);\n    printf("%d\\n", n);\n    return 0;\n}`,
            cpp: `#include <iostream>\nusing namespace std;\n\nint main() {\n    // 在这里编写你的代码\n    int n;\n    cin >> n;\n    cout << n << endl;\n    return 0;\n}`,
            java: `import java.util.Scanner;\n\npublic class Main {\n    public static void main(String[] args) {\n        Scanner scanner = new Scanner(System.in);\n        // 在这里编写你的代码\n        int n = scanner.nextInt();\n        System.out.println(n);\n    }\n}`,
            python: `# 在这里编写你的代码\nn = int(input())\nprint(n)`,
            javascript: `// 在这里编写你的代码\nconst readline = require('readline');\nconst rl = readline.createInterface({\n    input: process.stdin,\n    output: process.stdout\n});\n\nrl.on('line', (line) => {\n    const n = parseInt(line);\n    console.log(n);\n    rl.close();\n});`
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
        
        // 格式化内容
        const formatContent = (content) => {
            if (!content) return '<p class="text-muted fst-italic">暂无内容</p>'
            
            // 将换行符转换为HTML换行
            let formattedContent = content
                .replace(/&/g, '&amp;')
                .replace(/</g, '&lt;')
                .replace(/>/g, '&gt;')
                .replace(/"/g, '&quot;')
                .replace(/'/g, '&#039;')
                .replace(/\n/g, '<br>')
            
            return formattedContent
        }
        
        // 获取题目详情
        const getProblemDetail = () => {
            const problemId = store.state.topic.topic_id
            
            if (problemId === "-1") {
                error.value = '题目不存在或已被删除'
                console.log(`获取题目详情，ID: ${problemId}`)
                return
            }
            
            loading.value = true
            error.value = null
            
            console.log(`获取题目详情，ID: ${problemId}`)
            
            // 发送请求获取题目详情
            $.ajax({
                url: "http://127.0.0.1:3000/oj/topic/get/",
                type: "GET",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                data: { id: problemId },
                success(resp) {
                    console.log('题目详情API响应:', resp)
                    if (resp && (resp.id || resp.title)) {
                        // 根据后端返回的map格式赋值
                        problem.value = {
                            id: resp.id || problemId,
                            title: resp.title || '无标题',
                            description: resp.description || '暂无题目描述',
                            star: resp.star || '3',
                            timeLimit: resp.timeLimit || 1000,
                            memLimit: resp.memLimit || 256,
                            testPoint: resp.testPoint || 0,
                            inputFormat: resp.inputFormat || '标准输入',
                            outputFormat: resp.outputFormat || '标准输出',
                            sampleInput: resp.sampleInput || '',
                            sampleOutput: resp.sampleOutput || '',
                            hint: resp.hint || ''
                        }
                        
                        console.log('题目详情加载成功:', problem.value.title)
                        
                        // 加载成功后初始化代码编辑器
                        if (aceEditor.value) {
                            aceEditor.value.setValue(codeTemplates[selectedLanguage.value], 1)
                            updateEditorStats()
                        }
                    } else {
                        error.value = '获取的题目数据格式不正确'
                        console.error('题目数据格式错误:', resp)
                    }
                },
                error(jqXHR, textStatus, errorThrown) {
                    console.error('获取题目详情失败:', jqXHR.status, textStatus, errorThrown)
                    
                    let errorMsg = '网络请求失败'
                    if (jqXHR.status === 0) {
                        errorMsg = '无法连接到服务器'
                    } else if (jqXHR.status === 401) {
                        errorMsg = '登录已过期'
                    } else if (jqXHR.status === 404) {
                        errorMsg = '题目不存在'
                    } else {
                        errorMsg = `请求失败: ${jqXHR.status} ${errorThrown}`
                    }
                    
                    error.value = errorMsg
                    
                    error.value = null
                },
                complete() {
                    loading.value = false
                }
            })
        }
        
        // 初始化Ace编辑器
        const initAceEditor = () => {
            if (!editor.value) return
            
            // 创建编辑器实例
            aceEditor.value = ace.edit(editor.value, {
                mode: 'ace/mode/c_cpp',
                theme: 'ace/theme/monokai',
                fontSize: 14,
                showPrintMargin: false,
                enableBasicAutocompletion: true,
                enableLiveAutocompletion: true,
                enableSnippets: true
            })
            
            // 设置默认代码
            aceEditor.value.setValue(codeTemplates.cpp, 1)
            aceEditor.value.clearSelection()
            
            // 监听内容变化
            aceEditor.value.on('change', () => {
                updateEditorStats()
            })
            
            // 监听语言切换
            watch(selectedLanguage, (newLanguage) => {
                const modes = {
                    c: 'c_cpp',
                    cpp: 'c_cpp',
                    java: 'java',
                    python: 'python',
                    javascript: 'javascript'
                }
                
                aceEditor.value.session.setMode(`ace/mode/${modes[newLanguage]}`)
                aceEditor.value.setValue(codeTemplates[newLanguage], 1)
                updateEditorStats()
            })
            
            // 初始化统计
            updateEditorStats()
        }
        
        // 更新编辑器统计
        const updateEditorStats = () => {
            if (!aceEditor.value) return
            
            const value = aceEditor.value.getValue()
            editorLines.value = aceEditor.value.session.getLength()
            editorChars.value = value.length
        }
        
        // 重置代码
        const resetCode = () => {
            if (aceEditor.value) {
                aceEditor.value.setValue(codeTemplates[selectedLanguage.value], 1)
                aceEditor.value.clearSelection()
                updateEditorStats()
            }
        }
        
        // 复制代码
        const copyCode = async () => {
            if (!aceEditor.value) return
            
            const code = aceEditor.value.getValue()
            
            try {
                await navigator.clipboard.writeText(code)
                alert('代码已复制到剪贴板')
            } catch (err) {
                console.error('复制失败:', err)
                alert('复制失败，请手动复制代码')
            }
        }
        
        // 提交代码
        const submitCode = () => {
            if (!aceEditor.value) {
                alert('编辑器未初始化')
                return
            }
            
            const code = aceEditor.value.getValue()
            
            if (!code.trim()) {
                alert('代码不能为空')
                return
            }
            
            console.log('提交代码，语言:', selectedLanguage.value)
            console.log('代码:', code)
            
            isSubmitting.value = true
            submissionResult.value = null
            
            // 实际提交代码到后端的示例
            /*
            $.ajax({
                url: "http://127.0.0.1:3000/oj/submit/",
                type: "POST",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                data: {
                    problemId: problem.value.id,
                    language: selectedLanguage.value,
                    code: code
                },
                success(resp) {
                    console.log('提交成功:', resp)
                    submissionResult.value = resp
                },
                error(jqXHR, textStatus, errorThrown) {
                    console.error('提交失败:', errorThrown)
                    alert('提交失败: ' + errorThrown)
                },
                complete() {
                    isSubmitting.value = false
                }
            })
            */
        }
        
        // 获取结果提示框的类
        const getResultAlertClass = (status) => {
            const classes = {
                accepted: 'alert-success',
                wrong_answer: 'alert-warning',
                time_limit: 'alert-danger',
                runtime_error: 'alert-danger',
                default: 'alert-info'
            }
            
            return classes[status] || classes.default
        }
        
        // 获取测试用例标签的类
        const getTestCaseBadgeClass = (status) => {
            if (status === '通过') return 'bg-success'
            if (status === '失败') return 'bg-danger'
            return 'bg-secondary'
        }
        
        // 返回题库列表
        const backToProblemList = () => {
            router.push('/ranklist/')
        }
        
        onMounted(() => {
            getProblemDetail()
            
            // 延迟初始化编辑器，确保DOM已渲染
            setTimeout(() => {
                initAceEditor()
            }, 100)
        })
        
        onUnmounted(() => {
            // 销毁编辑器
            if (aceEditor.value) {
                aceEditor.value.destroy()
                aceEditor.value = null
            }
        })
        
        return {
            problem,
            loading,
            error,
            editor,
            selectedLanguage,
            editorLines,
            editorChars,
            isSubmitting,
            submissionResult,
            testCases,
            getDifficultyClass,
            getDifficultyText,
            formatContent,
            resetCode,
            copyCode,
            submitCode,
            getResultAlertClass,
            getTestCaseBadgeClass,
            backToProblemList
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
.oj-detail-card {
    border: 1px solid #e0e0e0;
    border-radius: 12px;
    overflow: hidden;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.05);
    background-color: #fff;
    min-height: 600px;
}

/* 卡片头部样式 */
.oj-detail-card .card-header {
    background: linear-gradient(135deg, #f8f9fa 0%, #e9ecef 100%);
    padding: 1rem 1.5rem;
    border-bottom: 2px solid #dee2e6;
}

/* 题目描述区域 */
.problem-description-area {
    height: calc(100vh - 250px);
    overflow-y: auto;
}

.problem-description-content {
    height: 100%;
}

/* 题目标题 */
.problem-title {
    font-size: 1.8rem;
    font-weight: 700;
    color: #2c3e50;
    padding-bottom: 0.5rem;
    border-bottom: 2px solid #f0f0f0;
}

/* 题目元信息 */
.problem-meta .badge {
    font-size: 0.85rem;
    padding: 0.4em 0.8em;
    border: 1px solid #dee2e6;
}

/* 题目章节 */
.problem-section {
    margin-bottom: 2rem;
}

.problem-section:last-child {
    margin-bottom: 0;
}

.section-title {
    font-size: 1.2rem;
    font-weight: 600;
    color: #2c757d;
    margin-bottom: 1rem;
    padding-bottom: 0.5rem;
    border-bottom: 1px solid #e9ecef;
}

.section-content {
    font-size: 1rem;
    line-height: 1.6;
    color: #333;
}

.section-content p {
    margin-bottom: 1rem;
}

/* 样例代码 */
.sample-title {
    font-size: 0.9rem;
    font-weight: 600;
    color: #6c757d;
    margin-bottom: 0.5rem;
}

.sample-code {
    background-color: #f8f9fa;
    border: 1px solid #e9ecef;
    border-radius: 0.5rem;
    padding: 1rem;
    margin: 0;
    overflow-x: auto;
    font-family: 'Courier New', Courier, monospace;
    font-size: 0.9rem;
    line-height: 1.4;
    white-space: pre-wrap;
    word-wrap: break-word;
}

/* 代码编辑区域 */
.code-editor-area {
    height: calc(100vh - 250px);
    display: flex;
    flex-direction: column;
}

.editor-container {
    min-height: 300px;
    overflow: hidden;
    position: relative;
}

.code-editor {
    position: absolute;
    top: 0;
    right: 0;
    bottom: 0;
    left: 0;
    font-size: 14px;
}

/* 编辑器工具栏 */
.editor-toolbar {
    padding: 0.5rem 0;
    border-top: 1px solid #e9ecef;
    border-bottom: 1px solid #e9ecef;
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

/* 提交结果样式 */
.alert {
    padding: 1rem;
    border-radius: 0.5rem;
    border: 1px solid transparent;
}

.alert-success {
    background-color: rgba(40, 167, 69, 0.1);
    border-color: rgba(40, 167, 69, 0.3);
    color: #155724;
}

.alert-warning {
    background-color: rgba(255, 193, 7, 0.1);
    border-color: rgba(255, 193, 7, 0.3);
    color: #856404;
}

.alert-danger {
    background-color: rgba(220, 53, 69, 0.1);
    border-color: rgba(220, 53, 69, 0.3);
    color: #721c24;
}

.alert-info {
    background-color: rgba(23, 162, 184, 0.1);
    border-color: rgba(23, 162, 184, 0.3);
    color: #0c5460;
}

/* 测试用例样式 */
.list-group-item {
    border: 1px solid #e9ecef;
    border-radius: 0.5rem;
    margin-bottom: 0.5rem;
    padding: 0.75rem 1rem;
    transition: all 0.2s;
}

.list-group-item:hover {
    background-color: #f8f9fa;
    transform: translateX(2px);
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

/* 响应式调整 */
@media (max-width: 768px) {
    .content-field {
        margin-top: 1rem;
        padding: 0.5rem;
    }
    
    .oj-detail-card .card-header {
        padding: 0.75rem;
    }
    
    .problem-description-area,
    .code-editor-area {
        height: auto;
        border: none !important;
    }
    
    .problem-title {
        font-size: 1.5rem;
    }
    
    .editor-container {
        min-height: 250px;
    }
    
    .problem-meta .badge {
        margin-bottom: 0.5rem;
    }
}
</style>