package com.kob.backend.service.impl.manage;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.kob.backend.mapper.UserMapper;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.service.manage.UpdataBannedService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.Map;

@Service
public class UpdataBannedServiceImpl implements UpdataBannedService {
    @Autowired
    private UserMapper userMapper;

    @Override
    public Map<String, String> updataBanned(String username, Integer status) { // status为0表示将其封禁，1为解除封禁
        UsernamePasswordAuthenticationToken authenticationToken =
                (UsernamePasswordAuthenticationToken) SecurityContextHolder.getContext().getAuthentication();
        UserDetailsImpl loginUser = (UserDetailsImpl) authenticationToken.getPrincipal();
        User user = loginUser.getUser();
        Map<String,String> map = new HashMap<>();
        if(!"true".equals(user.getRoot())){
            map.put("error_message", "你没有权限进行此修改");
            return map;
        }
        QueryWrapper<User> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("username",username);
        user = userMapper.selectOne(queryWrapper);
        if(status == 0){
            user.setBanned("true");
        }
        if(status == 1){
            user.setBanned("false");
        }
        userMapper.update(user,queryWrapper);
        map.put("error_message", "success");
        return map;
    }
}
