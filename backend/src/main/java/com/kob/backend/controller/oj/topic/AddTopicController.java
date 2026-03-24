package com.kob.backend.controller.oj.topic;

import com.kob.backend.pojo.Topic;
import com.kob.backend.service.oj.topic.AddTopicService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class AddTopicController {
    @Autowired
    private AddTopicService addTopicService;

    @PostMapping("/oj/topic/add/")
    private Map<String,String> addTopic(@RequestParam Map<String,String> data){
        return addTopicService.add(data);
    }
}
