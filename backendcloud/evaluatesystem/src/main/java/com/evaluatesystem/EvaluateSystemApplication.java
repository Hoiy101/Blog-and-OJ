package com.evaluatesystem;


import com.evaluatesystem.service.utils.EvaluatePool;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;

@SpringBootApplication
public class EvaluateSystemApplication {
    public static void main(String[] args) {
        SpringApplication.run(EvaluateSystemApplication.class, args);
    }
}