package com.kob.backend.controller.oj.topic;

import com.kob.backend.service.oj.topic.UpdataTopicService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class UpdataTopicController {
    @Autowired
    UpdataTopicService updataTopicService;

    @PostMapping("/oj/topic/updata/")
    private Map<String,String> updataTopic(@RequestParam Map<String,String> data)
    {
        return updataTopicService.updateTopic(data);
    }
}
