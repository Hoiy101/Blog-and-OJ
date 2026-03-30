package com.kob.backend.service.impl.answer;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.kob.backend.mapper.AnswerMapper;
import com.kob.backend.pojo.Answer;
import com.kob.backend.service.oj.answer.GetAnswerService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
public class GetAnswerServiceImpl implements GetAnswerService {
    @Autowired
    private AnswerMapper answerMapper;

    @Override
    public Map<String, String> getAnswer(Map<String, String> data) {
        Integer topic_id = Integer.parseInt(data.get("topic_id"));
        QueryWrapper<Answer> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("topic_id",topic_id);

        List<Answer> answers = answerMapper.selectList(queryWrapper);
        Map<String,String> map = new HashMap<>();

        if(answers==null|| answers.isEmpty()){
            map.put("error_message", "该题暂无题解");
            return map;
        }
        Answer answer = answers.get(0);
        map.put("title",answer.getTitle());
        map.put("content",answer.getContent());
        map.put("error_message", "success");
        return map;
    }
}
