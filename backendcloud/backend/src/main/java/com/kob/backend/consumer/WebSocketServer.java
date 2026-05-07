package com.kob.backend.consumer;

import com.alibaba.fastjson2.JSONObject;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.kob.backend.mapper.EvaluateMapper;
import com.kob.backend.mapper.UserMapper;
import com.kob.backend.pojo.Evaluate;
import com.kob.backend.pojo.User;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import javax.websocket.*;
import javax.websocket.server.PathParam;
import javax.websocket.server.ServerEndpoint;
import java.io.IOException;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

@Component
@ServerEndpoint("/websocket/{token}")  // 注意不要以'/'结尾
public class WebSocketServer {

    private static UserMapper userMapper;

    private static EvaluateMapper evaluateMapper;

    private static ConcurrentHashMap<Integer, WebSocketServer> users = new ConcurrentHashMap<>();

    private User user;

    private Session session = null;

    @Autowired
    public void setusermapper (UserMapper userMapper) {
        WebSocketServer.userMapper = userMapper;
    }

    @Autowired
    public void setevaluatemapper(EvaluateMapper evaluateMapper) {
        WebSocketServer.evaluateMapper = evaluateMapper;
    }

    @OnOpen
    public void onOpen(Session session, @PathParam("token") String token) {
        this.session = session;
        Integer user_id = Integer.parseInt(token);

        this.user = userMapper.selectById(user_id);
        System.out.println(user_id + "add websocket");
        if(this.user != null) {
            users.put(user_id, this);

        }
    }

    @OnClose
    public void onClose() {
        System.out.println("remove websocket");
        if(this.user != null) {
            users.remove(this.user.getId());
        }
    }

    @OnMessage
    public void onMessage(String message, Session session) {
        // 从Client接收消息
    }

    @OnError
    public void onError(Session session, Throwable error) {
        error.printStackTrace();
    }

    public void sendMessage(String message) {
        synchronized (this.session) {
            try {
                this.session.getBasicRemote().sendText(message);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }

        }
    }

    public static void startEvaluate(Integer user_id, Integer evaluation_id, Integer score) {
        JSONObject jsonObject = new JSONObject();
        jsonObject.put("user_id", user_id);
        jsonObject.put("evaluation_id", evaluation_id);
        jsonObject.put("score", score);
        System.out.println(jsonObject);
        users.get(user_id).sendMessage(jsonObject.toJSONString());
    }
}
