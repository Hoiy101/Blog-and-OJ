package com.evaluatesystem;


import com.evaluatesystem.service.Impl.EvaluateServiceImpl;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class EvaluateSystemApplication {
    public static void main(String[] args) {
        EvaluateServiceImpl.evaluatePool.start();
        SpringApplication.run(EvaluateSystemApplication.class, args);
    }
}