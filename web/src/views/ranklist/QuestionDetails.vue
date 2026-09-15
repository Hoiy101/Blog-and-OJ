<template>
    <div class="problem-page">
        <!-- 结果提示窗 -->
        <ResultModal 
            :visible="showResultModal" 
            :resultData="websocketResult"
            @close="closeResultModal"
            @viewSolution="goToAnswer"
        />

        <!-- 头部：返回按钮和题目信息 -->
        <div class="problem-toolbar">
            <div class="problem-toolbar-left">
                <button class="btn btn-outline-secondary btn-sm" @click="backToProblemList">
                    <i class="bi bi-arrow-left"></i> 题库
                </button>
                <div class="problem-heading">
                    <span class="tag tag-mono">#{{ problem.id || '…' }}</span>
                    <h1 class="problem-title" :title="problem.title">{{ problem.title || '加载中...' }}</h1>
                    <span v-if="!loading && !error" class="difficulty-badge" :class="getDifficultyClass(problem.star)">
                        {{ getDifficultyText(problem.star) }}
                    </span>
                </div>
            </div>
            <div class="problem-toolbar-right">
                <span class="tag" title="时间限制"><i class="bi bi-stopwatch"></i> 时间限制 {{ problem.timeLimit || 0 }}s</span>
                <span class="tag" title="内存限制"><i class="bi bi-memory"></i> 内存限制 {{ problem.memLimit || 0 }}MB</span>
                <span class="tag" title="测试点"><i class="bi bi-list-check"></i> 测试点 {{ problem.testPoint || 0 }}</span>
                <button class="btn btn-outline-primary btn-sm" @click="viewAnswer">
                    <i class="bi bi-lightbulb"></i> 查看题解
                </button>
            </div>
        </div>

        <div class="problem-workspace">
            <!-- 左侧：题目描述区域 -->
            <section class="workspace-pane problem-pane" aria-label="题目描述">
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
                    <button class="btn btn-primary" @click="backToProblemList">返回题库</button>
                </div>

                <!-- 题目内容 -->
                <div v-else class="problem-statement">
                    <!-- 题目描述 -->
                    <div class="problem-section">
                        <h2 class="section-title"><i class="bi bi-file-text"></i> 题目描述</h2>
                        <div class="section-content" v-html="formatContent(problem.description)"></div>
                    </div>

                    <!-- 输入格式 -->
                    <div class="problem-section">
                        <h2 class="section-title"><i class="bi bi-box-arrow-in-right"></i> 输入格式</h2>
                        <div class="section-content">
                            <p v-if="problem.inputFormat">{{ problem.inputFormat }}</p>
                            <p v-else class="text-muted fst-italic">题目未提供输入格式说明</p>
                        </div>
                    </div>

                    <!-- 输出格式 -->
                    <div class="problem-section">
                        <h2 class="section-title"><i class="bi bi-box-arrow-right"></i> 输出格式</h2>
                        <div class="section-content">
                            <p v-if="problem.outputFormat">{{ problem.outputFormat }}</p>
                            <p v-else class="text-muted fst-italic">题目未提供输出格式说明</p>
                        </div>
                    </div>

                    <!-- 样例输入输出 -->
                    <div class="problem-section">
                        <h2 class="section-title"><i class="bi bi-terminal"></i> 样例</h2>
                        <div class="section-content">
                            <div v-if="problem.sampleInput || problem.sampleOutput" class="sample-grid">
                                <div class="sample-block">
                                    <div class="sample-head">输入</div>
                                    <pre class="sample-code"><code>{{ problem.sampleInput || '无' }}</code></pre>
                                </div>
                                <div class="sample-block">
                                    <div class="sample-head">输出</div>
                                    <pre class="sample-code"><code>{{ problem.sampleOutput || '无' }}</code></pre>
                                </div>
                            </div>
                            <p v-else class="text-muted fst-italic">题目未提供样例</p>
                        </div>
                    </div>

                    <!-- 提示 -->
                    <div v-if="problem.hint" class="problem-section">
                        <h2 class="section-title"><i class="bi bi-info-circle"></i> 提示</h2>
                        <div class="section-content" v-html="formatContent(problem.hint)"></div>
                    </div>
                </div>
            </section>

            <!-- 右侧：代码编辑区域 -->
            <section class="workspace-pane editor-pane" aria-label="代码编辑">
                <!-- 语言选择 -->
                <div class="editor-toolbar">
                    <div class="editor-toolbar-group">
                        <label for="language-select" class="visually-hidden">选择编程语言</label>
                        <select id="language-select" class="form-select form-select-sm language-select" v-model="selectedLanguage">
                            <option value="c">C</option>
                            <option value="cpp">C++</option>
                            <option value="java">Java</option>
                            <option value="python">Python</option>
                            <option value="javascript">JavaScript</option>
                        </select>
                    </div>
                    <!-- 编辑器工具栏 -->
                    <div class="editor-toolbar-group">
                        <button class="btn btn-sm btn-light tool-btn" @click="resetCode" title="重置代码">
                            <i class="bi bi-arrow-counterclockwise"></i> 重置
                        </button>
                        <button class="btn btn-sm btn-light tool-btn" @click="copyCode" title="复制代码">
                            <i class="bi bi-clipboard"></i> 复制
                        </button>
                    </div>
                </div>

                <!-- 代码编辑器容器 -->
                <div class="editor-container">
                    <div ref="editor" class="code-editor"></div>
                </div>

                <div class="editor-footer">
                    <div class="editor-status">
                        <span class="editor-stat">行数 {{ editorLines }}</span>
                        <span class="editor-stat">字符数 {{ editorChars }}</span>
                    </div>
                    <!-- 提交按钮区域 -->
                    <div class="submit-area">
                        <button class="btn btn-primary submit-btn" :disabled="isSubmitting" @click="submitCode">
                            <span v-if="isSubmitting">
                                <span class="spinner-border spinner-border-sm me-2" role="status" aria-hidden="true"></span>
                                提交中...
                            </span>
                            <span v-else>
                                <i class="bi bi-send me-1"></i> 提交代码
                            </span>
                        </button>
                    </div>
                </div>

                <!-- 测试用例区域 -->
                <div v-if="testCases && testCases.length > 0" class="test-cases">
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
            </section>
        </div>
    </div>
</template>

<script>
import { ref, onMounted, onUnmounted, watch } from 'vue'
import { useRouter, useRoute } from 'vue-router'
import $ from 'jquery'
import { useStore } from 'vuex'
import ace from 'ace-builds'
import 'ace-builds/src-noconflict/mode-c_cpp'
import 'ace-builds/src-noconflict/mode-java'
import 'ace-builds/src-noconflict/mode-python'
import 'ace-builds/src-noconflict/mode-javascript'
import 'ace-builds/src-noconflict/theme-monokai'
import 'ace-builds/src-noconflict/ext-language_tools'
import ResultModal from '../../components/ResultModal.vue'
import {
    extractSubmissionError,
    isJudgeResult,
    toJudgeModalResult,
    toSubmissionErrorResult
} from '../../utils/judgeSubmission.mjs'

export default {
    name: 'OJProblemDetail',
    
    components: {
        ResultModal
    },
    
    setup() {
        const router = useRouter()
        const route = useRoute()
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
        const testCases = ref([])
        
        // WebSocket相关状态
        const ws = ref(null)
        const wsConnected = ref(false)
        
        // 结果提示窗状态
        const showResultModal = ref(false)
        const websocketResult = ref({
            user_id: null,
            evaluation_id: null,
            score: null,
            state: '',
            message: ''
        })
        
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
            const problemId = route.params.id
            
            if (!problemId) {
                error.value = '题目不存在或已被删除'
                loading.value = false
                console.log(`获取题目详情，ID: ${problemId}`)
                return
            }
            
            loading.value = true
            error.value = null
            
            console.log(`获取题目详情，ID: ${problemId}`)
            
            // 发送请求获取题目详情
            const headers = {}
            if (store.state.user.token && store.state.user.token.trim().length > 0) {
                headers.Authorization = "Bearer " + store.state.user.token
            }
            
            $.ajax({
                url: "http://127.0.0.1:3000/oj/topic/get/",
                type: "GET",
                headers: headers,
                data: { id: problemId },
                success(resp) {
                    console.log('题目详情API响应:', resp)
                    if (resp && (resp.id || resp.title)) {
                        problem.value = {
                            id: resp.id || problemId,
                            title: resp.title || '无标题',
                            description: resp.description || '暂无题目描述',
                            star: resp.star || '3',
                            timeLimit: resp.time_limit || 1000,
                            memLimit: resp.mem_limit || 256,
                            testPoint: resp.test_point || 0,
                            inputFormat: resp.input_format || '标准输入',
                            outputFormat: resp.output_format || '标准输出',
                            sampleInput: resp.sample_input || '',
                            sampleOutput: resp.sample_output || '',
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

            $.ajax({
                url: "http://127.0.0.1:3000/oj/evaluate/add/",
                type: "POST",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                data: {
                    evaluateId: problem.value.id,
                    language: selectedLanguage.value,
                    code: code
                },
                success(resp) {
                    console.log('提交成功，等待判题结果:', resp)
                },
                error(xhr) {
                    isSubmitting.value = false
                    websocketResult.value = toSubmissionErrorResult(extractSubmissionError(xhr))
                    showResultModal.value = true
                }
            })
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
        
        // 查看题解
        const viewAnswer = () => {
            router.push(`/answer/${problem.value.id}`)
        }
        
        // 关闭结果提示窗
        const closeResultModal = () => {
            showResultModal.value = false
        }
        
        // 跳转到题解页面
        const goToAnswer = (evaluationId) => {
            showResultModal.value = false
            router.push(`/answer/${evaluationId}`)
        }
        
        // 建立WebSocket连接
        const connectWebSocket = () => {
            const userId = store.state.user.id
            if (!userId) {
                console.log('用户未登录，无法建立WebSocket连接')
                return
            }
            
            const wsUrl = `ws://127.0.0.1:3000/websocket/${userId}`
            console.log('尝试建立WebSocket连接:', wsUrl)
            
            ws.value = new WebSocket(wsUrl)
            
            ws.value.onopen = () => {
                console.log('WebSocket连接已建立')
                wsConnected.value = true
            }
            
            ws.value.onmessage = (event) => {
                console.log('收到WebSocket消息:', event.data)
                try {
                    const data = JSON.parse(event.data)
                    if (isJudgeResult(data)) {
                        isSubmitting.value = false
                        websocketResult.value = toJudgeModalResult(data)
                        showResultModal.value = true
                    }
                } catch (error) {
                    console.error('解析WebSocket消息失败:', error)
                }
            }
            
            ws.value.onerror = (error) => {
                console.error('WebSocket连接错误:', error)
                wsConnected.value = false
            }
            
            ws.value.onclose = (event) => {
                console.log('WebSocket连接已关闭:', event.code, event.reason)
                wsConnected.value = false
            }
        }
        
        // 断开WebSocket连接
        const disconnectWebSocket = () => {
            if (ws.value) {
                console.log('断开WebSocket连接')
                ws.value.close()
                ws.value = null
                wsConnected.value = false
            }
        }
        
        // 监听路由参数变化
        watch(() => route.params.id, (newId) => {
            if (newId) {
                getProblemDetail()
            }
        }, { immediate: true })

        onMounted(() => {
            // 延迟初始化编辑器，确保DOM已渲染
            setTimeout(() => {
                initAceEditor()
            }, 100)
            
            // 建立WebSocket连接
            connectWebSocket()
        })
        
        onUnmounted(() => {
            // 销毁编辑器
            if (aceEditor.value) {
                aceEditor.value.destroy()
                aceEditor.value = null
            }
            
            // 断开WebSocket连接
            disconnectWebSocket()
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
            testCases,
            showResultModal,
            websocketResult,
            getDifficultyClass,
            getDifficultyText,
            formatContent,
            resetCode,
            copyCode,
            submitCode,
            getTestCaseBadgeClass,
            backToProblemList,
            viewAnswer,
            closeResultModal,
            goToAnswer
        }
    }
}
</script>

<style scoped>
.problem-page {
    display: flex;
    flex-direction: column;
    gap: 1rem;
    width: min(1440px, 100% - 2rem);
    margin: 0 auto;
    padding: 1.25rem 0 2rem;
    min-height: calc(100vh - var(--app-navbar-height));
}

/* 顶部工具条 */
.problem-toolbar {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    justify-content: space-between;
    gap: 0.75rem 1.5rem;
    padding: 0.85rem 1.25rem;
    background: var(--app-surface);
    border: 1px solid rgba(148, 163, 184, 0.28);
    border-radius: var(--app-radius-lg);
    box-shadow: var(--app-shadow);
}

.problem-toolbar-left,
.problem-toolbar-right {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    gap: 0.6rem;
    min-width: 0;
}

.problem-heading {
    display: flex;
    align-items: center;
    gap: 0.6rem;
    min-width: 0;
}

.problem-title {
    margin: 0;
    font-size: 1.2rem;
    font-weight: 700;
    color: var(--app-text);
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
    max-width: 420px;
}

/* 工作区 */
.problem-workspace {
    display: grid;
    grid-template-columns: minmax(0, 1fr) minmax(0, 1fr);
    gap: 1rem;
    flex: 1;
    min-height: 0;
}

.workspace-pane {
    display: flex;
    flex-direction: column;
    min-height: 0;
    background: var(--app-surface);
    border: 1px solid rgba(148, 163, 184, 0.28);
    border-radius: var(--app-radius-lg);
    box-shadow: var(--app-shadow);
    overflow: hidden;
}

.problem-pane {
    height: calc(100vh - var(--app-navbar-height) - 140px);
    min-height: 520px;
    overflow-y: auto;
}

.problem-statement {
    padding: 1.75rem 2rem 2rem;
}

.problem-section {
    margin-bottom: 2rem;
}

.problem-section:last-child {
    margin-bottom: 0;
}

.section-title {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    margin: 0 0 0.75rem;
    font-size: 1.02rem;
    font-weight: 700;
    color: var(--app-text);
}

.section-title .bi {
    color: var(--app-primary);
}

.section-content {
    font-size: 1rem;
    line-height: 1.8;
    color: #374151;
    overflow-wrap: anywhere;
}

.section-content p {
    margin-bottom: 0;
    white-space: pre-wrap;
}

.sample-grid {
    display: grid;
    grid-template-columns: repeat(2, minmax(0, 1fr));
    gap: 1rem;
}

.sample-block {
    border: 1px solid var(--app-border);
    border-radius: var(--app-radius);
    overflow: hidden;
}

.sample-head {
    padding: 0.45rem 0.85rem;
    background: var(--app-surface-soft);
    border-bottom: 1px solid var(--app-border);
    font-size: 0.82rem;
    font-weight: 600;
    color: var(--app-text-secondary);
}

.sample-code {
    margin: 0;
    padding: 0.85rem 1rem;
    background: #fff;
    font-family: var(--app-font-mono);
    font-size: 0.9rem;
    line-height: 1.6;
    white-space: pre-wrap;
    word-wrap: break-word;
    min-height: 3.5rem;
}

/* 代码编辑区域 */
.editor-pane {
    height: calc(100vh - var(--app-navbar-height) - 140px);
    min-height: 520px;
}

.editor-toolbar {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    justify-content: space-between;
    gap: 0.5rem 1rem;
    padding: 0.6rem 0.85rem;
    border-bottom: 1px solid var(--app-border);
    background: var(--app-surface-soft);
}

.editor-toolbar-group {
    display: flex;
    align-items: center;
    gap: 0.4rem;
}

.language-select {
    width: auto;
    min-width: 130px;
    font-weight: 500;
}

.tool-btn {
    display: inline-flex;
    align-items: center;
    gap: 0.3rem;
}

.editor-container {
    flex: 1;
    min-height: 300px;
    position: relative;
    background: #272822;
}

.code-editor {
    position: absolute;
    top: 0;
    right: 0;
    bottom: 0;
    left: 0;
    font-size: 14px;
}

.editor-footer {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    justify-content: space-between;
    gap: 0.75rem;
    padding: 0.75rem 1rem;
    border-top: 1px solid var(--app-border);
    background: var(--app-surface-soft);
}

.editor-status {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    gap: 0.4rem 1rem;
    font-size: 0.8rem;
    color: var(--app-text-muted);
}

.submit-btn {
    min-width: 140px;
    padding: 0.55rem 1.4rem;
    font-weight: 600;
}

/* 测试用例样式 */
.test-cases {
    padding: 0.75rem 1rem 1rem;
    border-top: 1px solid var(--app-border);
}

.list-group-item {
    border: 1px solid var(--app-border);
    border-radius: var(--app-radius-sm);
    margin-bottom: 0.5rem;
    padding: 0.75rem 1rem;
}

@media (max-width: 1199.98px) {
    .problem-title {
        max-width: 260px;
    }
}

@media (max-width: 991.98px) {
    .problem-workspace {
        grid-template-columns: 1fr;
    }

    .problem-pane,
    .editor-pane {
        height: auto;
        min-height: 0;
    }

    .editor-container {
        min-height: 380px;
    }

    .problem-title {
        max-width: 100%;
        white-space: normal;
    }
}

@media (max-width: 575.98px) {
    .problem-page {
        width: calc(100% - 1rem);
        padding-top: 0.75rem;
    }

    .problem-statement {
        padding: 1.25rem;
    }

    .sample-grid {
        grid-template-columns: 1fr;
    }

    .problem-toolbar-right .tag {
        display: none;
    }

    .submit-area,
    .submit-btn {
        width: 100%;
    }
}
</style>
