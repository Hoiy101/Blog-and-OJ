package com.kob.backend.mapper;

import com.kob.backend.pojo.RequestData;
import org.apache.ibatis.annotations.Insert;
import org.apache.ibatis.annotations.Mapper;

@Mapper
public interface RequestDataMapper {

    @Insert("INSERT INTO requestData (number, code, language, input, is_self_test) VALUES (#{number}, #{code}, #{language}, #{input}, #{isSelfTest})")
    void addRequestData(RequestData requestData);
}
