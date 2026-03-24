package com.kob.backend.service.impl.oj.record;


import com.kob.backend.mapper.RecordOfQuestionMapper;
import com.kob.backend.pojo.RecordOfQuestion;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.service.oj.record.AddRecordService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;

@Service
public class AddRecordRecordServiceImpl implements AddRecordService {
    @Autowired
    private RecordOfQuestionMapper recordOfQuestionMapper;

    @Override
    public Map<String, String> add(Map<String, String> data) {
        UsernamePasswordAuthenticationToken authenticationToken =
                (UsernamePasswordAuthenticationToken) SecurityContextHolder.getContext().getAuthentication();
        UserDetailsImpl loginUser = (UserDetailsImpl) authenticationToken.getPrincipal();
        User user = loginUser.getUser();

        Map<String,String> map = new HashMap<>();

        int question_id = Integer.parseInt(data.get("question_id"));
        String title = data.get("title");
        String state =  data.get("state");
        int score = Integer.parseInt(data.get("score"));

        Date date = new Date();
        RecordOfQuestion recordOfQuestion = new RecordOfQuestion(null, user.getId(), question_id, title, state, score, date);
        recordOfQuestionMapper.insert(recordOfQuestion);
        map.put("error_message", "success");

        return map;
    }
}
