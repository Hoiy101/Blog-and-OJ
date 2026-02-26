package com.kob.backend;

import org.junit.jupiter.api.Test;
import redis.clients.jedis.Jedis;

public class TextRedis {

    @Test
    public void testRedis() {
        Jedis jedis = new Jedis("127.0.0.1", 6379, 60000);
        jedis.set("name", "zhangsan");
        jedis.set("age", "18");
        System.out.println(jedis.get("name") + " " + jedis.get("age"));
    }
}
