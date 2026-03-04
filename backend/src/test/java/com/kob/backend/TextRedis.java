package com.kob.backend;

import org.junit.jupiter.api.Test;
import redis.clients.jedis.Jedis;

import java.util.HashMap;
import java.util.Map;

public class TextRedis {

    @Test
    public void testRedis() {
        Jedis jedis = new Jedis("127.0.0.1", 6379, 60000);
        jedis.set("name", "zhangsan");
        jedis.set("age", "18");
        Map<String, String> map = new HashMap<>();
        map.put("name", "lisi");
        map.put("age", "19");
        System.out.println(jedis);
        jedis.close();
    }
}
