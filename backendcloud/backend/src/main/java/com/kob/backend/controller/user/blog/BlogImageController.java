package com.kob.backend.controller.user.blog;

import com.kob.backend.service.user.blog.BlogImageService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.multipart.MultipartFile;

import java.util.Map;

@RestController
public class BlogImageController {
    @Autowired
    private BlogImageService blogImageService;

    @PostMapping("/user/blog/image/upload/")
    public Map<String, String> upload(@RequestParam("blog_id") Integer blogId,
                                      @RequestParam("file") MultipartFile file) {
        return blogImageService.upload(blogId, file);
    }
}
