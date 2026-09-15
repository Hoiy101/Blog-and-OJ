<template>
    <div class="container page-shell">
        <div class="page-card">
            <div class="page-card-header">
                <div class="d-flex align-items-center gap-3">
                    <span class="page-title-icon"><i class="bi bi-trophy"></i></span>
                    <div>
                        <h1>排行榜</h1>
                        <p class="page-card-subtitle">按通过题数展示活跃用户</p>
                    </div>
                </div>
            </div>
            <div class="state-panel">
                <span class="state-icon"><i class="bi bi-hourglass-split"></i></span>
                <p class="state-title">排行榜即将上线</p>
                <p class="state-text">该功能还在开发中，先去题库刷几道题，抢占榜首位置吧。</p>
                <router-link class="btn btn-primary" :to="{ name: 'RanKlist_index' }">去刷题</router-link>
            </div>
        </div>
    </div>
</template>

<script>
export default{
    name: 'RecordIndexView'
}
</script>
