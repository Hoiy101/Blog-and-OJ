package com.kob.backend.controller.user.account;

import com.kob.backend.service.user.account.AvatarService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.multipart.MultipartFile;

import java.util.Map;

@RestController
public class AvatarController {
    @Autowired
    private AvatarService avatarService;

    @PostMapping("/user/account/avatar/upload/")
    public Map<String, String> uploadAvatar(@RequestParam(value = "file", required = false) MultipartFile file) {
        return avatarService.uploadAvatar(file);
    }
}
