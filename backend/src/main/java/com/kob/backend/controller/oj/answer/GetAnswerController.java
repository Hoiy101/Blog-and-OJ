package com.kob.backend.controller.oj.answer;

import com.kob.backend.service.oj.answer.GetAnswerService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class GetAnswerController {
    @Autowired
    private GetAnswerService getAnswerService;

    @GetMapping("/oj/answer/get/")
    private Map<String, String> GetAnswer(@RequestParam Map<String, String> data){
        return getAnswerService.getAnswer(data);
    }
}
