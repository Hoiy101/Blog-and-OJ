package com.kob.backend.service.user.account;

import org.springframework.web.multipart.MultipartFile;

import java.util.Map;

public interface AvatarService {
    Map<String, String> uploadAvatar(MultipartFile file);
}
