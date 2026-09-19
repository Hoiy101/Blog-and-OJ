package com.bao.backend.mapper;


import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.bao.backend.pojo.Blog;
import org.apache.ibatis.annotations.Mapper;

@Mapper
public interface BlogMapper extends BaseMapper<Blog> {

}
