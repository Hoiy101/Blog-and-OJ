package com.kob.backend.service.blog;

import org.springframework.web.multipart.MultipartFile;

import java.util.Map;

public interface BlogImageService {
    Map<String, String> upload(Integer blogId, MultipartFile file);
}
