package com.evaluatesystem.service.utils;

import com.alibaba.fastjson2.JSONObject;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.context.event.ApplicationReadyEvent;
import org.springframework.context.event.EventListener;
import org.springframework.stereotype.Component;

import java.util.Queue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

@Component
public class EvaluatePool {
    @Autowired
    private final Consumer consumer;

    public EvaluatePool(Consumer consumer) {
        this.consumer = consumer;
    }

    @RabbitListener(queues = "evaluate.task.queue", concurrency = "2")
    private void pool(JSONObject message) {
        consumer.startEvaluate(message);
    }
}
