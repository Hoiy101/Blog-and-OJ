package com.kob.backend.service.impl.user.blog;

import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.pojo.Blog;
import com.kob.backend.service.user.blog.GetService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.Map;

@Service
public class GetServiceImpl implements GetService {
    @Autowired
    private BlogMapper blogMapper;

    @Override
    public Map<String, String> get(Integer blogId) {
        Blog blog = blogMapper.selectById(blogId);
        Map<String, String> map = new HashMap<>();
        if(blog == null)
        {
            map.put("error_message", "该博客不存在或已被删除");
        }
        else{
            map.put("title", blog.getTitle());
            map.put("description",  blog.getDescription());
            map.put("content", blog.getContent());
            map.put("createtime", blog.getCreatetime().toString());
            map.put("modifytime",  blog.getModifytime().toString());
        }
        return map;
    }
}
