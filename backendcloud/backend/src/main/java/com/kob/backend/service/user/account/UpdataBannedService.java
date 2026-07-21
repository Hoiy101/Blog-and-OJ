package com.kob.backend.service.user.account;

import java.util.Map;

public interface UpdataBannedService {
    Map<String, String> updataBanned(String username, Integer status);
}
