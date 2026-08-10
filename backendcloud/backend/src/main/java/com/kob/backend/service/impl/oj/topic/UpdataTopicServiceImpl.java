package com.kob.backend.service.impl.oj.topic;

import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.pojo.Topic;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.service.oj.topic.UpdataTopicService;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.Map;

@Service
public class UpdataTopicServiceImpl implements UpdataTopicService {
    private final TopicMapper topicMapper;

    public UpdataTopicServiceImpl(TopicMapper topicMapper) {
        this.topicMapper = topicMapper;
    }

    @Override
    public Map<String, String> updateTopic(Map<String, String> data) {
        UsernamePasswordAuthenticationToken authenticationToken =
                (UsernamePasswordAuthenticationToken) SecurityContextHolder.getContext().getAuthentication();
        UserDetailsImpl loginUser = (UserDetailsImpl) authenticationToken.getPrincipal();
        User user = loginUser.getUser();

        HashMap<String,String> map=new HashMap<>();


        if(!user.getRoot().equals("true")){
            map.put("error_message", "你没有权限进行此操作");
            return map;
        }
        if(data.get("test_point")==null || data.get("test_point").equals("0")){
            map.put("error_message", "测试点数量不能为0");
            return map;
        }
        if(data.get("title")==null || data.get("title").isEmpty()){
            map.put("error_message", "标题不能为空");
            return map;
        }
        if(data.get("title").length()>100){
            map.put("error_message", "标题的长度不能超过100");
            return map;
        }
        if(data.get("description")==null || data.get("description").isEmpty()){
            map.put("error_message", "题目描述不能为空");
            return map;
        }
        if(data.get("time_limit") ==null || data.get("time_limit").equals("0")){
            map.put("error_message", "时间限制不能为0");
            return map;
        }
        if(data.get("mem_limit") ==null || data.get("mem_limit").equals("0")){
            map.put("error_message", "内存限制不能为0");
            return map;
        }

        Topic new_topic=new Topic(
                Integer.parseInt(data.get("topic_id")),
                Integer.parseInt(data.get("test_point")),
                data.get("title"),
                data.get("description"),
                data.get("star"),
                Integer.parseInt(data.get("time_limit")),
                Integer.parseInt(data.get("mem_limit")),
                data.get("input_format"),
                data.get("output_format"),
                data.get("sample_input"),
                data.get("sample_output"),
                data.get("hint")
        );
        topicMapper.updateById(new_topic);

        map.put("error_message", "success");
        return map;
    }
}
