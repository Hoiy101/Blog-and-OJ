package com.kob.backend.controller.oj.record;


import com.kob.backend.service.oj.record.AddRecordService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class AddRecordController {
    @Autowired
    AddRecordService addRecordService;

    @PostMapping("/oj/record/add/")
    private Map<String, String> add(@RequestParam Map<String, String> data){
        return addRecordService.add(data);
    }

}
