export function createBlogImageFormData(blogId, file, FormDataClass = FormData) {
  const data = new FormDataClass()
  data.append('blog_id', String(blogId)); data.append('file', file)
  return data
}

export async function uploadBlogImage(blogId, file, token) {
  const response = await fetch('http://127.0.0.1:3000/user/blog/image/upload/', {
    method: 'POST', headers: { Authorization: `Bearer ${token}` }, body: createBlogImageFormData(blogId, file)
  })
  const data = await response.json()
  if (!response.ok || data.error_message !== 'success') throw new Error(data.error_message || '图片上传失败')
  return data.url
}
