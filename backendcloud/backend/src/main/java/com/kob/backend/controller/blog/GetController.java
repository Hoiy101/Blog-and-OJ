package com.kob.backend.controller.blog;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.kob.backend.service.blog.GetService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import java.util.Map;

@RestController
public class GetController {
    @Autowired
    private GetService getService;

    @GetMapping("/user/bot/get/")
    public Map<String,String> getUser(@RequestParam Integer id) throws JsonProcessingException {
        return getService.get(id);
    }
}
