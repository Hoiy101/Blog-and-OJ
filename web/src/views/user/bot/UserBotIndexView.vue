<template>
    <div class="container page-shell">
        <div class="row g-4">
            <!-- 左侧：个人信息 -->
            <div class="col-lg-4 col-xl-3">
                <aside class="page-card profile-card">
                    <div class="profile-cover"></div>
                    <div class="profile-body">
                        <div class="profile-avatar">
                            <img :src = "$store.state.user.photo" alt = "" class="user-photo">
                        </div>
                        <h1 class="profile-name">{{ $store.state.user.username }}</h1>
                        <div class="profile-tags">
                            <span class="tag tag-mono">UID {{ $store.state.user.id }}</span>
                        </div>
                        <dl class="profile-stats">
                            <div class="profile-stat">
                                <dt>博客</dt>
                                <dd>{{ bots.length }}</dd>
                            </div>
                            <div class="profile-stat">
                                <dt>提交</dt>
                                <dd>{{ records.length }}</dd>
                            </div>
                        </dl>
                        <button type="button" class="btn btn-outline-primary w-100 avatar-edit-btn" data-bs-toggle="modal" data-bs-target="#avatar-upload-modal" @click="clearSelectedAvatar">
                            <i class="bi bi-image"></i> 修改头像
                        </button>
                    </div>
                </aside>
            </div>

            <!-- 右侧：博客与刷题记录 -->
            <div class="col-lg-8 col-xl-9">
                <div class="page-card">
                    <nav class="space-tabs">
                        <div class="nav nav-tabs" id="nav-tab" role="tablist" >
                            <button class="nav-link active" id="nav-home-tab" data-bs-toggle="tab" data-bs-target="#nav-home" type="button" role="tab" aria-controls="nav-home" aria-selected="true"><i class="bi bi-journal-richtext"></i> 博客</button>
                            <button class="nav-link" id="nav-profile-tab" data-bs-toggle="tab" data-bs-target="#nav-profile" type="button" role="tab" aria-controls="nav-profile" aria-selected="false"><i class="bi bi-clipboard-check"></i> 刷题记录</button>
                            <!-- <button class="nav-link" id="nav-contact-tab" data-bs-toggle="tab" data-bs-target="#nav-contact" type="button" role="tab" aria-controls="nav-contact" aria-selected="false">Contact</button> -->
                        </div>
                    </nav>
                    <div class="tab-content" id="nav-tabContent">
                        <div class="tab-pane fade show active" id="nav-home" role="tabpanel" aria-labelledby="nav-home-tab">
                            <div class="space-section-header">
                                <h2 class="space-section-title">我的博客 <span class="space-count">{{ bots.length }}</span></h2>
                                <button type="button" class="btn btn-primary" data-bs-toggle="modal" data-bs-target="#add-bot-btn">
                                    <i class="bi bi-plus-lg"></i> 创建博客
                                </button>
                            </div>

                            <div v-if="bots.length === 0" class="state-panel">
                                <span class="state-icon"><i class="bi bi-journal-plus"></i></span>
                                <p class="state-title">还没有发布过博客</p>
                                <p class="state-text">记录学习心得、整理解题思路，写下第一篇文章吧。</p>
                            </div>
                            <ul v-else class="blog-manage-list">
                                <li v-for="bot in bots" :key="bot.id" class="blog-manage-item">
                                    <div class="blog-manage-main">
                                        <h5 class="blog-manage-title">{{ bot.title }}</h5>
                                        <p class="blog-manage-desc">{{ bot.description }}</p>
                                        <div class="blog-manage-meta">
                                            <span class="tag tag-mono">#{{ bot.id }}</span>
                                            <span><i class="bi bi-calendar-plus"></i> 创建于 {{ bot.createtime }}</span>
                                            <span><i class="bi bi-pencil"></i> 更新于 {{ bot.modifytime }}</span>
                                        </div>
                                    </div>
                                    <div class="blog-manage-actions">
                                        <button class="btn btn-sm btn-outline-primary" data-bs-toggle="modal" :data-bs-target="'#update-bot-modal-' + bot.id"><i class="bi bi-pencil"></i> 编辑</button>
                                        <button class="btn btn-sm btn-outline-danger" data-bs-toggle="modal" data-bs-target="#romver"><i class="bi bi-trash3"></i> 删除</button>
                                    </div>

                                    <div class="modal fade" :id="'update-bot-modal-' + bot.id" tabindex="-1">
                                        <div class="modal-dialog modal-xl">
                                            <div class="modal-content">
                                                <div class="modal-header">
                                                    <h5 class="modal-title" id="exampleModalLabel"><i class="bi bi-pencil me-2"></i>修改博客</h5>
                                                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                                </div>
                                                <div class="modal-body">
                                                    <div class="mb-3">
                                                        <label for="add-bot-title" class="form-label">标题</label>
                                                        <input v-model="bot.title" type="text" class="form-control" id="add-bot-title" placeholder="请填写bot名称">
                                                    </div>
                                                    <div class="mb-3">
                                                        <label for="add-bot-description" class="form-label">简介</label>
                                                        <textarea v-model="bot.description" class="form-control" id="add-bot-description" placeholder="请填写bot简介" rows="2"></textarea>
                                                    </div>
                                                    <div class="mb-3">
                                                        <label for="add-bot-code" class="form-label">正文</label>
                                                        <MarkdownEditor v-model="bot.content" :blog-id="bot.id" />
                                                    </div>
                                                </div>
                                                <div class="modal-footer">
                                                    <div class="error-message me-auto">{{bot.error_message}}</div>
                                                    <button type="button" class="btn btn-outline-secondary" data-bs-dismiss="modal">取消</button>
                                                    <button type="button" class="btn btn-primary" @click="update_bot(bot)">保存修改</button>
                                                </div>
                                            </div>
                                        </div>
                                    </div>

                                    <div class="modal fade" id="romver" tabindex="-1" aria-labelledby="exampleModalLabel" aria-hidden="true">
                                        <div class="modal-dialog modal-dialog-centered">
                                            <div class="modal-content">
                                                <div class="modal-header">
                                                    <h5 class="modal-title" id="exampleModalLabel"><i class="bi bi-trash3 me-2 text-danger"></i>删除博客</h5>
                                                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                                </div>
                                                <div class="modal-body">
                                                    是否确认删除博客 <strong>{{ bot.title }}</strong>
                                                </div>
                                                <div class="modal-footer">
                                                    <button type="button" class="btn btn-outline-secondary" data-bs-dismiss="modal">取消</button>
                                                    <button type="button" class="btn btn-danger" @click="remove_bot(bot)">确认</button>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </li>
                            </ul>
                        </div>
                        <div class="tab-pane fade" id="nav-profile" role="tabpanel" aria-labelledby="nav-profile-tab">
                            <div class="space-section-header">
                                <h2 class="space-section-title">我的刷题记录 <span class="space-count">{{ records.length }}</span></h2>
                            </div>
                            <div v-if="records.length === 0" class="state-panel">
                                <span class="state-icon"><i class="bi bi-code-slash"></i></span>
                                <p class="state-title">还没有提交记录</p>
                                <p class="state-text">去题库挑一道题，提交代码后这里会展示判题结果。</p>
                            </div>
                            <div v-else class="table-responsive">
                                <table class="table table-hover app-table record-table">
                                    <thead>
                                        <tr>
                                            <th>题号</th>
                                            <th>标题</th>
                                            <th>状态</th>
                                            <th>分数</th>
                                            <th>提交时间</th>
                                        </tr>
                                    </thead>
                                    <tbody>
                                        <tr v-for="record in records" :key="record.id">
                                            <td><span class="tag tag-mono">#{{ record.questionId }}</span></td>
                                            <td class="fw-semibold">{{ record.title }}</td>
                                            <td>
                                                <span
                                                    class="judge-badge"
                                                    :class="record.state === 'Accepted' ? 'judge-accepted' : (record.state === 'Wrong Answer' ? 'judge-wrong_answer' : 'judge-unknown')"
                                                >{{ record.state }}</span>
                                            </td>
                                            <td class="record-score">{{ record.score }}</td>
                                            <td class="text-muted">{{ record.createtime }}</td>
                                        </tr>
                                    </tbody>
                                </table>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <!-- 修改头像弹窗 -->
        <div class="modal fade" id="avatar-upload-modal" tabindex="-1">
            <div class="modal-dialog modal-dialog-centered">
                <div class="modal-content">
                    <div class="modal-header">
                        <h5 class="modal-title"><i class="bi bi-person-circle me-2"></i>修改头像</h5>
                        <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                    </div>
                    <div class="modal-body">
                        <div class="avatar-preview">
                            <img v-if="avatarSrc" :src="avatarSrc" alt="头像预览">
                            <div v-else class="avatar-placeholder">
                                {{ usernameInitial }}
                            </div>
                        </div>

                        <label for="avatar-file" class="form-label">选择新头像</label>
                        <input
                            id="avatar-file"
                            class="form-control"
                            type="file"
                            accept="image/png,image/jpeg,image/webp"
                            @change="selectAvatar"
                        >
                        <p class="avatar-hint">支持 PNG、JPG、WebP，大小不超过 5MB</p>

                        <div v-if="avatarErrorMessage" class="avatar-message error-message">
                            <i class="bi bi-exclamation-circle"></i> {{ avatarErrorMessage }}
                        </div>
                        <div v-if="avatarSuccessMessage" class="avatar-message success-message">
                            <i class="bi bi-check-circle"></i> {{ avatarSuccessMessage }}
                        </div>
                    </div>
                    <div class="modal-footer">
                        <button type="button" class="btn btn-outline-secondary" data-bs-dismiss="modal" :disabled="avatarUploading">取消</button>
                        <button type="button" class="btn btn-primary" :disabled="!avatarFile || avatarUploading" @click="uploadAvatar">
                            {{ avatarUploading ? "上传中..." : "确认修改" }}
                        </button>
                    </div>
                </div>
            </div>
        </div>

        <!-- 创建博客弹窗 -->
        <div class="modal fade" id="add-bot-btn" tabindex="-1">
            <div class="modal-dialog modal-xl">
                <div class="modal-content">
                    <div class="modal-header">
                        <h5 class="modal-title" id="exampleModalLabel"><i class="bi bi-pencil-square me-2"></i>创建博客</h5>
                        <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                    </div>
                    <div class="modal-body">
                        <div class="mb-3">
                            <label for="add-bot-title" class="form-label">名称</label>
                            <input v-model="botadd.title" type="text" class="form-control" id="add-bot-title" placeholder="请填写博客名称">
                        </div>
                        <div class="mb-3">
                            <label for="add-bot-description" class="form-label">简介</label>
                            <textarea v-model="botadd.description" class="form-control" id="add-bot-description" placeholder="请填写博客简介" rows="2"></textarea>
                        </div>
                        <div class="mb-3">
                            <label for="add-bot-code" class="form-label">正文</label>
                            <MarkdownEditor v-model="botadd.content" />
                        </div>
                    </div>
                    <div class="modal-footer">
                        <div class="error-message me-auto">{{botadd.error_message}}</div>
                        <button type="button" class="btn btn-outline-secondary" data-bs-dismiss="modal">取消</button>
                        <button type="button" class="btn btn-primary" @click="add_bot"><i class="bi bi-send"></i> 创建</button>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import { computed, ref , reactive} from 'vue'
import $ from 'jquery'
import { API_BASE } from '@/config.mjs'
import { useStore } from 'vuex';
import { Modal } from 'bootstrap/dist/js/bootstrap';
import MarkdownEditor from '../../../components/MarkdownEditor.vue';

export default{
    components: {
        MarkdownEditor,
    },
    setup(){
        const store = useStore();
        let bots = ref([]);
        let records = ref([]);
        const avatarFile = ref(null);
        const avatarPreviewUrl = ref("");
        const avatarUploading = ref(false);
        const avatarErrorMessage = ref("");
        const avatarSuccessMessage = ref("");

        const botadd = reactive({
            title: "",  
            description: "",
            content: "",
            error_message: "",
        });

        const avatarSrc = computed(() => avatarPreviewUrl.value || store.state.user.photo);
        const usernameInitial = computed(() => {
            const username = store.state.user.username || "U";
            return username.substring(0, 1).toUpperCase();
        });

        const clearAvatarPreviewUrl = () => {
            if(avatarPreviewUrl.value) {
                URL.revokeObjectURL(avatarPreviewUrl.value);
                avatarPreviewUrl.value = "";
            }
        };

        const clearSelectedAvatar = () => {
            avatarFile.value = null;
            avatarErrorMessage.value = "";
            avatarSuccessMessage.value = "";
            clearAvatarPreviewUrl();
            const input = document.getElementById("avatar-file");
            if(input) {
                input.value = "";
            }
        };

        const selectAvatar = (event) => {
            const file = event.target.files[0];
            avatarErrorMessage.value = "";
            avatarSuccessMessage.value = "";
            clearAvatarPreviewUrl();

            if(!file) {
                avatarFile.value = null;
                return;
            }

            const allowedTypes = ["image/png", "image/jpeg", "image/webp"];
            if(!allowedTypes.includes(file.type)) {
                avatarFile.value = null;
                avatarErrorMessage.value = "请选择 PNG、JPG 或 WebP 图片";
                event.target.value = "";
                return;
            }

            if(file.size > 5 * 1024 * 1024) {
                avatarFile.value = null;
                avatarErrorMessage.value = "头像不能超过 5MB";
                event.target.value = "";
                return;
            }

            avatarFile.value = file;
            avatarPreviewUrl.value = URL.createObjectURL(file);
        };

        const uploadAvatar = () => {
            if(!avatarFile.value || avatarUploading.value) {
                return;
            }

            avatarUploading.value = true;
            avatarErrorMessage.value = "";
            avatarSuccessMessage.value = "";

            store.dispatch("uploadAvatar", {
                file: avatarFile.value,
                success(resp) {
                    avatarUploading.value = false;
                    if(resp.photo) {
                        clearAvatarPreviewUrl();
                    }
                    avatarFile.value = null;
                    avatarSuccessMessage.value = "头像修改成功";
                    const input = document.getElementById("avatar-file");
                    if(input) {
                        input.value = "";
                    }
                    const modal = Modal.getInstance(document.getElementById("avatar-upload-modal"));
                    if(modal) {
                        modal.hide();
                    }
                },
                error(resp) {
                    avatarUploading.value = false;
                    avatarErrorMessage.value = resp.responseJSON?.error_message || resp.error_message || "头像上传失败";
                }
            })
        };

        const refresh_bots = () => {
            $.ajax({
                url : `${API_BASE}/user/bot/getlist/`,
                type : "get",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                success(resp){
                    bots.value = resp;
                }
            })
        }
        refresh_bots();
        const getlist_record = () => {
            $.ajax({
                url : `${API_BASE}/oj/record/getlist/`,
                type : "get",
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                success(resp){
                    records.value = resp;
                }
            })
        }
        getlist_record();
        
        const add_bot = () => {
            botadd.error_message = "";
            $.ajax({
                url : `${API_BASE}/user/bot/add/`,
                type : "post",
                data: {
                    title: botadd.title,
                    description: botadd.description,
                    content: botadd.content,
                },
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                success(resp){
                    if(resp.error_message === "success"){
                        botadd.title = "";
                        botadd.description = "";
                        botadd.content = "";
                        Modal.getInstance("#add-bot-btn").hide();
                        refresh_bots();
                    }
                    else{
                        botadd.error_message = resp.error_message;
                    }
                }
            })
        }

        

        const remove_bot = (bot) => {
            $.ajax({
                url : `${API_BASE}/user/bot/remove/`,
                type : "post",
                data: {
                    bot_id: bot.id,
                },
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                success(resp){
                    if(resp.error_message === "success"){
                        refresh_bots();
                        Modal.getInstance("#romver").hide();
                    }
                }
            })
        }

        const update_bot = (bot) => {
            botadd.error_message = "";
            $.ajax({
                url : `${API_BASE}/user/bot/update/`,
                type : "post",
                data: {
                    bot_id: bot.id,
                    title: bot.title,
                    description: bot.description,
                    content: bot.content,
                },
                headers: {
                    Authorization: "Bearer " + store.state.user.token,
                },
                success(resp){
                    if(resp.error_message === "success"){
                        Modal.getInstance('#update-bot-modal-' + bot.id).hide();
                        refresh_bots();
                    }
                    else{
                        botadd.error_message = resp.error_message;
                    }
                }
            })
        }
        
        return{
            bots, 
            botadd,
            add_bot,
            remove_bot,
            update_bot,
            getlist_record,
            records,
            avatarFile,
            avatarSrc,
            avatarUploading,
            avatarErrorMessage,
            avatarSuccessMessage,
            usernameInitial,
            selectAvatar,
            uploadAvatar,
            clearSelectedAvatar,
        }

    }
}
</script>

<style scoped>
/* 个人信息卡 */
.profile-card {
    position: sticky;
    top: calc(var(--app-navbar-height) + 1.75rem);
}

.profile-cover {
    height: 88px;
    background:
        radial-gradient(circle at 20% 30%, rgba(255, 255, 255, 0.45), transparent 45%),
        linear-gradient(135deg, #60a5fa 0%, #2563eb 60%, #1e40af 100%);
}

.profile-body {
    padding: 0 1.5rem 1.5rem;
    text-align: center;
}

.profile-avatar {
    width: 104px;
    height: 104px;
    margin: -52px auto 0;
    border: 4px solid #fff;
    border-radius: 50%;
    background: var(--app-surface-muted);
    box-shadow: var(--app-shadow);
    overflow: hidden;
}

img.user-photo {
    width: 100%;
    height: 100%;
    object-fit: cover;
    display: block;
}

.profile-name {
    margin: 0.85rem 0 0.5rem;
    font-size: 1.3rem;
    font-weight: 700;
    word-break: break-all;
}

.profile-tags {
    display: flex;
    flex-wrap: wrap;
    justify-content: center;
    gap: 0.4rem;
}

.profile-stats {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: 0.5rem;
    margin: 1.5rem 0;
    padding: 0.9rem 0;
    border-top: 1px solid var(--app-border);
    border-bottom: 1px solid var(--app-border);
}

.profile-stat dt {
    order: 2;
    font-size: 0.78rem;
    font-weight: 500;
    color: var(--app-text-muted);
}

.profile-stat dd {
    margin: 0;
    font-size: 1.35rem;
    font-weight: 700;
    color: var(--app-text);
    font-variant-numeric: tabular-nums;
}

/* 标签页 */
.space-tabs {
    padding: 0.6rem 1rem 0;
    border-bottom: 1px solid var(--app-border);
    background: linear-gradient(180deg, #ffffff 0%, #f8fafc 100%);
}

.space-tabs .nav-tabs {
    border-bottom: 0;
    gap: 0.25rem;
}

.space-tabs .nav-link {
    display: inline-flex;
    align-items: center;
    gap: 0.45rem;
    padding: 0.85rem 1rem;
    border: 0;
    border-bottom: 2px solid transparent;
    margin-bottom: -1px;
    border-radius: 0;
    background: transparent;
    color: var(--app-text-secondary);
    font-weight: 600;
    font-size: 0.95rem;
}

.space-tabs .nav-link:hover {
    color: var(--app-text);
}

.space-tabs .nav-link.active {
    color: var(--app-primary);
    border-bottom-color: var(--app-primary);
    background: transparent;
}

.space-section-header {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    justify-content: space-between;
    gap: 0.75rem;
    padding: 1rem 1.5rem;
    border-bottom: 1px solid var(--app-border);
}

.space-section-title {
    display: flex;
    align-items: center;
    gap: 0.5rem;
    margin: 0;
    font-size: 1.05rem;
    font-weight: 700;
}

.space-count {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    min-width: 22px;
    height: 22px;
    padding: 0 0.4rem;
    border-radius: 999px;
    background: var(--app-primary-soft);
    color: var(--app-primary);
    font-size: 0.75rem;
}

/* 博客管理列表 */
.blog-manage-list {
    margin: 0;
    padding: 0;
    list-style: none;
}

.blog-manage-item {
    display: flex;
    align-items: flex-start;
    justify-content: space-between;
    gap: 1.25rem;
    padding: 1.35rem 1.5rem;
    border-bottom: 1px solid var(--app-border);
    transition: background-color 0.15s ease;
}

.blog-manage-item:last-child {
    border-bottom: 0;
}

.blog-manage-item:hover {
    background: var(--app-surface-soft);
}

.blog-manage-main {
    min-width: 0;
    flex: 1;
}

.blog-manage-title {
    margin: 0 0 0.35rem;
    font-size: 1.1rem;
    font-weight: 700;
    color: var(--app-text);
}

.blog-manage-desc {
    margin: 0 0 0.6rem;
    color: var(--app-text-secondary);
    font-size: 0.92rem;
    line-height: 1.6;
    display: -webkit-box;
    -webkit-line-clamp: 2;
    -webkit-box-orient: vertical;
    overflow: hidden;
}

.blog-manage-meta {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    gap: 0.4rem 1rem;
    font-size: 0.8rem;
    color: var(--app-text-muted);
}

.blog-manage-meta i {
    margin-right: 0.2rem;
}

.blog-manage-actions {
    display: flex;
    flex-shrink: 0;
    gap: 0.4rem;
}

/* 刷题记录 */
.record-score {
    font-weight: 700;
    font-variant-numeric: tabular-nums;
}

/* 头像弹窗 */
.avatar-preview {
    width: 128px;
    height: 128px;
    margin: 0 auto 18px;
    border-radius: 50%;
    overflow: hidden;
    background: var(--app-surface-muted);
    border: 4px solid #fff;
    box-shadow: 0 0 0 1px var(--app-border), var(--app-shadow);
}

.avatar-preview img {
    width: 100%;
    height: 100%;
    object-fit: cover;
    display: block;
}

.avatar-placeholder {
    width: 100%;
    height: 100%;
    display: flex;
    align-items: center;
    justify-content: center;
    background: linear-gradient(135deg, #60a5fa, #2563eb);
    color: #fff;
    font-size: 42px;
    font-weight: 600;
}

.avatar-hint {
    margin: 0.4rem 0 0;
    font-size: 0.8rem;
    color: var(--app-text-muted);
}

.avatar-message {
    margin-top: 12px;
    display: flex;
    align-items: center;
    gap: 0.4rem;
    font-size: 0.9rem;
}

div.error-message{
    color: var(--app-danger);
}

div.success-message {
    color: var(--app-success);
}

@media (max-width: 991.98px) {
    .profile-card {
        position: static;
    }
}

@media (max-width: 767.98px) {
    .blog-manage-item {
        flex-direction: column;
        gap: 0.85rem;
        padding: 1.1rem 1.15rem;
    }

    .blog-manage-actions {
        width: 100%;
    }

    .blog-manage-actions .btn {
        flex: 1;
    }
}
</style>
