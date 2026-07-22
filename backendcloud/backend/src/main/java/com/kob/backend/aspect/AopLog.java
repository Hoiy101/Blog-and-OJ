package com.kob.backend.aspect;


import com.baomidou.mybatisplus.core.mapper.Mapper;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.kob.backend.mapper.LoginRecordMapper;
import com.kob.backend.pojo.LoginRecord;
import org.aspectj.lang.ProceedingJoinPoint;
import org.aspectj.lang.annotation.Around;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.annotation.Pointcut;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.web.context.request.RequestContextHolder;
import org.springframework.web.context.request.ServletRequestAttributes;

import javax.servlet.http.HttpServletRequest;
import java.util.Date;
import java.util.Map;

@Aspect
@Component
public class AopLog {
    @Autowired
    private LoginRecordMapper loginRecordMapper;

    @Pointcut(value = "execution(* com.kob.backend.controller.user.account.*.*(..))")
    public void aopWebLog(){

    }

    @Around("aopWebLog()")
    public Object logAop(ProceedingJoinPoint pjp) throws Throwable{
        ServletRequestAttributes attributes = (ServletRequestAttributes) RequestContextHolder.getRequestAttributes();
        HttpServletRequest request = attributes.getRequest();
        ObjectMapper mapper = new ObjectMapper();
        Object[] args = pjp.getArgs();
        Object result = pjp.proceed();
        String methodName = pjp.getSignature().getName();
        LoginRecord loginRecord = new LoginRecord();
        if(methodName.equals("getinfo")) {
            if(result instanceof Map) {
                Map<String, String> map_input = (Map<String, String>) result;
                loginRecord = new LoginRecord(null, map_input.get("username"), request.getRemoteAddr(), new Date());
            }
            loginRecordMapper.insert(loginRecord);
        }
        return result;
    }
}
