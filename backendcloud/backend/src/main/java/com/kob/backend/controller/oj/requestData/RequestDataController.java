package com.kob.backend.controller.oj.requestData;

import com.kob.backend.pojo.RequestData;
import com.kob.backend.pojo.Result;
import com.kob.backend.service.oj.requestData.RequestDataService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RestController;

@Slf4j
@RestController
public class RequestDataController {

    @Autowired
    private RequestDataService requestDataService;

    @PostMapping("/compile_and_run")
    public Result addRequestData(RequestData data) {
        requestDataService.addRequestData(data);
        return Result.success();
    }
}
