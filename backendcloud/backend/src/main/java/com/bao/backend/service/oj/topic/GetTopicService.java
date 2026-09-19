package com.bao.backend.service.oj.topic;

import com.fasterxml.jackson.core.JsonProcessingException;

import java.util.Map;

public interface GetTopicService {
    Map<String,String> getTopic(Integer topic_id) throws JsonProcessingException;
}
