<template>
  <div class="markdown-editor">
    <div class="markdown-editor-toolbar">
      <button type="button" class="btn btn-sm btn-outline-secondary" @mousedown.prevent="formatBlock('strong')">粗体</button>
      <button type="button" class="btn btn-sm btn-outline-secondary" @mousedown.prevent="formatBlock('h2')">标题</button>
      <input v-model="linkUrl" class="form-control form-control-sm markdown-link-input" type="url" placeholder="https://链接地址">
      <button type="button" class="btn btn-sm btn-outline-secondary" @mousedown.prevent="createLink">添加链接</button>
      <button type="button" class="btn btn-sm btn-outline-primary" :disabled="!blogId || uploading" @click="$refs.file.click()">
        {{ uploading ? '上传中…' : '上传图片' }}
      </button>
      <input ref="file" hidden type="file" accept="image/png,image/jpeg,image/webp,image/gif" @change="upload">
      <small v-if="!blogId" class="text-muted">先创建博客，再修改文章上传图片</small>
      <span v-if="error" class="text-danger">{{ error }}</span>
    </div>
    <div
      ref="editorRoot"
      class="wysiwyg-editor markdown-content"
      contenteditable="true"
      role="textbox"
      aria-label="博客正文"
      aria-multiline="true"
      data-placeholder="请输入博客正文"
      @input="syncMarkdown"
      @paste="handlePaste"
      @keydown="handleKeydown"
      @focus="saveSelection"
      @keyup="saveSelection"
      @mouseup="saveSelection"
      @compositionstart="composing = true"
      @compositionend="finishComposition"
    ></div>
  </div>
</template>

<script>
import { nextTick, onBeforeUnmount, ref, watch } from 'vue'
import { useStore } from 'vuex'
import { uploadBlogImage } from '../api/blogImages.mjs'
import { renderMarkdown } from '../utils/markdown.mjs'
import { hydrateMarkdownImages, loadImageUrl } from '../utils/markdownImages.mjs'
import { isSafeEditorUrl, sanitizeEditorFragment, serializeEditor } from '../utils/wysiwygMarkdown.mjs'
import violationImage from '../assets/images/violation-del.png'

export default {
  name: 'MarkdownEditor',
  props: {
    modelValue: { type: String, default: '' },
    blogId: { type: [Number, String], default: null }
  },
  emits: ['update:modelValue'],
  setup(props, { emit }) {
    const store = useStore()
    const editorRoot = ref(null)
    const uploading = ref(false)
    const error = ref('')
    const composing = ref(false)
    const linkUrl = ref('https://')
    let emittedMarkdown = null
    let savedRange = null
    let renderGeneration = 0

    const selectionBelongsToEditor = selection => Boolean(
      selection?.rangeCount && editorRoot.value?.contains(selection.anchorNode)
    )

    const saveSelection = () => {
      const selection = window.getSelection()
      if (selectionBelongsToEditor(selection)) savedRange = selection.getRangeAt(0).cloneRange()
    }

    const restoreSelection = range => {
      if (!range || !editorRoot.value?.contains(range.commonAncestorContainer)) return false
      const selection = window.getSelection()
      selection.removeAllRanges()
      selection.addRange(range)
      return true
    }

    const syncMarkdown = () => {
      if (composing.value || !editorRoot.value) return
      const markdown = serializeEditor(editorRoot.value)
      emittedMarkdown = markdown
      emit('update:modelValue', markdown)
      saveSelection()
    }

    const renderExternalMarkdown = async markdown => {
      const generation = ++renderGeneration
      await nextTick()
      if (generation !== renderGeneration || !editorRoot.value) return
      editorRoot.value.innerHTML = renderMarkdown(markdown, { imagePlaceholder: violationImage })
      await hydrateMarkdownImages(editorRoot.value, () => generation === renderGeneration)
    }

    watch(() => props.modelValue, value => {
      if (value === emittedMarkdown) {
        emittedMarkdown = null
        return
      }
      renderExternalMarkdown(value)
    }, { immediate: true })

    const wrapSelection = tagName => {
      const selection = window.getSelection()
      if (!selectionBelongsToEditor(selection)) return
      const range = selection.getRangeAt(0)
      const wrapper = document.createElement(tagName)
      if (range.collapsed) wrapper.appendChild(document.createTextNode(tagName === 'h2' ? '标题' : '文本'))
      else wrapper.appendChild(range.extractContents())
      range.insertNode(wrapper)
      const next = document.createRange()
      next.selectNodeContents(wrapper)
      next.collapse(false)
      selection.removeAllRanges()
      selection.addRange(next)
      syncMarkdown()
    }

    const formatBlock = kind => wrapSelection(kind === 'strong' ? 'strong' : 'h2')

    const createLink = () => {
      const href = linkUrl.value.trim()
      if (!isSafeEditorUrl(href)) {
        error.value = '请输入有效的 HTTP、HTTPS 或站内链接'
        return
      }
      const selection = window.getSelection()
      if (!selectionBelongsToEditor(selection)) return
      const range = selection.getRangeAt(0)
      const link = document.createElement('a')
      link.setAttribute('href', href)
      if (range.collapsed) link.appendChild(document.createTextNode(href))
      else link.appendChild(range.extractContents())
      range.insertNode(link)
      error.value = ''
      syncMarkdown()
    }

    const insertFragmentAtSelection = fragment => {
      const selection = window.getSelection()
      const range = selectionBelongsToEditor(selection) ? selection.getRangeAt(0) : null
      if (!range) {
        editorRoot.value.appendChild(fragment)
        return
      }
      range.deleteContents()
      const lastNode = fragment.lastChild
      range.insertNode(fragment)
      if (lastNode) {
        range.setStartAfter(lastNode)
        range.collapse(true)
        selection.removeAllRanges()
        selection.addRange(range)
      }
    }

    const handlePaste = event => {
      event.preventDefault()
      const html = event.clipboardData?.getData('text/html') || ''
      if (html) {
        const template = document.createElement('template')
        template.innerHTML = html
        insertFragmentAtSelection(sanitizeEditorFragment(template.content, document))
      } else {
        const fragment = document.createDocumentFragment()
        const lines = (event.clipboardData?.getData('text/plain') || '').split(/\r?\n/)
        lines.forEach((line, index) => {
          if (index) fragment.appendChild(document.createElement('br'))
          fragment.appendChild(document.createTextNode(line))
        })
        insertFragmentAtSelection(fragment)
      }
      syncMarkdown()
    }

    const placeCaretAfter = node => {
      const selection = window.getSelection()
      const range = document.createRange()
      range.setStartAfter(node)
      range.collapse(true)
      selection.removeAllRanges()
      selection.addRange(range)
      savedRange = range.cloneRange()
    }

    const insertUploadedImage = async (file, url, range, generation) => {
      if (generation !== renderGeneration || !editorRoot.value) return
      const image = document.createElement('img')
      image.src = violationImage
      image.alt = file.name.replace(/\.[^.]+$/, '') || '图片'
      image.dataset.originalSrc = url
      image.dataset.imageWidth = '25'
      image.dataset.imageState = 'loading'
      image.style.setProperty('--markdown-image-width', '25%')
      if (!restoreSelection(range)) editorRoot.value.appendChild(image)
      else {
        range.deleteContents()
        range.insertNode(image)
      }
      if (!image.nextSibling) image.parentNode.appendChild(document.createTextNode(''))
      placeCaretAfter(image)
      syncMarkdown()
      try {
        await loadImageUrl(url)
        if (generation !== renderGeneration || !image.isConnected) return
        image.src = url
        image.dataset.imageState = 'loaded'
      } catch (_) {
        if (generation === renderGeneration && image.isConnected) image.dataset.imageState = 'failed'
      }
    }

    const upload = async event => {
      const file = event.target.files?.[0]
      event.target.value = ''
      if (!file || !props.blogId || uploading.value) return
      saveSelection()
      const range = savedRange?.cloneRange() || null
      const generation = renderGeneration
      uploading.value = true
      error.value = ''
      try {
        const url = await uploadBlogImage(props.blogId, file, store.state.user.token)
        await insertUploadedImage(file, url, range, generation)
      } catch (uploadError) {
        error.value = uploadError.message || '图片上传失败'
      } finally {
        uploading.value = false
      }
    }

    const handleKeydown = event => {
      if (event.key === 'Escape') savedRange = null
    }

    const finishComposition = () => {
      composing.value = false
      syncMarkdown()
    }

    const onSelectionChange = () => saveSelection()
    document.addEventListener('selectionchange', onSelectionChange)
    onBeforeUnmount(() => {
      renderGeneration += 1
      document.removeEventListener('selectionchange', onSelectionChange)
    })

    return {
      editorRoot,
      uploading,
      error,
      composing,
      linkUrl,
      formatBlock,
      createLink,
      syncMarkdown,
      handlePaste,
      handleKeydown,
      finishComposition,
      saveSelection,
      upload
    }
  }
}
</script>

<style scoped>
.markdown-editor-toolbar { display: flex; align-items: center; flex-wrap: wrap; gap: .5rem; margin-bottom: .75rem; }
.markdown-link-input { width: min(260px, 100%); }
.wysiwyg-editor { min-height: 350px; padding: 1rem; overflow-y: auto; border: 1px solid #dee2e6; border-radius: 4px; background: white; outline: none; }
.wysiwyg-editor:focus { border-color: #86b7fe; box-shadow: 0 0 0 .2rem rgba(13, 110, 253, .15); }
.wysiwyg-editor:empty::before { color: #adb5bd; content: attr(data-placeholder); pointer-events: none; }
.wysiwyg-editor :deep(img) { display: block; width: var(--markdown-image-width, 25%); max-width: 100%; height: auto; margin: 1rem auto; cursor: pointer; }
</style>
