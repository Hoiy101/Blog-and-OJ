package com.kob.backend.pojo;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;

@Data
@AllArgsConstructor
@NoArgsConstructor
public class Topic {
    @TableId(type = IdType.AUTO)
    Integer id;
    Integer testPoint;
    String title;
    String description;
    String star;
    Integer timeLimit;
    Integer memLimit;
}
