package com.kob.backend.controller.user.blog;

import com.kob.backend.service.user.account.LoginService;
import com.kob.backend.service.user.blog.GetService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class GetController {
    @Autowired
    private GetService getService;

    @GetMapping("/user/bot/get/")
    public Map<String,String> getUser(@RequestParam Integer id){
        return getService.get(id);
    }
}
