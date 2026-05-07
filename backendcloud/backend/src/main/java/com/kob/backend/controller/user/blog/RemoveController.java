package com.kob.backend.controller.user.blog;


import com.kob.backend.service.user.blog.RemoveService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class RemoveController {
    @Autowired
    private RemoveService removeService;

    @PostMapping("/user/bot/remove/")
    private Map<String,String> remove(@RequestParam Map<String,String> data){
        return removeService.remove(data);
    }
}
