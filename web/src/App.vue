<template>
  <NavBar />
  <main class="app-main">
    <router-view></router-view>
  </main>
  <AppFooter />
</template>

<script>
import NavBar from './components/NavBar.vue';
import AppFooter from './components/AppFooter.vue';
import "bootstrap/dist/css/bootstrap.min.css";
import "bootstrap/dist/js/bootstrap";
import "bootstrap-icons/font/bootstrap-icons.css";
import "@/assets/styles/theme.css";
export default{
  components: {
    NavBar,
    AppFooter
  },
}
</script>
