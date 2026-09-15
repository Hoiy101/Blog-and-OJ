<template>
        <div class="container page-shell">
            <div class="page-card">
                <div class="content-field-body">
                    <slot></slot>
                </div>
            </div>
        </div>
</template>

<script>
export default {
    name: 'ContentField'
}
</script>

<style scoped>
    .content-field-body {
        padding: 1.5rem;
    }
</style>
