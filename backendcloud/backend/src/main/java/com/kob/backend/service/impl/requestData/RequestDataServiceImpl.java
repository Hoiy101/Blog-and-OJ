package com.kob.backend.service.impl.requestData;

import com.kob.backend.mapper.RequestDataMapper;
import com.kob.backend.pojo.RequestData;
import com.kob.backend.pojo.Result;
import com.kob.backend.service.oj.requestData.RequestDataService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class RequestDataServiceImpl implements RequestDataService {

    @Autowired
    private RequestDataMapper requestDataMapper;

    public void addRequestData(RequestData data) {
        if(data.is_self_text()){
            data.setInput(null);
        }
        requestDataMapper.addRequestData(data);
    }
}
