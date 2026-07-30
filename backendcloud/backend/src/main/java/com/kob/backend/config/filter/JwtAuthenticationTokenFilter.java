package com.kob.backend.config.filter;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.kob.backend.mapper.UserMapper;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.utils.JwtUtil;
import io.jsonwebtoken.Claims;
import org.jetbrains.annotations.NotNull;
import org.springframework.beans.factory.annotation.Autowired;

import org.springframework.data.redis.core.StringRedisTemplate;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Component;
import org.springframework.util.StringUtils;
import org.springframework.web.filter.OncePerRequestFilter;

import javax.servlet.FilterChain;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import java.io.IOException;
import java.util.concurrent.TimeUnit;

@Component
public class JwtAuthenticationTokenFilter extends OncePerRequestFilter {
    @Autowired
    private UserMapper userMapper;

    @Autowired
    private StringRedisTemplate redisTemplate;

    @Autowired
    private ObjectMapper objectMapper;
    @Override
    protected void doFilterInternal(HttpServletRequest request, @NotNull HttpServletResponse response, @NotNull FilterChain filterChain) throws ServletException, IOException {
        String token = request.getHeader("Authorization");

        if (!StringUtils.hasText(token) || !token.startsWith("Bearer ")) {
            filterChain.doFilter(request, response);
            return;
        }

        token = token.substring(7);

        try {
            Claims claims = JwtUtil.parseJWT(token);
            String userid = claims.getSubject();

            User user = new User();
            if(!redisTemplate.hasKey(userid)) {
                user = userMapper.selectById(Integer.parseInt(userid));
                String userStr = objectMapper.writeValueAsString(user);
                redisTemplate.opsForValue().set("user:" + userid, userStr, 10, TimeUnit.MINUTES);
            }
            else{
                String userStr = redisTemplate.opsForValue().get("user:" + userid);
                user = objectMapper.readValue(userStr, User.class);
            }
            if (user != null) {
                UserDetailsImpl loginUser = new UserDetailsImpl(user);
                UsernamePasswordAuthenticationToken authenticationToken =
                        new UsernamePasswordAuthenticationToken(loginUser, null, null);

                SecurityContextHolder.getContext().setAuthentication(authenticationToken);
            }
        } catch (Exception e) {
            // token无效时不抛出异常，继续作为匿名用户处理
        }

        filterChain.doFilter(request, response);
    }
}

