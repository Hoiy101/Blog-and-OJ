package com.kob.backend.service.impl.blog;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.pojo.Blog;
import com.kob.backend.service.blog.GetService;
import com.kob.backend.utils.BloomInitializer;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.data.redis.core.StringRedisTemplate;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.Map;

@Service
public class GetServiceImpl implements GetService {
    @Autowired
    private BlogMapper blogMapper;

    @Autowired
    private StringRedisTemplate redisTemplate;

    @Autowired
    private ObjectMapper objectMapper;

    @Autowired
    private BloomInitializer bloomInitializer;

    @Override
    public Map<String, String> get(Integer blogId) throws JsonProcessingException {
        Blog blog = new Blog();
        Map<String, String> map = new HashMap<>();

        if(!bloomInitializer.getBloomFilter_Blog().mightContain(blogId)){
            map.put("error_message", "该博客不存在或已被删除");
            return map;
        }
        if(!redisTemplate.hasKey("blog:" + blogId)) {
            blog = blogMapper.selectById(blogId);
            if(blog == null){
                map.put("error_message", "该博客不存在或已被删除");
                return map;
            }
            String string_blog = objectMapper.writeValueAsString(blog);
            redisTemplate.opsForValue().set("blog:" + blogId,string_blog);
        }
        else{
            String blog_string = redisTemplate.opsForValue().get("blog:" + blogId);
            blog = objectMapper.readValue(blog_string,Blog.class);
        }

        map.put("title", blog.getTitle());
        map.put("description",  blog.getDescription());
        map.put("content", blog.getContent());
        map.put("createtime", blog.getCreatetime().toString());
        map.put("modifytime",  blog.getModifytime().toString());
        QueryWrapper<Blog> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("title",blog.getTitle());
        Blog blog1=blogMapper.selectOne(queryWrapper);
        bloomInitializer.getBloomFilter_Blog().put(blog1.getId());
        return map;
    }
}
