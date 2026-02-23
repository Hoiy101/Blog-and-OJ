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
                <div class="card" style="margin-top: 20px;">
                    <div class="card-header">
                        <span style="font-size: 130%;">我的bot</span>
                        <button type="button" class="btn btn-primary float-end" data-bs-toggle="modal" data-bs-target="#add-bot-btn">
                            创建bot
                        </button>
                            <div class="modal fade" id="add-bot-btn" tabindex="-1">
                            <div class="modal-dialog modal-xl">
                                <div class="modal-content">
                                <div class="modal-header">
                                    <h5 class="modal-title" id="exampleModalLabel">创建bot</h5>
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
                                    <button type="button" class="btn btn-primary" @click="add_bot">创建</button>
                                    <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">取消</button>
                                </div>
                                </div>
                            </div>
                        </div>
                    </div>
                    <div class="card-body">
                        <table class="table table-striped table-hover">
                           <thead>
                                <tr>
                                    <th>名称</th>
                                    <th>创建时间</th>
                                    <th>修改时间</th>
                                    <th>操作</th>
                                </tr>
                            </thead>
                            <tbody>
                                <tr v-for="bot in bots" :key="bot.id">
                                    <td>{{ bot.title }}</td>
                                    <td>{{ bot.createtime }}</td>
                                    <td>{{ bot.modifytime }}</td>
                                    <td>
                                        <button class="btn btn-sm btn btn-primary" style="margin-right: 10px;" data-bs-toggle="modal" :data-bs-target="'#update-bot-modal-' + bot.id">编辑</button>

                                            <div class="modal fade" :id="'update-bot-modal-' + bot.id" tabindex="-1">
                                            <div class="modal-dialog modal-xl">
                                                <div class="modal-content">
                                                <div class="modal-header">
                                                    <h5 class="modal-title" id="exampleModalLabel">创建bot</h5>
                                                    <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                                </div>
                                                <div class="modal-body">
                                                    <div class="mb-3">
                                                        <label for="add-bot-title" class="form-label">名称</label>
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

                                        <button class="btn btn-sm btn-danger" data-bs-toggle="modal" data-bs-target="#romver">删除</button>

                                        <div class="modal fade" id="romver" tabindex="-1" aria-labelledby="exampleModalLabel" aria-hidden="true">
                                        <div class="modal-dialog">
                                            <div class="modal-content">
                                            <div class="modal-header">
                                                <h5 class="modal-title" id="exampleModalLabel">删除bot</h5>
                                                <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
                                            </div>
                                            <div class="modal-body">
                                                是否确认删除bot {{ bot.title }}
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
        }

    }
}
</script>

<style scoped>
div.error-message{
    color: red;
}
</style>