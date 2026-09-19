package com.bao.backend.service.impl.manage;

import com.bao.backend.service.manage.CircuitBreakerService;
import io.github.resilience4j.circuitbreaker.CircuitBreaker;
import io.github.resilience4j.circuitbreaker.CircuitBreakerConfig;
import io.github.resilience4j.circuitbreaker.CircuitBreakerRegistry;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/**
 * 只读地暴露 Resilience4j 熔断器的实时状态，给后台管理页的运行状态卡片用。
 * 熔断器是第一次调用时才注册进来的，所以调用发生之前这里会返回空列表。
 */
@Service
public class CircuitBreakerServiceImpl implements CircuitBreakerService {

    @Autowired
    private CircuitBreakerRegistry circuitBreakerRegistry;

    @Override
    public List<Map<String, Object>> getcircuitbreakers() {
        List<Map<String, Object>> list = new ArrayList<>();
        for (CircuitBreaker circuitBreaker : circuitBreakerRegistry.getAllCircuitBreakers()) {
            CircuitBreaker.Metrics metrics = circuitBreaker.getMetrics();
            CircuitBreakerConfig config = circuitBreaker.getCircuitBreakerConfig();

            Map<String, Object> map = new LinkedHashMap<>();
            map.put("name", circuitBreaker.getName());
            map.put("state", circuitBreaker.getState().name());
            // 窗口内调用数不足 minimumNumberOfCalls 时失败率是 -1，前端按“统计中”显示
            map.put("failure_rate", metrics.getFailureRate());
            map.put("slow_call_rate", metrics.getSlowCallRate());
            map.put("buffered_calls", metrics.getNumberOfBufferedCalls());
            map.put("successful_calls", metrics.getNumberOfSuccessfulCalls());
            map.put("failed_calls", metrics.getNumberOfFailedCalls());
            map.put("not_permitted_calls", metrics.getNumberOfNotPermittedCalls());
            map.put("sliding_window_type", config.getSlidingWindowType().name());
            map.put("sliding_window_size", config.getSlidingWindowSize());
            map.put("minimum_number_of_calls", config.getMinimumNumberOfCalls());
            map.put("failure_rate_threshold", config.getFailureRateThreshold());
            map.put("wait_duration_in_open_state", config.getWaitDurationInOpenState().getSeconds());
            map.put("permitted_calls_in_half_open_state", config.getPermittedNumberOfCallsInHalfOpenState());
            list.add(map);
        }
        return list;
    }
}
