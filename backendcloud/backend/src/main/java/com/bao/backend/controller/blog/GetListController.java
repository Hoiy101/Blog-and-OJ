package com.bao.backend.controller.blog;

import com.bao.backend.pojo.Blog;
import com.bao.backend.service.blog.GetListService;
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
