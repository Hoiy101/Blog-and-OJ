package com.evaluatesystem.controller;

import com.evaluatesystem.service.utils.EvaluatePool;
import org.springframework.amqp.rabbit.core.RabbitAdmin;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Properties;

@RestController
public class StatusController {
    private final EvaluatePool evaluatePool;
    private final RabbitAdmin rabbitAdmin;
    public StatusController(EvaluatePool evaluatePool, RabbitAdmin rabbitAdmin) {
        this.evaluatePool = evaluatePool;
        this.rabbitAdmin = rabbitAdmin;
    }

    @GetMapping("/status/")
    public Map<String, Object> status(){
        Map<String, Object> map = new LinkedHashMap<>();
        Properties q = rabbitAdmin.getQueueProperties("evaluate.task.queue");
        map.put("online", "true");
        map.put("queues", q == null ? -1 : q.get(RabbitAdmin.QUEUE_MESSAGE_COUNT));
        map.put("running", evaluatePool.getAtomicInteger());
        return map;
    }
}
