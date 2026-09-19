package com.bao.backend.controller.manage;


import com.bao.backend.pojo.LoginRecord;
import com.bao.backend.service.manage.GitRecordListService;
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
