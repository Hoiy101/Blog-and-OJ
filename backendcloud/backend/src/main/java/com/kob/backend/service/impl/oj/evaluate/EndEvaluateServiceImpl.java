package com.kob.backend.service.impl.oj.evaluate;

import com.alibaba.fastjson2.JSONObject;
import com.kob.backend.consumer.WebSocketServer;
import com.kob.backend.mapper.RecordOfQuestionMapper;
import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.pojo.RecordOfQuestion;
import com.kob.backend.service.oj.evaluate.EndEvaluateService;
import lombok.Data;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import javax.sql.DataSource;
import java.util.Date;

@Service
public class EndEvaluateServiceImpl implements EndEvaluateService {
    @Autowired
    private RecordOfQuestionMapper recordOfQuestionMapper;
    @Autowired
    private TopicMapper topicMapper;
    @Autowired
    private DataSource dataSource;

    @Override
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
