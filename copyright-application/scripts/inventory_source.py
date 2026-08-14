#!/usr/bin/env python3
from __future__ import annotations

import hashlib
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "copyright-application" / "qa" / "source-inventory.json"
ALLOWED_ROOTS = (
    ROOT / "web" / "src",
    ROOT / "backendcloud" / "backend" / "src" / "main" / "java",
    ROOT / "backendcloud" / "evaluatesystem" / "src" / "main" / "java",
)
EXTENSIONS = {".java": "Java", ".js": "JavaScript", ".mjs": "JavaScript", ".vue": "Vue"}
SENSITIVE_PATTERNS = {
    "password_assignment": re.compile(r"(?i)password\s*[:=]\s*['\"]?[^\s'\"]+"),
    "secret_assignment": re.compile(r"(?i)(secret|access[_-]?key)\s*[:=]\s*['\"]?[^\s'\"]+"),
    "private_key": re.compile(r"-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY-----"),
    "jdbc_url": re.compile(r"jdbc:[a-z0-9]+://", re.I),
    "bearer_token": re.compile(r"(?i)bearer\s+[a-z0-9._~-]{16,}"),
}


def normalized_lines(path: Path) -> list[str]:
    return path.read_text(encoding="utf-8", errors="replace").replace("\r\n", "\n").splitlines()


def main() -> None:
    files = []
    totals = {"files": 0, "lines": 0, "by_language": {}}
    for base in ALLOWED_ROOTS:
        for path in sorted(p for p in base.rglob("*") if p.suffix.lower() in EXTENSIONS):
            rel = path.relative_to(ROOT).as_posix()
            lines = normalized_lines(path)
            text = "\n".join(lines)
            matches = [name for name, pattern in SENSITIVE_PATTERNS.items() if pattern.search(text)]
            language = EXTENSIONS[path.suffix.lower()]
            files.append({
                "path": rel,
                "language": language,
                "line_count": len(lines),
                "sha256": hashlib.sha256(path.read_bytes()).hexdigest(),
                "sensitive_matches": matches,
            })
            totals["files"] += 1
            totals["lines"] += len(lines)
            totals["by_language"][language] = totals["by_language"].get(language, 0) + len(lines)
    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(json.dumps({"totals": totals, "files": files}, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(f"Inventory: {totals['files']} files, {totals['lines']} lines -> {OUT.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
