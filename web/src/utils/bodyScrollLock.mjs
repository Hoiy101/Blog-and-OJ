const owners = new Set()
let previousOverflow = ''

export function lockBodyScroll(owner, body = document.body) {
  if (!owners.size) previousOverflow = body.style.overflow
  owners.add(owner); body.style.overflow = 'hidden'
}
export function unlockBodyScroll(owner, body = document.body) {
  owners.delete(owner)
  if (!owners.size) body.style.overflow = previousOverflow
}
export function resetBodyScrollLocks(body = document.body) {
  owners.clear(); previousOverflow = body.style.overflow
}
