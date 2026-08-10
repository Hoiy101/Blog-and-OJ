package com.kob.backend.controller.blog;

import com.kob.backend.model.PageResponse;
import com.kob.backend.pojo.Blog;
import com.kob.backend.service.blog.AllGetListService;
import com.kob.backend.utils.PaginationUtils;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class AllGetListController {
    private final AllGetListService allGetListService;

    public AllGetListController(AllGetListService allGetListService) {
        this.allGetListService = allGetListService;
    }

    @GetMapping("/user/bot/all/getlist/")
    public PageResponse<Blog> getAll(
            @RequestParam(defaultValue = "1") String page,
            @RequestParam(defaultValue = "") String keyword) {
        return allGetListService.getAll(PaginationUtils.parsePage(page), keyword);
    }
}
