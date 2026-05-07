package com.evaluatesystem.service.utils;

import com.alibaba.fastjson2.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.web.client.RestTemplate;

@Component
public class EvaluatePool {

    public static RestTemplate restTemplate;


    @Autowired
    public void setEvaluatePool(RestTemplate restTemplate) {
        EvaluatePool.restTemplate = restTemplate;
    }

    private final String EvaluatePassUrl = "http://127.0.0.1:3000/oj/evaluate/end/";

    private void EvaluatePass(JSONObject jsonObject) {
        JSONObject passObject = new JSONObject();
        passObject.put("user_id",  jsonObject.getString("user_id"));
        restTemplate.postForObject(EvaluatePassUrl, passObject, String.class);
        System.out.println(jsonObject.get("user_id") + "EvaluatePass Success");
    }

    public void startEvaluate(JSONObject jsonObject) {
        EvaluatePass(jsonObject);

    }
}
