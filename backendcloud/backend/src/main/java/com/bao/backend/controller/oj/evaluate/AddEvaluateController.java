package com.bao.backend.controller.oj.evaluate;

import com.bao.backend.service.oj.evaluate.AddEvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
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
