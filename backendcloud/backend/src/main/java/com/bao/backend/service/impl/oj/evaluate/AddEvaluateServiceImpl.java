package com.bao.backend.service.impl.oj.evaluate;

import com.alibaba.fastjson2.JSONObject;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.bao.backend.mapper.EvaluateMapper;
import com.bao.backend.pojo.Evaluate;
import com.bao.backend.pojo.User;
import com.bao.backend.producer.OjRabbitmq;
import com.bao.backend.service.impl.utils.UserDetailsImpl;
import com.bao.backend.service.oj.evaluate.AddEvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

import java.util.*;

@Service
public class AddEvaluateServiceImpl implements AddEvaluateService {
    @Autowired
    private EvaluateMapper evaluateMapper;
    @Autowired
    private OjRabbitmq ojRabbitmq;

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
        queryWrapper.eq("topic_id",evaluateId);

        List<Evaluate> list = evaluateMapper.selectList(queryWrapper);

        List<String> inputList = new ArrayList<>();
        List<String> outputList = new ArrayList<>();
        for(Evaluate evaluate : list){
            inputList.add(evaluate.getInput());
            outputList.add(evaluate.getOutput());
        }
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("code",code);
        jsonObject.put("language",language);
        jsonObject.put("user_id",userId);
        jsonObject.put("inputList",inputList);
        jsonObject.put("outputList",outputList);
        jsonObject.put("evaluate_id",evaluateId);
        ojRabbitmq.stateMessage(jsonObject);
        System.out.println(userId + "add evaluate success");
        return "success";
    }
}
