package com.kob.backend.service.impl.oj.record;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.kob.backend.mapper.RecordOfQuestionMapper;
import com.kob.backend.pojo.RecordOfQuestion;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.service.oj.record.GetListRecordService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class GetListRecordRecordServiceImpl implements GetListRecordService {
    @Autowired
    RecordOfQuestionMapper recordOfQuestionMapper;

    @Override
    public List<RecordOfQuestion> getList() {
        UsernamePasswordAuthenticationToken authenticationToken=
                (UsernamePasswordAuthenticationToken) SecurityContextHolder.getContext().getAuthentication();
        UserDetailsImpl loginUser = (UserDetailsImpl) authenticationToken.getPrincipal();
        User user = loginUser.getUser();

        QueryWrapper<RecordOfQuestion> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("user_id",user.getId());

        return recordOfQuestionMapper.selectList(queryWrapper);
    }
}
