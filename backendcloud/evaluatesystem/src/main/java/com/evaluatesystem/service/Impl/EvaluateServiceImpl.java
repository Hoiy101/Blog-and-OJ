package com.evaluatesystem.service.Impl;

import com.alibaba.fastjson2.JSONObject;
import com.evaluatesystem.service.EvaluateService;
import com.evaluatesystem.service.utils.EvaluatePool;
import org.springframework.stereotype.Service;

@Service
public class EvaluateServiceImpl implements EvaluateService {
    public static final EvaluatePool evaluatePool = new EvaluatePool();

    @Override
    public String Evaluate(JSONObject jsonObject) {
        evaluatePool.addEvaluate(jsonObject);
        System.out.println("add evaluatePool success");
        return "success";
    }
}
