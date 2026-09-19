package com.bao.backend.controller.manage;

import com.bao.backend.pojo.Evaluate;
import com.bao.backend.service.manage.GetEvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;
import java.util.Map;

@RestController
public class GetEvaluateController {
    @Autowired
    private GetEvaluateService getEvaluateService;

    @PostMapping("/manage/evaluate/get/")
    private List<Evaluate> getEvaluateList(@RequestParam Map<String,String> map){
        Integer topic_id = Integer.parseInt(map.get("topic_id"));
        return getEvaluateService.getEvaluateList(topic_id);
    }


}
