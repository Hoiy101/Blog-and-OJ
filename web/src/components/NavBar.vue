<template>
<nav class="navbar navbar-expand-lg app-navbar">
  <div class="container">
    <router-link class="navbar-brand app-brand" :to="{name:'home'}">
      <span class="app-brand-mark"><i class="bi bi-braces-asterisk"></i></span>
      <span>Blog <span class="app-brand-amp">and</span> OJ</span>
    </router-link>
    <button class="navbar-toggler app-nav-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarText" aria-controls="navbarText" aria-expanded="false" aria-label="Toggle navigation">
      <i class="bi bi-list"></i>
    </button>
    <div class="collapse navbar-collapse" id="navbarText">
      <ul class="navbar-nav me-auto mb-2 mb-lg-0 app-nav-links">
        <li class="nav-item">
              <router-link :class="route_name == 'pk_index' ? 'nav-link active' : 'nav-link'" :to="{name:'pk_index'}"><i class="bi bi-journal-richtext"></i> 博客</router-link>
        </li>
        <li class="nav-item">
              <router-link :class="route_name == 'RanKlist_index' ? 'nav-link active' : 'nav-link'" :to="{name:'RanKlist_index'}"><i class="bi bi-code-square"></i> 题库</router-link>
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
            <i class="bi bi-shield-lock"></i> 后台管理
          </a>
          <ul class="dropdown-menu app-dropdown">
            <li><router-link class="dropdown-item" :to="{name:'manage_users'}"><i class="bi bi-people"></i> 用户管理</router-link></li>
            <li><router-link class="dropdown-item" :to="{name:'manage_login_records'}"><i class="bi bi-clock-history"></i> 用户登录信息</router-link></li>
            <li><router-link class="dropdown-item" :to="{name:'manage_topics'}"><i class="bi bi-collection"></i> 题库管理</router-link></li>
          </ul>
        </li>
        <!-- <li class="nav-item">
              <router-link :class="route_name == 'record_index' ? 'nav-link active' : 'nav-link'" :to="{name:'record_index'}">排行榜</router-link>
        </li> -->
      </ul>
        <ul class="navbar-nav app-nav-links" v-if="$store.state.user.is_login">
         <li class="nav-item dropdown">
          <a class="nav-link dropdown-toggle app-nav-user" href="#" role="button" data-bs-toggle="dropdown" aria-expanded="false">
            <img v-if="$store.state.user.photo" :src="$store.state.user.photo" alt="" class="app-nav-avatar">
            <span v-else class="app-nav-avatar app-nav-avatar-fallback">{{ ($store.state.user.username || 'U').substring(0, 1).toUpperCase() }}</span>
            <span class="app-nav-username">{{ $store.state.user.username }}</span>
          </a>
          <ul class="dropdown-menu dropdown-menu-end app-dropdown">
            <li class="app-dropdown-header">
              <strong>{{ $store.state.user.username }}</strong>
              <small>{{ admin ? '管理员' : '普通用户' }}</small>
            </li>
            <li>
                <router-link class="dropdown-item" :to="{name:'userbot_index'}"><i class="bi bi-person-badge"></i> 个人空间</router-link>
            </li>
            <li>
                <router-link class="dropdown-item" :to="{name:'settings_index'}"><i class="bi bi-gear"></i> 设置</router-link>
            </li>
            <li><hr class="dropdown-divider"></li>
            <li>
                <a class="dropdown-item text-danger" href="#" @click="logout"><i class="bi bi-box-arrow-right"></i> 退出</a>
            </li>
          </ul>
        </li>
      </ul>
      <ul class="navbar-nav app-nav-links" v-else-if="!$store.state.user.pulling_info">
         <li class="nav-item">
          <router-link class="nav-link" :to="{name:'user_account_login'}" role="button">
            登录
          </router-link>
        </li>
        <li class="nav-item">
          <router-link class="btn btn-primary btn-sm app-nav-cta" :to="{name:'user_account_register'}" role="button">
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
</script>

<style scoped>
.app-navbar {
  position: sticky;
  top: 0;
  z-index: 1030;
  min-height: var(--app-navbar-height);
  padding-top: 0.5rem;
  padding-bottom: 0.5rem;
  background: rgba(255, 255, 255, 0.84);
  backdrop-filter: blur(14px);
  -webkit-backdrop-filter: blur(14px);
  border-bottom: 1px solid rgba(148, 163, 184, 0.28);
  box-shadow: 0 6px 20px rgba(15, 23, 42, 0.05);
}

.app-brand {
  display: inline-flex;
  align-items: center;
  gap: 0.6rem;
  margin-right: 1.5rem;
  font-weight: 700;
  font-size: 1.1rem;
  color: var(--app-text);
}

.app-brand-mark {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  width: 36px;
  height: 36px;
  border-radius: 10px;
  background: linear-gradient(135deg, #3b82f6 0%, #1d4ed8 100%);
  color: #fff;
  font-size: 1.05rem;
  box-shadow: 0 6px 14px rgba(37, 99, 235, 0.3);
}

.app-brand-amp {
  color: var(--app-text-muted);
  font-weight: 400;
  font-size: 0.9em;
}

.app-nav-toggler {
  width: 40px;
  height: 40px;
  padding: 0;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  border: 1px solid var(--app-border);
  border-radius: 10px;
  color: var(--app-text);
  font-size: 1.3rem;
}

.app-nav-toggler:focus {
  box-shadow: 0 0 0 0.2rem var(--app-primary-soft);
}

.app-nav-links {
  gap: 0.25rem;
}

.app-nav-links .nav-link {
  display: inline-flex;
  align-items: center;
  gap: 0.4rem;
  padding: 0.5rem 0.9rem;
  border-radius: 999px;
  color: var(--app-text-secondary);
  font-weight: 500;
  font-size: 0.95rem;
  transition: background-color 0.15s ease, color 0.15s ease;
}

.app-nav-links .nav-link:hover,
.app-nav-links .nav-link.show {
  background: var(--app-surface-muted);
  color: var(--app-text);
}

.app-nav-links .nav-link.active {
  background: var(--app-primary-soft);
  color: var(--app-primary);
}

.app-nav-links .dropdown-toggle::after {
  margin-left: 0.15rem;
  font-size: 0.8em;
  opacity: 0.7;
}

.app-nav-cta {
  border-radius: 999px;
  padding: 0.45rem 1.1rem;
  margin-left: 0.35rem;
}

.app-nav-user {
  padding: 0.3rem 0.75rem 0.3rem 0.3rem !important;
  border: 1px solid transparent;
}

.app-nav-user:hover,
.app-nav-user.show {
  border-color: var(--app-border);
}

.app-nav-avatar {
  width: 32px;
  height: 32px;
  border-radius: 50%;
  object-fit: cover;
  background: var(--app-surface-muted);
  border: 2px solid #fff;
  box-shadow: 0 0 0 1px var(--app-border);
}

.app-nav-avatar-fallback {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  background: linear-gradient(135deg, #60a5fa, #2563eb);
  color: #fff;
  font-weight: 700;
  font-size: 0.9rem;
}

.app-nav-username {
  max-width: 140px;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
  color: var(--app-text);
}

.app-dropdown {
  min-width: 200px;
  margin-top: 0.5rem !important;
}

.app-dropdown .dropdown-item {
  display: flex;
  align-items: center;
  gap: 0.55rem;
  font-size: 0.92rem;
}

.app-dropdown .dropdown-item.router-link-active {
  color: var(--app-primary);
  background: var(--app-primary-soft);
}

.app-dropdown-header {
  display: flex;
  flex-direction: column;
  padding: 0.5rem 0.75rem 0.6rem;
  color: var(--app-text);
}

.app-dropdown-header small {
  color: var(--app-text-muted);
}

@media (max-width: 991.98px) {
  .navbar-collapse {
    padding: 0.75rem 0 0.5rem;
  }

  .app-nav-links .nav-link {
    border-radius: var(--app-radius-sm);
  }

  .app-nav-links .nav-link.dropdown-toggle {
    width: 100%;
  }

  .app-dropdown {
    border: 0;
    box-shadow: none;
    padding-left: 1.25rem;
    margin-top: 0 !important;
  }

  .app-nav-cta {
    display: inline-flex;
    justify-content: center;
    margin: 0.35rem 0 0;
  }
}
</style>
