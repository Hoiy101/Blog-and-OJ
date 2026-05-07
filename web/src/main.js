import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'

// 应用启动时初始化token
const jwt_token = localStorage.getItem("jwt_token");
if (jwt_token) {
  store.commit("user/updateToken", jwt_token);
}

createApp(App).use(store).use(router).mount('#app')
