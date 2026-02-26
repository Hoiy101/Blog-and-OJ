package com.kob.backend.service.userInterface.account;

import java.util.Map;

public interface LoginService {
    Map<String, String> getToken(String username, String password);
}
