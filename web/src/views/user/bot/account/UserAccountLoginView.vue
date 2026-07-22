<template>
    <ContentField v-if="!$store.state.user.pulling_info">
        <div class="row justify-content-md-center">
            <div class="col-3">
                <form @submit.prevent="login">
                    <div class="mb-3">
                        <label for="username" class="form-label">用户名</label>
                        <input v-model = "username" type="text" class="form-control" id="username" placeholder="请输入用户名">
                    </div>
                    <div class="mb-3">
                        <label for="password" class="form-label">密码</label>
                        <input v-model = "password" type="password" class="form-control" id="password" placeholder="请输入密码">
                    </div>
                    <div class="error-message">
                        {{ error_message }}
                    </div>
                    <button type="submit" class="btn btn-primary">登录</button>
                </form>
            </div>
        </div>
    </ContentField>
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
button{
    width: 100%;
}
.error-message{
    color: red;
    margin-bottom: 10px;
}
</style>
