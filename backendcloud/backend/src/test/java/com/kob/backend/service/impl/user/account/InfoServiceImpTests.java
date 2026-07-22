package com.kob.backend.service.impl.user.account;

import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;

import java.util.Map;

import static org.junit.jupiter.api.Assertions.assertEquals;

class InfoServiceImpTests {

    @AfterEach
    void tearDown() {
        SecurityContextHolder.clearContext();
    }

    @Test
    void returnsAdministratorFlagForAuthenticatedUser() {
        User user = new User(7, "admin", "password", "photo", "true", "false");
        SecurityContextHolder.getContext().setAuthentication(
                new UsernamePasswordAuthenticationToken(new UserDetailsImpl(user), null));

        Map<String, String> response = new InfoServiceImp().getinfo();

        assertEquals("success", response.get("error_message"));
        assertEquals("true", response.get("root"));
    }
}
