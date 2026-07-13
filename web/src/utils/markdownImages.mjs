const cache = new Map()

export function loadImageUrl(url, ImageClass = Image) {
  if (cache.has(url)) return cache.get(url)
  const promise = new Promise((resolve, reject) => {
    const image = new ImageClass()
    image.onload = () => resolve(url)
    image.onerror = () => reject(new Error(`图片加载失败: ${url}`))
    image.src = url
  })
  cache.set(url, promise)
  return promise
}

export function resetImageLoadCache() { cache.clear() }

export async function hydrateMarkdownImages(root, isCurrent = () => true) {
  const nodes = [...(root?.querySelectorAll?.('img[data-original-src]') ?? [])]
  await Promise.all(nodes.map(async node => {
    const url = node.dataset.originalSrc
    try {
      await loadImageUrl(url)
      if (!isCurrent()) return
      node.src = url
      node.dataset.imageState = 'loaded'
    } catch (_) {
      if (isCurrent()) node.dataset.imageState = 'failed'
    }
  }))
}
