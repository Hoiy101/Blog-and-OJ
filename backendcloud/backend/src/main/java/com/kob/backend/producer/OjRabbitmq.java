package com.kob.backend.producer;

import com.alibaba.fastjson2.JSONObject;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
public class OjRabbitmq {

    public static RabbitTemplate rabbitTemplate;

    @Autowired
    public void setRabbitTemplate(RabbitTemplate rabbitTemplate) {
        OjRabbitmq.rabbitTemplate = rabbitTemplate;
    }

    public void stateMessage(JSONObject message) {
        rabbitTemplate.convertAndSend("evaluate.task.exchange", "evaluate.task", message);
    }
}
