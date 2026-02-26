package com.kob.backend.controller.user.bot;

import com.kob.backend.pojo.Result;
import com.kob.backend.service.userInterface.Bot.GetListService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class GetListController {
    @Autowired
    private GetListService getListService;

    @GetMapping("/user/bot/getlist/")
    public Result getList(){
        return Result.success(getListService.getList());
    }
}
