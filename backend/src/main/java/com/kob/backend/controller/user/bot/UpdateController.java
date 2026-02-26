package com.kob.backend.controller.user.bot;

import com.kob.backend.pojo.Result;
import com.kob.backend.service.userInterface.Bot.UpdateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class UpdateController {
    @Autowired
    private UpdateService updateService;

    @PostMapping("/user/bot/update/")
    private Result update(@RequestParam Map<String,String> data){

        return Result.success(updateService.update(data));
    }
}
