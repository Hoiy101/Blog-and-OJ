package com.kob.backend.controller.user.bot;

import com.kob.backend.pojo.Result;
import com.kob.backend.service.userInterface.Bot.AddService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class AddController {
    @Autowired
    private AddService addService;

    @PostMapping("/user/bot/add/")
    private Result add(@RequestParam Map<String,String> data){
        return Result.success(addService.add(data));
    }
}
