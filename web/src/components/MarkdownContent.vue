<template>
  <div ref="contentRoot" class="markdown-content" v-bind="$attrs" v-html="html" @click="handleContentClick"></div>
  <Teleport to="body">
    <div v-if="lightboxUrl" class="markdown-lightbox" role="dialog" aria-modal="true" @click.self="closeLightbox">
      <button class="markdown-lightbox-close" type="button" aria-label="关闭大图" @click="closeLightbox">×</button>
      <img :src="lightboxUrl" alt="图片大图" @click.stop>
    </div>
  </Teleport>
</template>

<script>
import { computed, nextTick, onBeforeUnmount, ref, watch } from 'vue'
import { renderMarkdown } from '../utils/markdown.mjs'
import { hydrateMarkdownImages } from '../utils/markdownImages.mjs'
import { lockBodyScroll, unlockBodyScroll } from '../utils/bodyScrollLock.mjs'
import violationImage from '../assets/images/violation-del.png'

export default {
  name: 'MarkdownContent',
  // 根节点包含 Teleport，需要手动把 class 等属性透传给正文容器
  inheritAttrs: false,
  props: { source: { type: String, default: '' } },
  setup(props) {
    const contentRoot = ref(null)
    const lightboxUrl = ref('')
    const lockOwner = Symbol('markdown-lightbox')
    let renderGeneration = 0
    const html = computed(() => renderMarkdown(props.source, { imagePlaceholder: violationImage }))

    const hydrate = async () => {
      const generation = ++renderGeneration
      await nextTick()
      if (generation !== renderGeneration) return
      await hydrateMarkdownImages(contentRoot.value, () => generation === renderGeneration)
    }
    watch(() => props.source, hydrate, { immediate: true })

    const handleContentClick = event => {
      const image = event.target.closest?.('img')
      if (!image || !contentRoot.value?.contains(image)) return
      lightboxUrl.value = image.src
      lockBodyScroll(lockOwner)
    }
    const closeLightbox = () => {
      lightboxUrl.value = ''
      unlockBodyScroll(lockOwner)
    }
    const onKeydown = event => { if (event.key === 'Escape' && lightboxUrl.value) closeLightbox() }
    document.addEventListener('keydown', onKeydown)
    onBeforeUnmount(() => {
      renderGeneration += 1
      document.removeEventListener('keydown', onKeydown)
      unlockBodyScroll(lockOwner)
    })
    return { contentRoot, html, lightboxUrl, handleContentClick, closeLightbox }
  }
}
</script>

<style>
.markdown-content { line-height: 1.75; overflow-wrap: anywhere; }
.markdown-content img { display: block; width: var(--markdown-image-width, 25%); max-width: 100%; height: auto; margin: 1rem auto; cursor: zoom-in; border-radius: 8px; box-shadow: 0 2px 8px rgba(15, 23, 42, .08); }
.markdown-content img[data-image-state="loading"] { opacity: .6; }
.markdown-content pre { padding: 1rem 1.25rem; overflow: auto; background: var(--app-surface-soft, #f6f8fa); border: 1px solid var(--app-border, #e5e7eb); border-radius: 10px; font-family: var(--app-font-mono, monospace); font-size: .9rem; line-height: 1.6; }
.markdown-content code { background: var(--app-surface-muted, #f1f3f5); color: #be185d; padding: .12rem .35rem; border-radius: 4px; font-family: var(--app-font-mono, monospace); font-size: .9em; }
.markdown-content pre code { padding: 0; background: transparent; color: inherit; }
.markdown-content blockquote { margin: 1rem 0; padding: .6rem 1rem; color: var(--app-text-secondary, #4b5563); border-left: 4px solid #93c5fd; background: var(--app-surface-soft, #f8fafc); border-radius: 0 8px 8px 0; }
.markdown-content blockquote p:last-child { margin-bottom: 0; }
.markdown-content table { width: 100%; border-collapse: collapse; margin: 1rem 0; }
.markdown-content th, .markdown-content td { padding: .5rem .75rem; border: 1px solid var(--app-border, #dfe2e5); }
.markdown-content th { background: var(--app-surface-soft, #f8fafc); }
.markdown-content a { color: var(--app-primary, #2563eb); }
.markdown-content hr { margin: 1.5rem 0; border-color: var(--app-border, #e5e7eb); opacity: 1; }
.markdown-lightbox { position: fixed; inset: 0; z-index: 2000; display: flex; align-items: center; justify-content: center; padding: 5vh 5vw; background: rgba(15, 23, 42, .86); cursor: zoom-out; animation: markdown-lightbox-in .15s ease; }
.markdown-lightbox img { max-width: 90vw; max-height: 90vh; width: auto; height: auto; object-fit: contain; border-radius: 8px; box-shadow: 0 30px 80px rgba(0, 0, 0, .5); cursor: default; }
.markdown-lightbox-close { position: fixed; top: 16px; right: 22px; display: inline-flex; align-items: center; justify-content: center; width: 44px; height: 44px; border: 0; border-radius: 50%; background: rgba(255, 255, 255, .12); color: white; font-size: 30px; line-height: 1; cursor: pointer; }
.markdown-lightbox-close:hover { background: rgba(255, 255, 255, .22); }
@keyframes markdown-lightbox-in { from { opacity: 0; } to { opacity: 1; } }
</style>
