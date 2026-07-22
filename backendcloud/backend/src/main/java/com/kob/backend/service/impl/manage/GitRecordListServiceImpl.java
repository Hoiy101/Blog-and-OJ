package com.kob.backend.service.impl.manage;

import com.kob.backend.mapper.LoginRecordMapper;
import com.kob.backend.pojo.LoginRecord;
import com.kob.backend.service.manage.GitRecordListService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class GitRecordListServiceImpl implements GitRecordListService {

    @Autowired
    private LoginRecordMapper loginRecordMapper;

    @Override
    public List<LoginRecord> getGitRecordList() {
        return loginRecordMapper.selectList(null);
    }
}
