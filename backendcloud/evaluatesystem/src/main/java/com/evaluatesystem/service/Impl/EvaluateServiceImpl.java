package com.evaluatesystem.service.Impl;

import com.alibaba.fastjson2.JSONObject;
import com.evaluatesystem.service.EvaluateService;
import com.evaluatesystem.service.utils.EvaluatePool;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.Map;

@Service
public class EvaluateServiceImpl implements EvaluateService {
    private EvaluatePool evaluatePool = new EvaluatePool();

    @Override
    public String Evaluate(JSONObject jsonObject) {
        evaluatePool.startEvaluate(jsonObject);
        System.out.println("add evaluatePool success");
        return "success";
    }
}
