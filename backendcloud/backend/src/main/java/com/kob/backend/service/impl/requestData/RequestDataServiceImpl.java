package com.kob.backend.service.impl.requestData;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.kob.backend.client.CppSandboxClient;
import com.kob.backend.consumer.WebSocketServer;
import com.kob.backend.mapper.EvaluateMapper;
import com.kob.backend.mapper.RecordOfQuestionMapper;
import com.kob.backend.mapper.RequestDataMapper;
import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.pojo.*;
import com.kob.backend.service.oj.requestData.RequestDataService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.*;

@Service
public class RequestDataServiceImpl implements RequestDataService {

    @Autowired
    private RequestDataMapper requestDataMapper;

    @Autowired
    private TopicMapper topicMapper;

    @Autowired
    private EvaluateMapper evaluateMapper;

    private Topic topic;
    @Autowired
    private CppSandboxClient cppSandboxClient;
    @Autowired
    private RecordOfQuestionMapper recordOfQuestionMapper;

    @Autowired
    private User user;


    public Result addRequestData(RequestData data) {
        if(data.getCode() == null || data.getCode().isEmpty())
            return Result.error("代码不能为空");

        Integer topicId;
        try{
            topicId = Integer.parseInt(data.getNumber());
        }catch (Exception e){
            return Result.error("题目编号格式错误");
        }

        Topic topic = topicMapper.selectById(topicId);
        if(topic == null){
            return Result.error("题目不存在");
        }

        if(!data.isSelfTest()){
            data.setInput(null);
        }
        requestDataMapper.addRequestData(data);

        // 获取测试用例
        List<Evaluate> tests = evaluateMapper.selectList(
                new QueryWrapper<Evaluate>().eq("topic_id", topicId)
        );
        if(tests == null || tests.isEmpty())
            return Result.error("题目测试用例获取失败");

        SandboxResult sandboxResult;
        try{
            sandboxResult = cppSandboxClient.compileAndRun(data);
        } catch (Exception e){
            return Result.error("测评服务暂时不可用，请稍后重试");
        }

        String state = getState(sandboxResult);

        if(!data.isSelfTest()){
            recordOfQuestionMapper.insert(new RecordOfQuestion(
                    null,
                    user.getId(),
                    topicId,
                    topic.getTitle(),
                    state,
                    60,
                    new Date()
            ));
        }

        try{
            WebSocketServer.startEvaluate(user.getId(), topicId, 60, state);
        } catch (Exception e){
            return Result.error("测评服务暂时不可用，请稍后重试");
        }

        Map<String, Object> resultBody = new LinkedHashMap<>();
        resultBody.put("state", state);
        resultBody.put("score", 60);
        resultBody.put("stdout", sandboxResult != null ? sandboxResult.getStdout() : "");
        resultBody.put("reason", sandboxResult != null ? sandboxResult.getReason() : "系统异常");



        return Result.success(resultBody);
    }

    private String getState(SandboxResult sandboxResult) {
        if(sandboxResult == null){
            return "SYSTEM_ERROR";
        }

        int status = sandboxResult.getStatus();

        switch (status){
            case 0:
                return "Accepted";
            case -3:
                return "Compile Error";
            case 24:
                return "Time Limit Exceeded";
            case 6:
                return "Memory Limit Exceeded";
            default:
                return "Runtime Error";
        }
    }


}
