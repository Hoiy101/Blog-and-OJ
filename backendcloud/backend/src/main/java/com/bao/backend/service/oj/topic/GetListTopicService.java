package com.bao.backend.service.oj.topic;

import com.bao.backend.model.PageResponse;
import com.bao.backend.pojo.Topic;

import java.util.List;

public interface GetListTopicService {
    List<Topic> getList();

    PageResponse<Topic> getList(long page, String keyword);
}
