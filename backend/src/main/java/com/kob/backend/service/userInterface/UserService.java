package com.kob.backend.service.userInterface;

import com.kob.backend.pojo.Question;
import com.kob.backend.pojo.RecordOfQuestion;

import java.util.List;

public interface UserService {
    List<Question> getRecord(Integer userId);

    void addRecord(RecordOfQuestion record);

    void update(RecordOfQuestion record);
}
