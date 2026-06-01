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
public class EvaluatePool extends Thread {
    public RabbitTemplate rabbitTemplate;

    private BlockingQueue<JSONObject> queue = new LinkedBlockingQueue<>();

    @Autowired
    private Consumer consumer;

    @Autowired
    public void setRabbitTemplate(RabbitTemplate rabbitTemplate) {
        this.rabbitTemplate = rabbitTemplate;
    }

    @EventListener(ApplicationReadyEvent.class)
    public void startThread(){
        this.start();
    }

    @RabbitListener (queues = "evaluate.task.queue")
    public void EvaluateTask(JSONObject message) {
        try {
            queue.put(message);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }

    }


    @Override
    public void run() {
        while (true) {
            JSONObject message = null;
            try {
                message = queue.take();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            System.out.println("EvaluatePool: " + message);
            consumer.startEvaluate(message);
        }
    }
}
