<template>
    <div class="contaier">
        <div class="row">
            <div class="col-3">
                <div class="card" style="margin-top: 20px; margin-left: 40px;">
                    <div class="card-body">
                        <img :src = "$store.state.user.photo" alt = "" style="width: 100%;">
                    </div>
                </div>
            </div>
            <div class="col-9">
                <nav class="custom-tabs-nav" style="margin-top: 20px; background-color: white; border-radius: 8px 8px 0 0;">
                    <div class="nav nav-tabs" id="nav-tab" role="tablist" >
                        <button class="nav-link active" id="nav-home-tab" data-bs-toggle="tab" data-bs-target="#nav-home" type="button" role="tab" aria-controls="nav-home" aria-selected="true">blog</button>
                        <button class="nav-link" id="nav-profile-tab" data-bs-toggle="tab" data-bs-target="#nav-profile" type="button" role="tab" aria-controls="nav-profile" aria-selected="false">刷题记录</button>
                        <button class="nav-link" id="nav-contact-tab" data-bs-toggle="tab" data-bs-target="#nav-contact" type="button" role="tab" aria-controls="nav-contact" aria-selected="false">Contact</button>
                    </div>
                </nav>
                <div class="tab-content" id="nav-tabContent">
                <div class="tab-pane fade show active" id="nav-home" role="tabpanel" aria-labelledby="nav-home-tab">
                <div class="card">
                    <div class="card-header">
                        <span style="font-size: 130%;">我的blog</span>
                        <button type="button" class="btn btn-primary float-end" data-bs-toggle="modal" data-bs-target="#add-bot-btn">
                            创建blog
                        </button>
                            <div class="modal fade" id="add-bot-btn" tabindex="-1">
                            <div class="modal-dialog modal-xl">
                                <div class="modal-content">
                                <div class="modal-header">
                                    <h5 class="modal-title" id="exampleModalLabel">创建blog</h5>
                                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                </div>
                                <div class="modal-body">
                                    <div class="mb-3">
                                        <label for="add-bot-title" class="form-label">名称</label>
                                        <input v-model="botadd.title" type="text" class="form-control" id="add-bot-title" placeholder="请填写bot名称">
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-description" class="form-label">简介</label>
                                        <textarea v-model="botadd.description" class="form-control" id="add-bot-description" placeholder="请填写bot简介" rows="2"></textarea>
                                    </div>
                                    <div class="mb-3">
                                        <label for="add-bot-code" class="form-label">代码</label>
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
                                                    <h5 class="modal-title" id="exampleModalLabel">创建blog</h5>
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
                                                        <label for="add-bot-code" class="form-label">代码</label>
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
                                                <h5 class="modal-title" id="exampleModalLabel">删除blog</h5>
                                                <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                            </div>
                                            <div class="modal-body">
                                                是否确认删除blog {{ bot.title }}
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
import { ref , reactive} from 'vue'
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

        const botadd = reactive({
            title: "",  
            description: "",
            content: "",
            error_message: "",
        });

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
        }

    }
}
</script>

<style scoped>
div.error-message{
    color: red;
}
</style>