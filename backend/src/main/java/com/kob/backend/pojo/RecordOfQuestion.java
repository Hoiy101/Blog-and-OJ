package com.kob.backend.pojo;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@NoArgsConstructor
@AllArgsConstructor
public class RecordOfQuestion {
    private Integer id;
    private Integer userId;
    private Integer questionId;
    private String score;
}
