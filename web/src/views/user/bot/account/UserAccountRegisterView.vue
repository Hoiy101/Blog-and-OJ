<template>
    <ContentField>
        <div class="row justify-content-md-center">
            <div class="col-3">
                <form @submit.prevent="register">
                    <div class="mb-3">
                        <label for="username" class="form-label">用户名</label>
                        <input v-model = "username" type="text" class="form-control" id="username" placeholder="请输入用户名">
                    </div>
                    <div class="mb-3">
                        <label for="password" class="form-label">密码</label>
                        <input v-model = "password" type="password" class="form-control" id="password" placeholder="请输入密码">
                    </div>
                    <div class="mb-3">
                        <label for="cofirmedPassword" class="form-label">确认密码</label>
                        <input v-model = "cofirmedPassword" type="password" class="form-control" id="cofirmedPassword" placeholder="请再次输入密码">
                    </div>
                    <div class="error-message">
                        {{ error_message }}
                    </div>
                    <button type="submit" class="btn btn-primary">注册</button>
                </form>
            </div>
        </div>
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
button{
    width: 100%;
}
.error-message{
    color: red;
    margin-bottom: 10px;
}
</style>