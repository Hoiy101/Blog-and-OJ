package com.kob.backend.pojo;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class RequestData {
    private String number;
    private String code;
    private String language;
    private String input;
    private boolean isSelfTest;
    private Integer cpu_limit;
    private Integer mem_limit;
    private Integer test_count;
}
