package com.kob.backend.controller.manage.topic;

import com.kob.backend.pojo.Topic;
import com.kob.backend.service.manage.topic.GetListTopicService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
public class GetListTopicController {
    @Autowired
    private GetListTopicService getListTopicService;

    @GetMapping("/oj/topic/getlist/")
    public List<Topic> getlist(){
        return getListTopicService.getList();
    }
}
