package com.bao.backend.service.impl.oj.answer;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.bao.backend.mapper.AnswerMapper;
import com.bao.backend.pojo.Answer;
import com.bao.backend.pojo.User;
import com.bao.backend.service.impl.utils.UserDetailsImpl;
import com.bao.backend.service.oj.answer.AddAnswerService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
public class AddAnswerServiceImpl implements AddAnswerService {
    @Autowired
    private AnswerMapper answerMapper;

    @Override
    public Map<String, String> addAnswer(Map<String, String> data) {
        UsernamePasswordAuthenticationToken authenticationToken =
                (UsernamePasswordAuthenticationToken) SecurityContextHolder.getContext().getAuthentication();
        UserDetailsImpl loginUser = (UserDetailsImpl) authenticationToken.getPrincipal();
        User user = loginUser.getUser();
        Integer topic_id = Integer.parseInt(data.get("topic_id"));
        String title = data.get("title");
        String content = data.get("content");

        QueryWrapper<Answer> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("topic_id",topic_id);

        List<Answer> answers = answerMapper.selectList(queryWrapper);

        Map<String,String> map = new HashMap<>();
        if(!user.getRoot().equals("true")){
            map.put("error_message", "你没有权限写入题解");
            return map;
        }
        if(!answers.isEmpty()){
            map.put("error_massage", "该题已存在题解");
            return map;
        }
        if(title==null || title.isEmpty()){
            map.put("error_message", "标题不能为空");
            return map;
        }
        if(content==null || content.isEmpty()){
            map.put("error_message", "内容不能为空");
        }
        Answer answer =  new Answer(null, topic_id, title, content);
        answerMapper.insert(answer);
        map.put("error_message", "success");
        return map;
    }
}
