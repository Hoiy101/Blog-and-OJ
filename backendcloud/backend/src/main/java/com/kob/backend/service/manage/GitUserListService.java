package com.kob.backend.service.manage;

import com.kob.backend.pojo.User;

import java.util.HashMap;
import java.util.List;

public interface GitUserListService {
    List<HashMap<String, String>> getuserlist();
}
