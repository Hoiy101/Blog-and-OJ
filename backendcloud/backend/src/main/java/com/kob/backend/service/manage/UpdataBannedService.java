package com.kob.backend.service.manage;

import java.util.Map;

public interface UpdataBannedService {
    Map<String, String> updataBanned(String username, Integer status);
}
