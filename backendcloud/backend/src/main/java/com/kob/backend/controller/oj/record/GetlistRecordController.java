package com.kob.backend.controller.oj.record;

import com.kob.backend.pojo.RecordOfQuestion;
import com.kob.backend.service.oj.record.GetListRecordService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
public class GetlistRecordController {
    @Autowired
    private GetListRecordService getListRecordService;

    @GetMapping("/oj/record/getlist/")
    private List<RecordOfQuestion> getlist(){
        return getListRecordService.getList();
    }
}
