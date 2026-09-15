<template>
    <div class="container page-shell">
        <div class="page-card not-found-card">
            <div class="not-found-code">404</div>
            <h1 class="not-found-title">页面走丢了</h1>
            <p class="not-found-text">你访问的地址不存在，或者已经被移动。检查一下链接，或从下面的入口继续。</p>
            <div class="not-found-actions">
                <router-link class="btn btn-primary" :to="{ name: 'home' }"><i class="bi bi-house"></i> 回到首页</router-link>
                <router-link class="btn btn-outline-secondary" :to="{ name: 'RanKlist_index' }"><i class="bi bi-code-square"></i> 去题库</router-link>
            </div>
        </div>
    </div>
</template>

<script>
export default{
    name: 'NotFoundView'
}
</script>

<style scoped>
.not-found-card {
    max-width: 560px;
    margin: 6vh auto 0;
    padding: 3rem 2rem;
    text-align: center;
}

.not-found-code {
    font-size: 5rem;
    font-weight: 800;
    line-height: 1;
    letter-spacing: -0.04em;
    background: linear-gradient(135deg, #60a5fa, #1d4ed8);
    -webkit-background-clip: text;
    background-clip: text;
    color: transparent;
}

.not-found-title {
    margin: 1rem 0 0.5rem;
    font-size: 1.5rem;
    font-weight: 700;
}

.not-found-text {
    margin: 0 auto 1.75rem;
    max-width: 380px;
    color: var(--app-text-muted);
}

.not-found-actions {
    display: flex;
    flex-wrap: wrap;
    justify-content: center;
    gap: 0.6rem;
}
</style>
