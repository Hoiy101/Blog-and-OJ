package com.kob.backend.service.impl.manage.topic;

import com.kob.backend.mapper.EvaluateMapper;
import com.kob.backend.pojo.Evaluate;
import com.kob.backend.service.manage.topic.UpdataEvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

@Service
public class UpdataEvaluateServiceImpl implements UpdataEvaluateService {
    @Autowired
    private EvaluateMapper evaluateMapper;

    @Override
    public Map<String, String> updataevaluate(List<Map<String, String>> data) {
        HashMap<String,String> message = new HashMap<>();
        for (Map<String, String> map : data) {
            if(map.get("topic_id") == null || Objects.equals(map.get("topic_id"), "")){
                message.put("error_message", "题号不能为空");
                return message;
            }
            if(map.get("input") ==  null || Objects.equals(map.get("input"), "")){
                message.put("error_message", "输入不能为空");
                return message;
            }
            if(map.get("output") ==  null || Objects.equals(map.get("output"), "")){
                message.put("error_message", "输出不能为空");
                return message;
            }
        }

        for (Map<String, String> map : data) {
            Integer id = null;
            if(map.get("id") != null) id = Integer.parseInt(map.get("id"));
            Evaluate evaluate = new Evaluate(
                    id,
                    Integer.parseInt(map.get("topic_id")),
                    map.get("input"),
                    map.get("output")
            );
            if(map.get("id") == null){
                evaluateMapper.insert(evaluate);
            }
            else{
                evaluateMapper.updateById(evaluate);
            }
        }
        message.put("error_message", "success");
        return message;
    }
}
