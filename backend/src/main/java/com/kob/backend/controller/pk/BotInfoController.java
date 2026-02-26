package com.kob.backend.controller.pk;

import com.kob.backend.pojo.Result;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;

@RestController
@RequestMapping("/pk")
public class BotInfoController {

    @RequestMapping("/getbotinfo/")
    public Result getBotInfo(){
        List<String> list = new LinkedList<>();
        list.add("sword");
        list.add("tiger");
        return Result.success(list);
    }
}
