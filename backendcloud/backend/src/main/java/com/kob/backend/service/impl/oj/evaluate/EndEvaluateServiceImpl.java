package com.kob.backend.service.impl.oj.evaluate;

import com.alibaba.fastjson2.JSONObject;
import com.kob.backend.consumer.WebSocketServer;
import com.kob.backend.service.oj.evaluate.EndEvaluateService;
import org.springframework.stereotype.Service;

@Service
public class EndEvaluateServiceImpl implements EndEvaluateService {
    @Override
    public String EndEvaluate(JSONObject jsonObject) {
        System.out.println(jsonObject.get("user_id") + "EndEvaluate success");
        Integer user_id = Integer.parseInt(jsonObject.getString("user_id"));
        WebSocketServer.startEvaluate(jsonObject.getInteger("user_id"),jsonObject.getInteger("evaluation_id"),100);
        return "success";
    }
}
