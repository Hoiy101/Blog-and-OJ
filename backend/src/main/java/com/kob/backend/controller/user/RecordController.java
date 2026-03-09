package com.kob.backend.controller.user;


import com.kob.backend.pojo.Question;
import com.kob.backend.pojo.RecordOfQuestion;
import com.kob.backend.service.userInterface.UserService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.List;

@Slf4j
@RequestMapping("/record")
@RestController
public class RecordController {

    @Autowired
    private UserService userService;

    @GetMapping("/{Userid}")
    public List<Question> getRecord(@PathVariable Integer Userid) {
        log.info("获取记录");
        return userService.getRecord(Userid);
    }

    @PostMapping
    public String addRecord(@RequestBody RecordOfQuestion record) {
        log.info("添加记录");
        userService.addRecord(record);
        return "success";
    }

    @PutMapping
    public String updateRecord(@RequestBody RecordOfQuestion record) {
        log.info("更新记录");
        userService.update(record);
        return "success";
    }
}
