package com.kob.backend.mapper;


import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.kob.backend.pojo.Question;
import com.kob.backend.pojo.RecordOfQuestion;
import com.kob.backend.pojo.User;
import org.apache.ibatis.annotations.Insert;
import org.apache.ibatis.annotations.Mapper;

import java.util.List;

@Mapper
public interface UserMapper extends BaseMapper<User> {


    List<Question> getRecord(Integer userId);

    @Insert("insert into record_of_question (user_id,question_id,score) values (#{userId},#{questionId},#{questionScore})")
    void save(RecordOfQuestion record);


    void updateRecord(RecordOfQuestion record);
}
