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
      @click="handleEditorClick"
      @paste="handlePaste"
      @dragover.prevent
      @drop.prevent="handleDrop"
      @keydown="handleKeydown"
      @focus="handleFocus"
      @blur="handleBlur"
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
import {
  adjacentImageFromSelection,
  calculateImageWidth,
  isSafeEditorUrl,
  sanitizeEditorFragment,
  serializeEditor
} from '../utils/wysiwygMarkdown.mjs'
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
    const selectedImage = ref(null)
    const editorHasFocus = ref(false)
    let emittedMarkdown = null
    let pendingExternalMarkdown = null
    let savedRange = null
    let renderGeneration = 0
    let imageControls = null
    let resizeState = null

    const positionImageControls = () => {
      if (!imageControls || !selectedImage.value || !editorRoot.value) return
      const editorRect = editorRoot.value.getBoundingClientRect()
      const imageRect = selectedImage.value.getBoundingClientRect()
      imageControls.style.left = `${imageRect.left - editorRect.left + editorRoot.value.scrollLeft}px`
      imageControls.style.top = `${imageRect.top - editorRect.top + editorRoot.value.scrollTop}px`
      imageControls.style.width = `${imageRect.width}px`
      imageControls.style.height = `${imageRect.height}px`
    }

    const clearImageSelection = () => {
      selectedImage.value?.classList.remove('is-selected')
      selectedImage.value = null
      imageControls?.remove()
      imageControls = null
    }

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
      clearImageSelection()
      editorRoot.value.innerHTML = renderMarkdown(markdown, { imagePlaceholder: violationImage })
      await hydrateMarkdownImages(editorRoot.value, () => generation === renderGeneration)
    }

    watch(() => props.modelValue, value => {
      if (value === emittedMarkdown) {
        emittedMarkdown = null
        return
      }
      if (composing.value || editorHasFocus.value) {
        pendingExternalMarkdown = value
        return
      }
      pendingExternalMarkdown = null
      renderExternalMarkdown(value)
    }, { immediate: true })

    watch(() => props.blogId, (value, previous) => {
      if (previous === undefined || value === previous) return
      emittedMarkdown = null
      pendingExternalMarkdown = null
      renderExternalMarkdown(props.modelValue)
    })

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
      if (!editorRoot.value) return
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
      const blogIdAtStart = props.blogId
      uploading.value = true
      error.value = ''
      try {
        const url = await uploadBlogImage(blogIdAtStart, file, store.state.user.token)
        if (props.blogId !== blogIdAtStart) {
          error.value = '文章已切换，上传的图片未插入正文'
          return
        }
        await insertUploadedImage(file, url, generation === renderGeneration ? range : null, renderGeneration)
      } catch (uploadError) {
        error.value = uploadError.message || '图片上传失败'
      } finally {
        uploading.value = false
      }
    }

    const continueResize = event => {
      if (!resizeState || event.pointerId !== resizeState.pointerId) return
      const distance = Math.abs(event.clientX - resizeState.centerX) * 2
      const virtualPointerX = resizeState.editorLeft + distance
      const width = calculateImageWidth(virtualPointerX, resizeState.editorLeft, resizeState.editorWidth)
      resizeState.image.dataset.imageWidth = String(width)
      resizeState.image.style.setProperty('--markdown-image-width', `${width}%`)
      positionImageControls()
    }

    const removeResizeListeners = () => {
      window.removeEventListener('pointermove', continueResize)
      window.removeEventListener('pointerup', finishResize)
      window.removeEventListener('pointercancel', finishResize)
    }

    const finishResize = event => {
      if (!resizeState || (event?.pointerId != null && event.pointerId !== resizeState.pointerId)) return
      removeResizeListeners()
      resizeState = null
      positionImageControls()
      syncMarkdown()
    }

    const beginResize = (event, side) => {
      if (!selectedImage.value || !editorRoot.value) return
      event.preventDefault()
      event.stopPropagation()
      const editorRect = editorRoot.value.getBoundingClientRect()
      resizeState = {
        image: selectedImage.value,
        side,
        pointerId: event.pointerId,
        editorLeft: editorRect.left,
        editorWidth: editorRect.width,
        centerX: editorRect.left + editorRect.width / 2
      }
      window.addEventListener('pointermove', continueResize)
      window.addEventListener('pointerup', finishResize)
      window.addEventListener('pointercancel', finishResize)
    }

    const createResizeHandle = (side, label) => {
      const handle = document.createElement('button')
      handle.type = 'button'
      handle.className = `image-resize-handle image-resize-handle-${side}`
      handle.dataset.editorUi = 'resize-handle'
      handle.contentEditable = 'false'
      handle.setAttribute('aria-label', label)
      handle.addEventListener('pointerdown', event => beginResize(event, side))
      handle.addEventListener('click', event => event.stopPropagation())
      return handle
    }

    const selectImage = image => {
      clearImageSelection()
      selectedImage.value = image
      image.classList.add('is-selected')
      imageControls = document.createElement('span')
      imageControls.className = 'image-resize-controls'
      imageControls.dataset.editorUi = 'image-controls'
      imageControls.contentEditable = 'false'
      imageControls.appendChild(createResizeHandle('left', '从左侧调整图片大小'))
      imageControls.appendChild(createResizeHandle('right', '从右侧调整图片大小'))
      editorRoot.value.appendChild(imageControls)
      positionImageControls()
    }

    const handleEditorClick = event => {
      if (event.target.closest?.('[data-editor-ui]')) return
      const image = event.target.closest?.('img[data-original-src]')
      if (image && editorRoot.value?.contains(image)) {
        event.preventDefault()
        selectImage(image)
      } else clearImageSelection()
    }

    const placeCaretAtRemovalPoint = (parent, index) => {
      if (!parent || !editorRoot.value?.contains(parent) && parent !== editorRoot.value) return
      const range = document.createRange()
      range.setStart(parent, Math.min(index, parent.childNodes.length))
      range.collapse(true)
      const selection = window.getSelection()
      selection.removeAllRanges()
      selection.addRange(range)
      savedRange = range.cloneRange()
    }

    const removeImageFromEditor = image => {
      const parent = image.parentNode
      const index = [...parent.childNodes].indexOf(image)
      clearImageSelection()
      image.remove()
      placeCaretAtRemovalPoint(parent, index)
      syncMarkdown()
    }

    const handleKeydown = event => {
      if (event.key === 'Escape') {
        clearImageSelection()
        savedRange = null
        return
      }
      const deletingBackward = event.key === 'Backspace'
      const deletingForward = event.key === 'Delete'
      if (!deletingBackward && !deletingForward) {
        clearImageSelection()
        return
      }
      const selection = window.getSelection()
      const image = selectedImage.value || adjacentImageFromSelection(
        selection,
        deletingBackward ? 'backward' : 'forward'
      )
      if (!image || !editorRoot.value?.contains(image)) return
      event.preventDefault()
      removeImageFromEditor(image)
    }

    const finishComposition = () => {
      composing.value = false
      syncMarkdown()
    }

    const handleFocus = () => {
      editorHasFocus.value = true
      saveSelection()
    }

    const handleBlur = () => {
      editorHasFocus.value = false
      if (pendingExternalMarkdown === null || !editorRoot.value) return
      const value = props.modelValue
      pendingExternalMarkdown = null
      if (value !== serializeEditor(editorRoot.value)) renderExternalMarkdown(value)
    }

    const handleDrop = () => {
      error.value = '请使用“上传图片”按钮添加本地图片'
    }

    const onSelectionChange = () => saveSelection()
    document.addEventListener('selectionchange', onSelectionChange)
    onBeforeUnmount(() => {
      renderGeneration += 1
      removeResizeListeners()
      clearImageSelection()
      document.removeEventListener('selectionchange', onSelectionChange)
    })

    return {
      editorRoot,
      uploading,
      error,
      composing,
      editorHasFocus,
      linkUrl,
      formatBlock,
      createLink,
      syncMarkdown,
      handlePaste,
      handleDrop,
      handleEditorClick,
      handleKeydown,
      beginResize,
      continueResize,
      finishResize,
      finishComposition,
      handleFocus,
      handleBlur,
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
.wysiwyg-editor { position: relative; }
.wysiwyg-editor:focus { border-color: #86b7fe; box-shadow: 0 0 0 .2rem rgba(13, 110, 253, .15); }
.wysiwyg-editor:empty::before { color: #adb5bd; content: attr(data-placeholder); pointer-events: none; }
.wysiwyg-editor :deep(img) { display: block; width: var(--markdown-image-width, 25%); max-width: 100%; height: auto; margin: 1rem auto; cursor: pointer; }
.wysiwyg-editor :deep(img.is-selected) { outline: 3px solid #0d6efd; outline-offset: 3px; }
.wysiwyg-editor :deep(.image-resize-controls) { position: absolute; z-index: 2; display: block; pointer-events: none; }
.wysiwyg-editor :deep(.image-resize-handle) { position: absolute; top: 50%; width: 16px; height: 32px; padding: 0; border: 2px solid #fff; border-radius: 4px; background: #0d6efd; box-shadow: 0 1px 4px rgba(0, 0, 0, .35); cursor: ew-resize; pointer-events: auto; transform: translateY(-50%); }
.wysiwyg-editor :deep(.image-resize-handle-left) { left: -8px; }
.wysiwyg-editor :deep(.image-resize-handle-right) { right: -8px; }
</style>
