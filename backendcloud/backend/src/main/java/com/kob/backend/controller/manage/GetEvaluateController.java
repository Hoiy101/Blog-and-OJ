package com.kob.backend.controller.manage;

import com.kob.backend.pojo.Evaluate;
import com.kob.backend.service.manage.GetEvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;
import java.util.Map;

@RestController
public class GetEvaluateController {
    @Autowired
    private GetEvaluateService getEvaluateService;

    @PostMapping("/manage/evaluate/get/")
    private List<Evaluate> getEvaluateList(@RequestParam Map<String,String> map){
        Integer topic_id = Integer.parseInt(map.get("topic_id"));
        return getEvaluateService.getEvaluateList(topic_id);
    }


}
