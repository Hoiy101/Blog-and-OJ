package com.kob.backend.service.impl.oj.topic;

import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.pojo.Topic;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.service.oj.topic.RemoveTopicService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

@Service
public class RemoveTopicServiceImpl implements RemoveTopicService {
    @Autowired
    TopicMapper topicMapper;

    @Override
    public Map<String, String> removeTopic(Map<String, String> data) {
        UsernamePasswordAuthenticationToken authenticationToken =
                (UsernamePasswordAuthenticationToken) SecurityContextHolder.getContext().getAuthentication();
        UserDetailsImpl loginUser = (UserDetailsImpl) authenticationToken.getPrincipal();
        User user = loginUser.getUser();

        Map<String,String> map = new HashMap<>();
        Integer topic_id = Integer.parseInt(data.get("topic_id"));

        Topic topic = topicMapper.selectById(topic_id);
        if(!user.getRoot().equals("true")){
            map.put("error_message", "你没有权限删除该题目");
            return map;
        } else if (topic==null) {
            map.put("error_message", "该题目不存在或已被删除");
        }

        topicMapper.deleteById(topic_id);
        map.put("error_message", "success");
        return map;
    }
}
