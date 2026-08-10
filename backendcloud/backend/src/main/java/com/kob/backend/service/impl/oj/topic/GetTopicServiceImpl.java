package com.kob.backend.service.impl.oj.topic;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.pojo.Topic;
import com.kob.backend.service.oj.topic.GetTopicService;
import com.kob.backend.utils.BloomInitializer;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.redis.core.StringRedisTemplate;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.Map;

@Service
public class GetTopicServiceImpl implements GetTopicService {
    @Autowired
    private TopicMapper topicMapper;

    @Autowired
    private StringRedisTemplate redisTemplate;

    @Autowired
    private ObjectMapper objectMapper;

    @Autowired
    private BloomInitializer bloomInitializer;

    @Override
    public Map<String, String> getTopic(Integer topic_id) throws JsonProcessingException {
        Topic topic = new Topic();
        Map<String,String> map = new HashMap<>();
        if(!bloomInitializer.getBloomFilter_Topic().mightContain(topic_id)){
            map.put("error_message", "该题目不存在或已被删除");
            return map;
        }
        if(!redisTemplate.hasKey("topic:" + topic_id)){
            topic = topicMapper.selectById(topic_id);
            if(topic == null){
                map.put("error_message", "该题目不存在或已被删除");
                return map;
            }
            String topic_string = objectMapper.writeValueAsString(topic);
            redisTemplate.opsForValue().set("topic:" + topic_id,  topic_string);
        }
        else{
            String topic_string = redisTemplate.opsForValue().get("topic:" + topic_id);
            topic = objectMapper.readValue(topic_string, Topic.class);
        }
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
