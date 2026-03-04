package com.kob.backend.service.impl.user;

import com.kob.backend.mapper.UserMapper;
import com.kob.backend.pojo.Question;
import com.kob.backend.pojo.RecordOfQuestion;
import com.kob.backend.service.userInterface.UserService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class UserServiceImpl implements UserService {

    @Autowired
    private UserMapper  userMapper;

    @Override
    public List<Question> getRecord(Integer userId) {
        return userMapper.getRecord(userId);
    }

    @Override
    public void addRecord(RecordOfQuestion record) {
        userMapper.save(record);
    }

    @Override
    public void update(RecordOfQuestion record) {
        userMapper.updateRecord(record);
    }
}
