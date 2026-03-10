package com.kob.backend.controller.user.blog;

import com.kob.backend.pojo.Blog;
import com.kob.backend.service.user.blog.GetListService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
public class GetListController {
    @Autowired
    private GetListService getListService;

    @GetMapping("/user/bot/getlist/")
    public List<Blog> getList(){
        return getListService.getList();
    }
}
