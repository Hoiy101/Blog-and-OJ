import { createStore } from 'vuex'
import ModuleUser from './user'
import ModuleTopic from './topic'

export default createStore({
  state: {
  },
  getters: {
  },
  mutations: {
  },
  actions: {
  },
  modules: {
    user: ModuleUser,
    topic:ModuleTopic,
  }
})
