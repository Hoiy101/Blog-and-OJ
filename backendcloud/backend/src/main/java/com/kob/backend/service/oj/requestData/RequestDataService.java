package com.kob.backend.service.oj.requestData;

import com.kob.backend.pojo.RequestData;
import com.kob.backend.pojo.Result;
import org.springframework.stereotype.Service;

@Service
public interface RequestDataService {

    void addRequestData(RequestData requestData);
}
