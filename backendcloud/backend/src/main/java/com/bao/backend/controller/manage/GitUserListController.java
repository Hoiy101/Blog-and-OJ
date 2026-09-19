package com.bao.backend.controller.manage;

import com.bao.backend.service.manage.GitUserListService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.HashMap;
import java.util.List;

@RestController
public class GitUserListController {
    @Autowired
    private GitUserListService gitUserListService;

    @GetMapping("/manage/user/gitlist/")
    private List<HashMap<String, String>> getuserlist(){
        return gitUserListService.getuserlist();
    }
}
