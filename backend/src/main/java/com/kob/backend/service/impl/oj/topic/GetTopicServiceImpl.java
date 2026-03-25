package com.kob.backend.service.impl.oj.topic;

import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.pojo.Topic;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.service.oj.topic.GetTopicService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

@Service
public class GetTopicServiceImpl implements GetTopicService {
    @Autowired
    private TopicMapper topicMapper;
    @Override
    public Map<String, String> getTopic(Integer topic_id) {
        Topic topic = topicMapper.selectById(topic_id);
        Map<String,String> map = new HashMap<>();
        map.put("test_point", topic.getTestPoint().toString());
        map.put("title", topic.getTitle());
        map.put("description", topic.getDescription());
        map.put("star", topic.getStar());
        map.put("time_limit", topic.getTimeLimit().toString());
        map.put("mem_limit", topic.getMemLimit().toString());
        map.put("input_format",  topic.getInputFormat());
        map.put("output_format",  topic.getOutputFormat());
        map.put("sample_input", topic.getSampleInput());
        map.put("sample_output", topic.getSampleOutput());
        map.put("hint",  topic.getHint());
        return map;
    }
}
