package com.kob.backend.controller.user.account;

import com.kob.backend.pojo.Result;
import com.kob.backend.service.userInterface.account.InfoService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class InfoController {
    @Autowired
    private InfoService infoService;

    @GetMapping("/user/account/info/")
    public Result getinfo(){
        return Result.success(infoService.getinfo());
    }

}
