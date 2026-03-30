package com.kob.backend.service.oj.answer;

import com.kob.backend.pojo.Answer;

import java.util.Map;

public interface AddAnswerService {
    Map<String,String> addAnswer(Map<String,String> data);
}
