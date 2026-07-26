package com.kob.backend.client;

import com.kob.backend.pojo.RequestData;
import com.kob.backend.pojo.SandboxResult;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.HttpEntity;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Component;
import org.springframework.web.client.RestTemplate;

import org.springframework.http.HttpHeaders;

@Component
public class CppSandboxClient {
    @Autowired
    private RestTemplate restTemplate;

    @Value("${cpp.sandbox.url:http://127.0.0.1:8081}")
    private String sandboxBaseUrl;

    public SandboxResult compileAndRun(RequestData request){
        String url = sandboxBaseUrl + "/compile_and_run";

        //设置请求头
        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_JSON);

        //将请求数据和请求头封装成一个HTTP请求实体
        HttpEntity<RequestData> requestEntity = new HttpEntity<>(request, headers);

        //发送HTTP请求，并指定返回值的类型为SandboxResult
        ResponseEntity<SandboxResult> response = restTemplate.postForEntity(
                url, requestEntity, SandboxResult.class);

        return response.getBody();
    }
}
