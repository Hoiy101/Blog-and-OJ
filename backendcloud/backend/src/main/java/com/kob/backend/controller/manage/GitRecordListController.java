package com.kob.backend.controller.manage;


import com.kob.backend.pojo.LoginRecord;
import com.kob.backend.service.manage.GitRecordListService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
public class GitRecordListController {

    @Autowired
    private GitRecordListService gitRecordListService;

    @GetMapping("/manage/record/login/")
    private List<LoginRecord> gitRecordList() {
        return gitRecordListService.getGitRecordList();
    }
}
