package com.evaluatesystem.controller;

import com.alibaba.fastjson2.JSONObject;
import com.evaluatesystem.service.EvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.Map;

@RestController
public class EvaluateController {
    @Autowired
    private EvaluateService evaluateService;

    @PostMapping("/oj/evaluate/")
    public String Evaluate(@RequestBody JSONObject jsonObject) {
        return evaluateService.Evaluate(jsonObject);
    }

}
