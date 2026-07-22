package com.kob.backend.controller.manage.topic;

import com.kob.backend.service.manage.topic.AddTopicService;
import com.kob.backend.service.manage.topic.UpdataEvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;
import java.util.Map;

@RestController
public class UpdataEvaluateController {
    @Autowired
    private UpdataEvaluateService updataEvaluateService;

    @PostMapping("/manage/evaluate/updata/")
    private Map<String, String> updataEvaluate(@RequestBody List<Map<String, String>> data){
        return updataEvaluateService.updataevaluate(data);
    }
}
