package com.kob.backend.controller.oj.topic;

import com.kob.backend.service.oj.topic.GetListTopicService;
import com.kob.backend.utils.PaginationUtils;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class GetListTopicController {
    private final GetListTopicService getListTopicService;

    public GetListTopicController(GetListTopicService getListTopicService) {
        this.getListTopicService = getListTopicService;
    }

    @GetMapping("/oj/topic/getlist/")
    public Object getlist(
            @RequestParam(required = false) String page,
            @RequestParam(defaultValue = "") String keyword) {
        if (page != null) {
            return getListTopicService.getList(PaginationUtils.parsePage(page), keyword);
        }
        return getListTopicService.getList();
    }
}
