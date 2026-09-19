package com.bao.backend.controller.manage;

import com.bao.backend.service.manage.StatusService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;
import java.util.Objects;

@RestController
public class StatusController {
    @Autowired
    private StatusService statusService;

    @GetMapping("/manage/judge/status/")
    public Map<String, Object> getJudgeStatus() {
        return statusService.getstatus();
    }
}
