package com.kob.backend.service.impl.oj.topic;

import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.pojo.Topic;
import com.kob.backend.service.oj.topic.GetListTopicService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.Collections;
import java.util.List;

@Service
public class GetListTopicServiceImpl implements GetListTopicService {
    @Autowired
    private TopicMapper topicMapper;

    @Override
    public List<Topic> getList() {
        return topicMapper.selectList(null);
    }
}
