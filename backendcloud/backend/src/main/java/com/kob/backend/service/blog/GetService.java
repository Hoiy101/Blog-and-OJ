package com.kob.backend.service.blog;

import com.fasterxml.jackson.core.JsonProcessingException;

import java.util.Map;

public interface GetService {
    Map<String,String> get(Integer blogId) throws JsonProcessingException;
}
