package com.kob.backend.service.manage;

import com.kob.backend.pojo.Evaluate;

import java.util.List;

public interface GetEvaluateService {
    List<Evaluate> getEvaluateList(Integer topic_id);
}
