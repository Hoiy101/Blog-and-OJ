package com.bao.backend.utils;

import com.bao.backend.pojo.JudgeStatus;
import io.github.resilience4j.circuitbreaker.CallNotPermittedException;
import org.springframework.cloud.openfeign.FallbackFactory;
import org.springframework.cloud.openfeign.FeignClient;
import org.springframework.stereotype.Component;
import org.springframework.web.bind.annotation.GetMapping;

@FeignClient(name = "evaluatesystem",  fallbackFactory = EvaluateSystemClient.EvaluateSystemFallbackFactory.class)
public interface EvaluateSystemClient {

    @GetMapping("/status/")
    JudgeStatus status();

    @Component
    class EvaluateSystemFallbackFactory implements FallbackFactory<EvaluateSystemClient> {
        @Override
        public EvaluateSystemClient create(Throwable cause) {
            return () -> {
                JudgeStatus judgeStatus = new JudgeStatus();
                judgeStatus.setOnline(false);
                judgeStatus.setMessage(cause instanceof CallNotPermittedException
                        ? "判题服务离线（熔断中，稍后自动重试）"     // 熔断器 OPEN，请求根本没发出去
                        : "判题服务无响应：" + cause.getClass().getSimpleName());
                return judgeStatus;
            };
        }
    }
}
