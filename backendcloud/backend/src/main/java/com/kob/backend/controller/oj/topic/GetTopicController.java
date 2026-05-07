package com.kob.backend.controller.oj.topic;

import com.kob.backend.service.oj.topic.GetTopicService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class GetTopicController {
    @Autowired
    GetTopicService getTopicService;

    @GetMapping("/oj/topic/get/")
    public Map<String,String> getTopic(@RequestParam Map<String,String> data){
        return getTopicService.getTopic(Integer.parseInt(data.get("id")));
    }
}
