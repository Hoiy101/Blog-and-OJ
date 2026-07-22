package com.kob.backend.controller.manage;

import com.kob.backend.pojo.User;
import com.kob.backend.service.manage.GitUserListService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

@RestController
public class GitUserListController {
    @Autowired
    private GitUserListService gitUserListService;

    @GetMapping("/manage/user/gitlist/")
    private List<HashMap<String, String>> getuserlist(){
        return gitUserListService.getuserlist();
    }
}
