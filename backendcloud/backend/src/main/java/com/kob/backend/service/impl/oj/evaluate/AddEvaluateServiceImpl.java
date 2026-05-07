package com.kob.backend.service.impl.oj.evaluate;

import com.alibaba.fastjson2.JSONObject;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.kob.backend.config.RestTemplateConfig;
import com.kob.backend.mapper.EvaluateMapper;
import com.kob.backend.pojo.Blog;
import com.kob.backend.pojo.Evaluate;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.service.oj.evaluate.AddEvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;

import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
public class AddEvaluateServiceImpl implements AddEvaluateService {
    @Autowired
    private EvaluateMapper evaluateMapper;

    @Autowired
    private RestTemplate restTemplate;

    private final static String evaluateUrl = "http://127.0.0.1:3001/oj/evaluate/";

    @Override
    public String AddEvaluate(Map<String,String> data) {
        Integer evaluateId = Integer.parseInt(data.get("evaluateId"));
        String code = data.get("code");
        String language = data.get("language");
        if(code == null || code.isEmpty()){
            return "代码不能为空";
        }
        UsernamePasswordAuthenticationToken authenticationToken =
                (UsernamePasswordAuthenticationToken) SecurityContextHolder.getContext().getAuthentication();
        UserDetailsImpl loginUser = (UserDetailsImpl) authenticationToken.getPrincipal();
        User user = loginUser.getUser();
        Integer userId = user.getId();

        QueryWrapper<Evaluate> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("id",evaluateId);

        List<Evaluate> list = evaluateMapper.selectList(queryWrapper);

        JSONObject jsonObject = new JSONObject();
        jsonObject.put("code",code);
        jsonObject.put("language",language);
        jsonObject.put("user_id",userId);
        jsonObject.put("list",list);
        jsonObject.put("evaluate_id",evaluateId);
        restTemplate.postForObject(evaluateUrl,jsonObject,String.class);
        System.out.println(userId + "add evaluate success");
        return "success";
    }
}
