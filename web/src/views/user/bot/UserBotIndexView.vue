<template>
    <div class="contaier">
        <div class="row">
            <div class="col-3">
                <div class="card" style="margin-top: 20px; margin-left: 40px;">
                    <div class="card-body">
                        <img :src = "$store.state.user.photo" alt = "" class="user-photo">
                        <button type="button" class="btn btn-outline-primary w-100 avatar-edit-btn" data-bs-toggle="modal" data-bs-target="#avatar-upload-modal" @click="clearSelectedAvatar">
                            修改头像
                        </button>

                        <div class="modal fade" id="avatar-upload-modal" tabindex="-1">
                            <div class="modal-dialog">
                                <div class="modal-content">
                                    <div class="modal-header">
                                        <h5 class="modal-title">修改头像</h5>
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

                                        <div v-if="avatarErrorMessage" class="avatar-message error-message">
                                            {{ avatarErrorMessage }}
                                        </div>
                                        <div v-if="avatarSuccessMessage" class="avatar-message success-message">
                                            {{ avatarSuccessMessage }}
                                        </div>
                                    </div>
                                    <div class="modal-footer">
                                        <button type="button" class="btn btn-secondary" data-bs-dismiss="modal" :disabled="avatarUploading">取消</button>
                                        <button type="button" class="btn btn-primary" :disabled="!avatarFile || avatarUploading" @click="uploadAvatar">
                                            {{ avatarUploading ? "上传中..." : "确认修改" }}
                                        </button>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
            <div class="col-9">
                <nav class="custom-tabs-nav" style="margin-top: 20px; background-color: white; border-radius: 8px 8px 0 0;">
                    <div class="nav nav-tabs" id="nav-tab" role="tablist" >
                        <button class="nav-link active" id="nav-home-tab" data-bs-toggle="tab" data-bs-target="#nav-home" type="button" role="tab" aria-controls="nav-home" aria-selected="true">博客</button>
                        <button class="nav-link" id="nav-profile-tab" data-bs-toggle="tab" data-bs-target="#nav-profile" type="button" role="tab" aria-controls="nav-profile" aria-selected="false">刷题记录</button>
                        <!-- <button class="nav-link" id="nav-contact-tab" data-bs-toggle="tab" data-bs-target="#nav-contact" type="button" role="tab" aria-controls="nav-contact" aria-selected="false">Contact</button> -->
                    </div>
                </nav>
                <div class="tab-content" id="nav-tabContent">
                <div class="tab-pane fade show active" id="nav-home" role="tabpanel" aria-labelledby="nav-home-tab">
                <div class="card">
                    <div class="card-header">
                        <span style="font-size: 130%;">我的博客</span>
                        <button type="button" class="btn btn-primary float-end" data-bs-toggle="modal" data-bs-target="#add-bot-btn">
                            创建博客
                        </button>
                            <div class="modal fade" id="add-bot-btn" tabindex="-1">
                            <div class="modal-dialog modal-xl">
                                <div class="modal-content">
                                <div class="modal-header">
                                    <h5 class="modal-title" id="exampleModalLabel">创建博客</h5>
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
                                        <VAceEditor
                                                v-model:value="botadd.content"
                                                @init="editorInit"
                                                lang="c_cpp"
                                                theme="textmate"
                                                :options="{
                                                    fontSize: '16px'
                                                }"
                                                style="height: 350px"/>
                                    </div>
                                </div>
                                <div class="modal-footer">
                                    <div class="error-message">{{botadd.error_message}}</div>
                                    <button type="button" class="btn btn-primary btn-lg" @click="add_bot">创建</button>
                                    <button type="button" class="btn btn-secondary btn-lg" data-bs-dismiss="modal">取消</button>
                                </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="card-body">
                        <table class="table table-striped table-hover">
                            <tbody>
                                <tr v-for="bot in bots" :key="bot.id">
                                    <td>
                                    <h5 class="card-title">{{ bot.title }}</h5>
                                        <h6 class="card-subtitle mb-2 text-muted">
                                            <small>创建于: {{ bot.createtime }} | 更新于: {{ bot.modifytime }}</small>
                                        </h6>
                                        <p class="card-text">{{ bot.description }}</p>
                                    </td>
                                    <td>
                                        <button class="btn btn-sm btn btn-primary btn-lg" style="margin-right: 10px;" data-bs-toggle="modal" :data-bs-target="'#update-bot-modal-' + bot.id">编辑</button>

                                            <div class="modal fade" :id="'update-bot-modal-' + bot.id" tabindex="-1">
                                            <div class="modal-dialog modal-xl">
                                                <div class="modal-content">
                                                <div class="modal-header">
                                                    <h5 class="modal-title" id="exampleModalLabel">修改博客</h5>
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
                                                        <VAceEditor
                                                            v-model:value="bot.content"
                                                            @init="editorInit"
                                                            lang="c_cpp"
                                                            theme="textmate"
                                                            :options="{
                                                                fontSize: '16px'
                                                            }"
                                                            style="height: 350px"/>
                                                    </div>
                                                </div>
                                                <div class="modal-footer">
                                                    <div class="error-message">{{bot.error_message}}</div>
                                                    <button type="button" class="btn btn-primary" @click="update_bot(bot)">修改</button>
                                                    <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">取消</button>
                                                </div>
                                                </div>
                                            </div>
                                            </div>

                                        <button class="btn btn-sm btn-danger btn-lg" data-bs-toggle="modal" data-bs-target="#romver">删除</button>

                                        <div class="modal fade" id="romver" tabindex="-1" aria-labelledby="exampleModalLabel" aria-hidden="true">
                                        <div class="modal-dialog">
                                            <div class="modal-content">
                                            <div class="modal-header">
                                                <h5 class="modal-title" id="exampleModalLabel">删除博客</h5>
                                                <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                            </div>
                                            <div class="modal-body">
                                                是否确认删除博客 {{ bot.title }}
                                            </div>
                                            <div class="modal-footer">
                                                <button type="button" class="btn btn-primary" @click="remove_bot(bot)">确认</button>
                                                <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">取消</button>
                                            </div>
                                            </div>
                                        </div>
                                        </div>

                                    </td>
                                </tr>
                            </tbody>
                        </table>
                    </div>
                </div>
                </div>
                <div class="tab-pane fade" id="nav-profile" role="tabpanel" aria-labelledby="nav-profile-tab">
                    <div class="card">
                        <div class="card-header">
                            <span style="font-size: 130%;">我的刷题记录</span>
                        </div>
                        <div class="card-body">
                            <table class="table table-striped table-hover">
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
                                        <td>{{ record.questionId }}</td>
                                        <td>{{ record.title }}</td>
                                        <td>{{ record.state }}</td>
                                        <td>{{ record.score }}</td>
                                        <td>{{ record.createtime }}</td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>
                </div>
             </div>
            </div>
        </div>
    </div>
</template>

<script>
import { computed, ref , reactive} from 'vue'
import $ from 'jquery'
import { useStore } from 'vuex';
import { Modal } from 'bootstrap/dist/js/bootstrap';
import { VAceEditor } from 'vue3-ace-editor';
import ace from 'ace-builds';

import 'ace-builds/src-noconflict/mode-c_cpp';
import 'ace-builds/src-noconflict/theme-textmate';

import modeCppUrl from 'ace-builds/src-noconflict/mode-c_cpp?url';
import themeTextmateUrl from 'ace-builds/src-noconflict/theme-textmate?url';

// 在顶层执行配置
ace.config.set(
  "basePath", 
  "https://cdn.jsdelivr.net/npm/ace-builds@" + require('ace-builds').version + "/src-noconflict/"
);
ace.config.setModuleUrl('ace/mode/c_cpp', modeCppUrl);
ace.config.setModuleUrl('ace/theme/textmate', themeTextmateUrl);

export default{
    components: {
        VAceEditor,
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
                url : "http://127.0.0.1:3000/user/bot/getlist/",
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
                url : "http://127.0.0.1:3000/oj/record/getlist/",
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
                url : "http://127.0.0.1:3000/user/bot/add/",
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
                url : "http://127.0.0.1:3000/user/bot/remove/",
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
                url : "http://127.0.0.1:3000/user/bot/update/",
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
img.user-photo {
    width: 100%;
    aspect-ratio: 1 / 1;
    object-fit: cover;
    border-radius: 8px;
}

.avatar-edit-btn {
    margin-top: 12px;
}

.avatar-preview {
    width: 128px;
    height: 128px;
    margin: 0 auto 18px;
    border: 1px solid #dee2e6;
    border-radius: 8px;
    overflow: hidden;
    background-color: #f8f9fa;
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
    color: #495057;
    font-size: 42px;
    font-weight: 600;
}

.avatar-message {
    margin-top: 12px;
}

div.error-message{
    color: red;
}

div.success-message {
    color: #198754;
}
</style>
