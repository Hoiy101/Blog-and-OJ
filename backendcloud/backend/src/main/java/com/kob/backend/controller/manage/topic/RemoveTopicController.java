package com.kob.backend.controller.manage.topic;

import com.kob.backend.service.manage.topic.RemoveTopicService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class RemoveTopicController {
    @Autowired
    private RemoveTopicService removeTopicService;

    @PostMapping("/oj/topic/remove/")
    public Map<String,String> removeTopic(@RequestParam Map<String,String> data){
        return  removeTopicService.removeTopic(data);
    }
}
