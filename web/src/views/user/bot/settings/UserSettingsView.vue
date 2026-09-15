<template>
    <div class="container page-shell settings-page">
        <header class="settings-heading">
            <span class="page-title-icon"><i class="bi bi-gear"></i></span>
            <div>
                <h1>设置</h1>
                <p class="page-card-subtitle">查看当前账号信息</p>
            </div>
        </header>

        <section class="page-card">
            <div class="page-card-header">
                <h2><i class="bi bi-person-badge me-2"></i>账号信息</h2>
            </div>
            <div class="settings-body">
                <div class="account-row">
                    <div class="account-avatar">
                        <img v-if="$store.state.user.photo" :src="$store.state.user.photo" alt="">
                        <span v-else class="account-avatar-fallback">{{ ($store.state.user.username || 'U').substring(0, 1).toUpperCase() }}</span>
                    </div>
                    <div class="account-info">
                        <div class="account-name">{{ $store.state.user.username }}</div>
                        <div class="account-tags">
                            <span class="tag" :class="{ 'tag-primary': $store.state.user.root === 'true' }">
                                <i class="bi" :class="$store.state.user.root === 'true' ? 'bi-shield-check' : 'bi-person'"></i>
                                {{ $store.state.user.root === 'true' ? '管理员' : '普通用户' }}
                            </span>
                            <span class="tag tag-mono">UID {{ $store.state.user.id }}</span>
                        </div>
                    </div>
                    <router-link class="btn btn-outline-primary" :to="{ name: 'userbot_index' }">
                        <i class="bi bi-image"></i> 去个人空间修改头像
                    </router-link>
                </div>
            </div>
        </section>

        <section class="page-card mt-4">
            <div class="state-panel">
                <span class="state-icon"><i class="bi bi-sliders"></i></span>
                <p class="state-title">更多设置正在开发中</p>
                <p class="state-text">修改密码、通知偏好等功能会在后续版本开放。</p>
            </div>
        </section>
    </div>
</template>

<script>
export default{
    name: 'UserSettingsView'
}
</script>

<style scoped>
.settings-page {
    max-width: 880px;
}

.settings-heading {
    display: flex;
    align-items: center;
    gap: 1rem;
    margin-bottom: 1.25rem;
}

.settings-heading h1 {
    margin: 0;
    font-size: 1.5rem;
    font-weight: 700;
}

.settings-body {
    padding: 1.25rem 1.5rem;
}

.account-row {
    display: flex;
    flex-wrap: wrap;
    align-items: center;
    gap: 1.25rem;
}

.account-avatar {
    width: 72px;
    height: 72px;
    border: 3px solid #fff;
    border-radius: 50%;
    background: var(--app-surface-muted);
    box-shadow: 0 0 0 1px var(--app-border);
    overflow: hidden;
    flex-shrink: 0;
}

.account-avatar img {
    width: 100%;
    height: 100%;
    object-fit: cover;
    display: block;
}

.account-avatar-fallback {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 100%;
    height: 100%;
    background: linear-gradient(135deg, #60a5fa, #2563eb);
    color: #fff;
    font-size: 1.7rem;
    font-weight: 700;
}

.account-info {
    flex: 1;
    min-width: 160px;
}

.account-name {
    font-size: 1.15rem;
    font-weight: 700;
    margin-bottom: 0.35rem;
}

.account-tags {
    display: flex;
    flex-wrap: wrap;
    gap: 0.4rem;
}
</style>
