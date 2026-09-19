package com.bao.backend.controller.manage;

import com.bao.backend.service.manage.CircuitBreakerService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;
import java.util.Map;

@RestController
public class CircuitBreakerController {
    @Autowired
    private CircuitBreakerService circuitBreakerService;

    @GetMapping("/manage/judge/circuitbreaker/")
    public List<Map<String, Object>> getCircuitBreakers() {
        return circuitBreakerService.getcircuitbreakers();
    }
}
