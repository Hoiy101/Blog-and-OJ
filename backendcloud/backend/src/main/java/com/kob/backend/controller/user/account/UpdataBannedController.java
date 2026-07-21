package com.kob.backend.controller.user.account;

import com.kob.backend.service.user.account.UpdataBannedService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class UpdataBannedController {
    @Autowired
    private UpdataBannedService updataBannedService;

    @GetMapping("/user/account/banned/")
    public Map<String, String> updataBanned(@RequestParam Map<String,String> map){
        String username = map.get("username");
        Integer status = Integer.parseInt(map.get("status"));
        return updataBannedService.updataBanned(username, status);
    }
}
