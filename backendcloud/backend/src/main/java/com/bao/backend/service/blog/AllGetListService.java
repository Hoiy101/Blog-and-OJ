package com.bao.backend.service.blog;

import com.bao.backend.model.PageResponse;
import com.bao.backend.pojo.Blog;

public interface AllGetListService {
    PageResponse<Blog> getAll(long page, String keyword);
}
