package com.kob.backend.controller.oj.answer;

import com.kob.backend.pojo.Answer;
import com.kob.backend.service.oj.answer.AddAnswerService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class AddAnswerController {
    @Autowired
    private AddAnswerService addAnswerService;

    @PostMapping("/oj/answer/add/")
    private Map<String, String> add(@RequestParam Map<String, String> data)
    {
        return addAnswerService.addAnswer(data);
    }
}
