package com.kob.backend.controller.oj.evaluate;

import com.alibaba.fastjson2.JSONObject;
import com.kob.backend.consumer.WebSocketServer;
import com.kob.backend.service.oj.evaluate.EndEvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class EndEvaluateController {
    @Autowired
    private EndEvaluateService endEvaluateService;

    @PostMapping("/oj/evaluate/end/")
    public String endEvaluate(@RequestBody JSONObject jsonObject){
        return endEvaluateService.EndEvaluate(jsonObject);
    }
}
