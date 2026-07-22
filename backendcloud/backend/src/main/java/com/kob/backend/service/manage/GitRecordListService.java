package com.kob.backend.service.manage;

import com.kob.backend.pojo.LoginRecord;

import java.util.List;

public interface GitRecordListService {
    List<LoginRecord> getGitRecordList();
}
