<template>
<nav class="navbar navbar-expand-lg navbar-dark bg-dark">
  <div class="container">
    <router-link class="navbar-brand" :to="{name:'home'}">Blog and Oj</router-link>
    <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarText" aria-controls="navbarText" aria-expanded="false" aria-label="Toggle navigation">
      <span class="navbar-toggler-icon"></span>
    </button>
    <div class="collapse navbar-collapse" id="navbarText">
      <ul class="navbar-nav me-auto mb-4 mb-lg-0">
        <li class="nav-item">
              <router-link :class="route_name == 'pk_index' ? 'nav-link active' : 'nav-link'" :to="{name:'pk_index'}">博客</router-link>
        </li>
        <li class="nav-item">
              <router-link :class="route_name == 'RanKlist_index' ? 'nav-link active' : 'nav-link'" :to="{name:'RanKlist_index'}">题库</router-link>
        </li>
        <li class="nav-item dropdown" v-if="admin">
          <a
            class="nav-link dropdown-toggle"
            :class="route_name && route_name.toString().startsWith('manage_') ? 'active' : ''"
            href="#"
            role="button"
            data-bs-toggle="dropdown"
            aria-expanded="false"
          >
            后台管理
          </a>
          <ul class="dropdown-menu">
            <li><router-link class="dropdown-item" :to="{name:'manage_users'}">用户管理</router-link></li>
            <li><router-link class="dropdown-item" :to="{name:'manage_login_records'}">用户登录信息</router-link></li>
            <li><router-link class="dropdown-item" :to="{name:'manage_topics'}">题库管理</router-link></li>
          </ul>
        </li>
        <!-- <li class="nav-item">
              <router-link :class="route_name == 'record_index' ? 'nav-link active' : 'nav-link'" :to="{name:'record_index'}">排行榜</router-link>
        </li> -->
      </ul>
        <ul class="navbar-nav" v-if="$store.state.user.is_login">
         <li class="nav-item dropdown">
          <a class="nav-link dropdown-toggle" href="#" role="button" data-bs-toggle="dropdown" aria-expanded="false">
            {{ $store.state.user.username }}
          </a>
          <ul class="dropdown-menu">
            <li>
                <router-link class="dropdown-item" :to="{name:'userbot_index'}">个人空间</router-link>
            </li>
            <li>
                <router-link class="dropdown-item" :to="{name:'settings_index'}">设置</router-link>
            </li>
            <li><hr class="dropdown-divider"></li>
            <li>
                <a class="dropdown-item" href="#" @click="logout">退出</a>
            </li>
          </ul>
        </li>
      </ul>
      <ul class="navbar-nav" v-else-if="!$store.state.user.pulling_info">
         <li class="nav-item">
          <router-link class="nav-link" :to="{name:'user_account_login'}" role="button">
            登录
          </router-link>
        </li>
        <li class="nav-item">
          <router-link class="nav-link" :to="{name:'user_account_register'}" role="button">
            注册
          </router-link>
        </li>
      </ul>
    </div>
  </div>
</nav>
</template>

<script>
import { useRoute } from 'vue-router';
import { computed } from 'vue';
import { useStore } from 'vuex';
import { isAdmin } from '@/utils/admin.mjs';

export default{
    setup(){
      const store = useStore();
      const route = useRoute();
      let route_name = computed(() => route.name)
      const admin = computed(() => isAdmin(store.state.user.root))

      const logout = () => {
        store.dispatch("logout");
      }

      return{
        route_name,
        admin,
        logout,
      }
    }
}
</Script>

<style scoped>

</style>
