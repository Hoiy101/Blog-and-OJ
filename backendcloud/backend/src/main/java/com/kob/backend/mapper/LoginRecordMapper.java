package com.kob.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.kob.backend.pojo.LoginRecord;
import org.apache.ibatis.annotations.Mapper;

@Mapper
public interface LoginRecordMapper extends BaseMapper<LoginRecord> {
}
