package com.kob.backend.service.blog;

import com.fasterxml.jackson.core.JsonProcessingException;

import java.util.Map;

public interface UpdateService {
    Map<String,String> update(Map<String,String> data) throws JsonProcessingException;
}
