package com.bao.backend.service.impl.manage;

import com.bao.backend.pojo.JudgeStatus;
import com.bao.backend.service.manage.StatusService;
import com.bao.backend.utils.EvaluateSystemClient;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.Map;

@Service
public class StatusServiceImpl implements StatusService{

    @Autowired
    private EvaluateSystemClient evaluateSystemClient;

    @Override
    public Map<String, Object> getstatus() {
        JudgeStatus judgeStatus = evaluateSystemClient.status();
        Map<String, Object> map = new HashMap<>();
        map.put("online", judgeStatus.getOnline());
        map.put("queues", judgeStatus.getQueues());
        map.put("running", judgeStatus.getRunning());
        map.put("message", judgeStatus.getMessage());
        return map;
    }

}
