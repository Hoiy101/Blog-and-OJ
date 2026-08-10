package com.kob.backend;

import com.kob.backend.utils.BloomInitializer;
import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.security.crypto.bcrypt.BCryptPasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.springframework.web.socket.server.standard.ServerEndpointExporter;

@SpringBootTest
class BackendApplicationTests {

    @MockBean
    private ServerEndpointExporter serverEndpointExporter;

    @MockBean
    private BloomInitializer bloomInitializer;

    @Test
    void contextLoads() {
    }

}
