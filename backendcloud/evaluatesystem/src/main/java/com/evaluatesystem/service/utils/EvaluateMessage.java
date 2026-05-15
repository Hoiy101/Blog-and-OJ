package com.evaluatesystem.service.utils;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class EvaluateMessage {
    private Integer exitValue;
    private String message;
    private String errorMessage;
    private Long time;
    private Long memory;
}
