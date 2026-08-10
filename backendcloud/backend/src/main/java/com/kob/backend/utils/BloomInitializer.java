package com.kob.backend.utils;

import com.google.common.hash.BloomFilter;
import com.google.common.hash.Funnels;
import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.mapper.UserMapper;
import com.kob.backend.pojo.Blog;
import com.kob.backend.pojo.Topic;
import lombok.Getter;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.ApplicationArguments;
import org.springframework.boot.ApplicationRunner;
import org.springframework.stereotype.Component;

import java.nio.charset.StandardCharsets;
import java.util.List;

@Component
public class BloomInitializer implements ApplicationRunner {

    @Getter
    private BloomFilter<Integer> bloomFilter_Blog =BloomFilter.create(
            Funnels.integerFunnel(),
            100000,
            0.01
    );


    @Getter
    private BloomFilter<Integer> bloomFilter_Topic = BloomFilter.create(
            Funnels.integerFunnel(),
            100000,
            0.01
    );
    @Autowired
    private BlogMapper blogMapper;

    @Autowired
    private TopicMapper topicMapper;

    @Override
    public void run(ApplicationArguments args) throws Exception {
        List<Blog> list_blog = blogMapper.selectList(null);
        List<Topic> list_topic = topicMapper.selectList(null);

        for(Blog blog : list_blog){
            bloomFilter_Blog.put(blog.getId());
        }
        for(Topic topic : list_topic){
            bloomFilter_Topic.put(topic.getId());
        }
    }

}
