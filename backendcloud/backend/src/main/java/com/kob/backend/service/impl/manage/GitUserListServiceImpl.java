package com.kob.backend.service.impl.manage;

import com.kob.backend.mapper.UserMapper;
import com.kob.backend.pojo.User;
import com.kob.backend.service.manage.GitUserListService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

@Service
public class GitUserListServiceImpl implements GitUserListService {
    @Autowired
    private UserMapper userMapper;

    @Override
    public List<HashMap<String, String>> getuserlist() {
        List<User> list = userMapper.selectList(null);
        List<HashMap<String, String>> new_list = new ArrayList<>();
        for (User user : list) {
            HashMap<String, String> map = new HashMap<>();
            map.put("username", user.getUsername());
            map.put("root", user.getRoot());
            map.put("banned",  user.getBanned());
            new_list.add(map);
        }
        return new_list;
    }
}
