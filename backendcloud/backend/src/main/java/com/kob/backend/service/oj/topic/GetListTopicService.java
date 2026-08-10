package com.kob.backend.service.oj.topic;

import com.kob.backend.model.PageResponse;
import com.kob.backend.pojo.Topic;

import java.util.List;

public interface GetListTopicService {
    List<Topic> getList();

    PageResponse<Topic> getList(long page, String keyword);
}
