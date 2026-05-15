package com.kob.backend.service.impl.oj.evaluate;

import com.alibaba.fastjson2.JSONObject;
import com.kob.backend.consumer.WebSocketServer;
import com.kob.backend.service.oj.evaluate.EndEvaluateService;
import org.springframework.stereotype.Service;

@Service
public class EndEvaluateServiceImpl implements EndEvaluateService {
    @Override
    public String EndEvaluate(JSONObject jsonObject) {
        System.out.println(jsonObject.getString("score"));
        Integer user_id = Integer.parseInt(jsonObject.getString("user_id"));
        Integer evaluate_id =  Integer.parseInt(jsonObject.getString("evaluate_id"));
        Integer score = Integer.parseInt(jsonObject.getString("score"));
        String state = jsonObject.getString("state");
        WebSocketServer.startEvaluate(user_id, evaluate_id, score, state);
        return "success";
    }
}
