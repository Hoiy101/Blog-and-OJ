import { createRouter, createWebHistory } from 'vue-router'
import PkIndexView from '@/views/pk/PkIndexView.vue'
import RanKlistIndexView from '@/views/ranklist/RanKlistIndexView.vue'
import RecordIndexView from '@/views/record/RecordIndexView.vue'
import UserBotIndexView from '@/views/user/bot/UserBotIndexView.vue'
import NotFound from '@/views/error/NotFound.vue'
import UserSettingsView from '@/views/user/bot/settings/UserSettingsView.vue'
import UserAccountLoginView from '@/views/user/bot/account/UserAccountLoginView.vue'
import UserAccountRegisterView from '@/views/user/bot/account/UserAccountRegisterView.vue'
import store from '@/store/index.js'

const routes = [
  {
    path:"/",
    name:"home",
    component:PkIndexView,
    meta: { requiresAuth: true },
  },
  {
    path:"/pk/",
    name:"pk_index",
    component:PkIndexView,
    meta: { requiresAuth: true },
  },
  {
    path:"/ranklist/",
    name:"RanKlist_index",
    component:RanKlistIndexView,
    meta: { requiresAuth: true },
  },
  {
    path:"/record/",
    name:"record_index",
    component:RecordIndexView,
    meta: { requiresAuth: true },
  },
  {
    path:"/user/bot/",
    name:"userbot_index",
    component:UserBotIndexView,
    meta: { requiresAuth: true },
  },
  {
    path:"/user/settings/",
    name:"settings_index",
    component: UserSettingsView,
    meta: { requiresAuth: true },
  },
    {
    path:"/user/account/login/",
    name:"user_account_login",
    component:UserAccountLoginView,
    meta: { requiresAuth: false },
  },
    {
    path:"/user/account/register/",
    name:"user_account_register",
    component:UserAccountRegisterView,
    meta: { requiresAuth: false },
  },
    {
    path:"/:CatchAll(.*)",
    name:"NotFound_index",
    component:NotFound,
  },
]

const router = createRouter({
  history: createWebHistory(),
  routes 
})

router.beforeEach((to, from, next) => {
  if(to.meta.requiresAuth && !store.state.user.is_login){
    next({name:"user_account_login"})
  }
  else{
    next()
  }
})

export default router
