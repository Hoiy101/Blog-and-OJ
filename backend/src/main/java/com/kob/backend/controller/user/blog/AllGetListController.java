package com.kob.backend.controller.user.blog;

import com.kob.backend.pojo.Blog;
import com.kob.backend.service.user.blog.AllGetListService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
public class AllGetListController {
    @Autowired
    private AllGetListService allGetListService;

    @GetMapping("/user/bot/all/getlist/")
    public List<Blog> getAll()
    {
        return allGetListService.getAll();
    }
}
