package com.kob.backend.service.impl.user.blog;

import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.pojo.Blog;
import com.kob.backend.service.user.blog.AllGetListService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class AllGetListServiceImpl implements AllGetListService {
    @Autowired
    private BlogMapper blogMapper;

    @Override
    public List<Blog> getAll() {
        return blogMapper.selectList(null);
    }
}
