package com.kob.backend.service.blog;

import com.kob.backend.model.PageResponse;
import com.kob.backend.pojo.Blog;

public interface AllGetListService {
    PageResponse<Blog> getAll(long page, String keyword);
}
