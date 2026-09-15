<template>
    <ContentField class="auth-shell">
        <div class="auth-brand">
            <span class="auth-brand-mark"><i class="bi bi-person-plus"></i></span>
            <h1 class="auth-title">创建账号</h1>
            <p class="auth-subtitle">注册后即可发布博客并在线提交代码</p>
        </div>
        <form class="auth-form" @submit.prevent="register">
            <div class="mb-3">
                <label for="username" class="form-label">用户名</label>
                <div class="auth-input">
                    <i class="bi bi-person"></i>
                    <input v-model = "username" type="text" class="form-control" id="username" placeholder="请输入用户名" autocomplete="username">
                </div>
            </div>
            <div class="mb-3">
                <label for="password" class="form-label">密码</label>
                <div class="auth-input">
                    <i class="bi bi-lock"></i>
                    <input v-model = "password" type="password" class="form-control" id="password" placeholder="请输入密码" autocomplete="new-password">
                </div>
            </div>
            <div class="mb-3">
                <label for="cofirmedPassword" class="form-label">确认密码</label>
                <div class="auth-input">
                    <i class="bi bi-shield-check"></i>
                    <input v-model = "cofirmedPassword" type="password" class="form-control" id="cofirmedPassword" placeholder="请再次输入密码" autocomplete="new-password">
                </div>
            </div>
            <div class="error-message" v-if="error_message">
                <i class="bi bi-exclamation-circle-fill"></i> {{ error_message }}
            </div>
            <button type="submit" class="btn btn-primary btn-lg auth-submit">注册</button>
            <p class="auth-switch">
                已有账号？
                <router-link :to="{ name: 'user_account_login' }">直接登录</router-link>
            </p>
        </form>
    </ContentField>
</template>

<script>
import ContentField from '@/components/ContentField.vue';
import { ref } from 'vue';
import router from '@/router/index.js';
import $ from 'jquery';

export default{
    components: {
        ContentField
    },
    setup(){
        let username = ref('');
        let password = ref('');
        let cofirmedPassword = ref('');
        let error_message = ref('');
        
        const register = () => {
                error_message.value = '';
                $.ajax({
                url: "http://127.0.0.1:3000/user/account/register/",
                type: "post",
                data: {
                    username: username.value,
                    password: password.value,
                    confirmedPassword: cofirmedPassword.value,
                },
                success(resp) {
                    if(resp.error_message === "success"){
                        router.push({ name: 'user_account_login' });
                    }
                    else{
                        error_message.value = resp.error_message;
                    }
                },
                })
        }

        return {
            username,
            password,
            cofirmedPassword,
            error_message,
            register,
        }
    }
}
</script>

<style scoped>
/* 登录/注册共用样式见 src/assets/styles/theme.css 的 auth 部分 */
</style>
