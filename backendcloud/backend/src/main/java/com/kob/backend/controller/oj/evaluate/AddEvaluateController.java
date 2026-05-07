package com.kob.backend.controller.oj.evaluate;

import com.kob.backend.pojo.Evaluate;
import com.kob.backend.service.oj.evaluate.AddEvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class AddEvaluateController {
    @Autowired
    private AddEvaluateService addEvaluateService;

    @PostMapping("/oj/evaluate/add/")
    public String addEvaluate(@RequestParam Map<String,String> data){
        return addEvaluateService.AddEvaluate(data);
    }
}
