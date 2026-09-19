package com.bao.backend.service.manage;

import com.bao.backend.pojo.Evaluate;

import java.util.List;

public interface GetEvaluateService {
    List<Evaluate> getEvaluateList(Integer topic_id);
}
