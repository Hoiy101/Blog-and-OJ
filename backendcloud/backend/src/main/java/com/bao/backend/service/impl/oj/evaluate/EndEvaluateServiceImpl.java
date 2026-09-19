package com.bao.backend.service.impl.oj.evaluate;

import com.alibaba.fastjson2.JSONObject;
import com.bao.backend.consumer.WebSocketServer;
import com.bao.backend.mapper.RecordOfQuestionMapper;
import com.bao.backend.mapper.TopicMapper;
import com.bao.backend.pojo.RecordOfQuestion;
import org.springframework.amqp.rabbit.annotation.RabbitListener;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.context.event.ApplicationReadyEvent;
import org.springframework.context.event.EventListener;
import org.springframework.stereotype.Component;

import java.util.Date;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

@Component
public class EndEvaluateServiceImpl extends Thread {
    @Autowired
    private RecordOfQuestionMapper recordOfQuestionMapper;
    @Autowired
    private TopicMapper topicMapper;
    @Autowired
    RabbitTemplate rabbitTemplate;

    BlockingQueue<JSONObject> queue = new LinkedBlockingQueue<>();

    @RabbitListener (queues = "evaluate.result.queue")
    public void EvaluateResult(JSONObject message) {
        try {
            queue.put(message);
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
    }

    @EventListener(ApplicationReadyEvent.class)
    public void startThread(){
        this.start();
    }

    @Override
    public void run() {
        while (true) {
            JSONObject message;
            try {
                message = queue.take();
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            System.out.println("End: " + message);
            EndEvaluate(message);
        }
    }

    public String EndEvaluate(JSONObject jsonObject) {
        System.out.println(jsonObject.getString("score"));
        Integer user_id = Integer.parseInt(jsonObject.getString("user_id"));
        Integer evaluate_id =  Integer.parseInt(jsonObject.getString("evaluate_id"));
        Integer score = Integer.parseInt(jsonObject.getString("score"));
        String state = jsonObject.getString("state");

        String title = topicMapper.selectById(evaluate_id).getTitle();
        Date date = new Date();
        RecordOfQuestion recordOfQuestion = new RecordOfQuestion(null,
                user_id,
                evaluate_id,
                title,
                state,
                score,
                date
                );
        recordOfQuestionMapper.insert(recordOfQuestion);
        WebSocketServer.startEvaluate(user_id, evaluate_id, score, state);
        return "success";
    }
}
