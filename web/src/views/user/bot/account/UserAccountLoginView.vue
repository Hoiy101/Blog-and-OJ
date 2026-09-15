<template>
    <ContentField v-if="!$store.state.user.pulling_info" class="auth-shell">
        <div class="auth-brand">
            <span class="auth-brand-mark"><i class="bi bi-braces-asterisk"></i></span>
            <h1 class="auth-title">欢迎回来</h1>
            <p class="auth-subtitle">登录后继续写博客、刷题目</p>
        </div>
        <form class="auth-form" @submit.prevent="login">
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
                    <input v-model = "password" type="password" class="form-control" id="password" placeholder="请输入密码" autocomplete="current-password">
                </div>
            </div>
            <div class="error-message" v-if="error_message">
                <i class="bi bi-exclamation-circle-fill"></i> {{ error_message }}
            </div>
            <button type="submit" class="btn btn-primary btn-lg auth-submit">登录</button>
            <p class="auth-switch">
                还没有账号？
                <router-link :to="{ name: 'user_account_register' }">立即注册</router-link>
            </p>
        </form>
    </ContentField>
    <div v-else class="auth-page">
        <div class="auth-restoring">
            <div class="loading-spinner"></div>
            <p>正在恢复登录状态…</p>
        </div>
    </div>
</template>

<script>
import ContentField from '@/components/ContentField.vue';
import { useStore } from 'vuex';
import { ref } from 'vue';
import { useRoute } from 'vue-router';
import router from '@/router/index.js';

export default{
    components: {
        ContentField
    },
    setup(){
        const store = useStore();
        const route = useRoute();
        let username = ref('');
        let password = ref('');
        let error_message = ref('');

        const goAfterLogin = () => {
            const redirect = typeof route.query.redirect === 'string'
                && route.query.redirect.startsWith('/')
                ? route.query.redirect
                : null;
            router.push(redirect || { name: 'home' });
        };

        const handleGetInfoError = (resp) => {
            store.commit("updatePullingInfo", false);
            error_message.value = resp?.error_message
                || resp?.responseJSON?.error_message
                || "登录状态已失效，请重新登录";
            router.replace({ name: 'user_account_login' });
        };

        const jwt_token = localStorage.getItem("jwt_token");
        if(jwt_token){
            store.commit("updateToken", jwt_token);
            store.dispatch("getinfo", {
                success(){
                    goAfterLogin();
                    store.commit("updatePullingInfo", false);
                },
                error: handleGetInfoError
            });
        }
        else{
            store.commit("updatePullingInfo", false);
        }
        const login = () => {
            error_message.value = '';
            store.dispatch("login", {
                username: username.value,
                password: password.value,
                success(){
                    store.dispatch("getinfo", {
                        success(){
                            goAfterLogin();
                            console.log(store.state.user);
                        },
                        error: handleGetInfoError
                    });
                },
                error(resp){
                    error_message.value = resp?.error_message
                        || resp?.responseJSON?.error_message
                        || "登录失败，请稍后重试";
                }
            })
        }

        return {
            username,
            password,
            error_message,
            login,
        }
    }
}
</script>

<style scoped>
/* 登录/注册共用样式见 src/assets/styles/theme.css 的 auth 部分 */
</style>
