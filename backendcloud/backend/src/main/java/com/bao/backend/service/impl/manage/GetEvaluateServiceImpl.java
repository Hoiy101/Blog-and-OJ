package com.bao.backend.service.impl.manage;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.bao.backend.mapper.EvaluateMapper;
import com.bao.backend.pojo.Evaluate;
import com.bao.backend.service.manage.GetEvaluateService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class GetEvaluateServiceImpl implements GetEvaluateService {
    @Autowired
    private EvaluateMapper evaluateMapper;

    @Override
    public List<Evaluate> getEvaluateList(Integer topic_id) {
        QueryWrapper<Evaluate> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("topic_id",topic_id);
        return evaluateMapper.selectList(queryWrapper);
    }
}
