package com.bao.backend.controller.blog;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.bao.backend.service.blog.UpdateService;
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
    private Map<String,String> update(@RequestParam Map<String,String> data) throws JsonProcessingException {
        return updateService.update(data);
    }
}
