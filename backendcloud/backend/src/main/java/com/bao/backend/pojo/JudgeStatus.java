package com.bao.backend.pojo;

import lombok.Data;

@Data
public class JudgeStatus {
    private Boolean online;
    private Integer queues;
    private Integer running;
    private String message;

}
