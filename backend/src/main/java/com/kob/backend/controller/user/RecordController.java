package com.kob.backend.controller.user;


import com.kob.backend.pojo.Question;
import com.kob.backend.pojo.RecordOfQuestion;
import com.kob.backend.pojo.Result;
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
    public Result getRecord(@PathVariable Integer Userid) {
        log.info("获取记录");
        List<Question> list = userService.getRecord(Userid);
        return Result.success(list);
    }

    @PostMapping
    public Result addRecord(@RequestBody RecordOfQuestion record) {
        log.info("添加记录");
        userService.addRecord(record);
        return Result.success();
    }

    @PutMapping
    public Result updateRecord(@RequestBody RecordOfQuestion record) {
        log.info("更新记录");
        userService.update(record);
        return Result.success();
    }
}
