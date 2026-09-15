<template>
  <footer class="app-footer">
    <div class="container app-footer-inner">
      <span class="app-footer-brand">
        <i class="bi bi-braces-asterisk"></i>
        Blog and OJ
      </span>
      <span class="app-footer-text">写博客，刷题目，实时判题。</span>
    </div>
  </footer>
</template>

<script>
export default {
  name: 'AppFooter'
}
</script>

<style scoped>
.app-footer {
  flex-shrink: 0;
  padding: 1.25rem 0;
  color: var(--app-text-muted);
  font-size: 0.85rem;
}

.app-footer-inner {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  justify-content: space-between;
  gap: 0.5rem;
}

.app-footer-brand {
  display: inline-flex;
  align-items: center;
  gap: 0.4rem;
  font-weight: 600;
  color: var(--app-text-secondary);
}
</style>
