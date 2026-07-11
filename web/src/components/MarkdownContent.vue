<template>
  <div ref="contentRoot" class="markdown-content" v-html="html" @click="handleContentClick"></div>
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
.markdown-content img { display: block; width: var(--markdown-image-width, 25%); max-width: 100%; height: auto; margin: 1rem auto; cursor: zoom-in; border-radius: 4px; }
.markdown-content pre { padding: 1rem; overflow: auto; background: #f6f8fa; border-radius: 6px; }
.markdown-content code { background: #f1f3f5; padding: .12rem .3rem; border-radius: 3px; }
.markdown-content pre code { padding: 0; background: transparent; }
.markdown-content blockquote { padding-left: 1rem; color: #6a737d; border-left: 4px solid #dfe2e5; }
.markdown-content table { width: 100%; border-collapse: collapse; margin: 1rem 0; }
.markdown-content th, .markdown-content td { padding: .5rem; border: 1px solid #dfe2e5; }
.markdown-lightbox { position: fixed; inset: 0; z-index: 2000; display: flex; align-items: center; justify-content: center; padding: 5vh 5vw; background: rgba(0,0,0,.82); }
.markdown-lightbox img { max-width: 90vw; max-height: 90vh; width: auto; height: auto; object-fit: contain; }
.markdown-lightbox-close { position: fixed; top: 16px; right: 22px; border: 0; background: transparent; color: white; font-size: 42px; line-height: 1; cursor: pointer; }
</style>
