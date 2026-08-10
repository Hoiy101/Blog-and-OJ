package com.kob.backend.service.impl.blog;

import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.pojo.Blog;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.service.blog.RemoveService;
import com.kob.backend.service.impl.blog.storage.BlogImageStorage;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.HashMap;
import java.util.Map;

@Service
public class RemoveServiceImpl implements RemoveService {
    private static final Logger LOGGER = LoggerFactory.getLogger(RemoveServiceImpl.class);
    private final BlogMapper blogMapper;
    private final BlogImageStorage blogImageStorage;

    @Autowired
    public RemoveServiceImpl(BlogMapper blogMapper, BlogImageStorage blogImageStorage) {
        this.blogMapper = blogMapper;
        this.blogImageStorage = blogImageStorage;
    }

    @Override
    public Map<String, String> remove(Map<String, String> data) {
        UsernamePasswordAuthenticationToken authenticationToken =
                (UsernamePasswordAuthenticationToken) SecurityContextHolder.getContext().getAuthentication();
        UserDetailsImpl loginUser = (UserDetailsImpl) authenticationToken.getPrincipal();
        User user = loginUser.getUser();

        int bot_id = Integer.parseInt(data.get("bot_id"));
        Blog blog = blogMapper.selectById(bot_id);

        Map<String,String> map = new HashMap<>();

        if(blog == null){
            map.put("error_message", "bot不存在或已被删除");
            return map;
        }
        if(!blog.getUserId().equals(user.getId())){
            map.put("error_message", "没有权限删除此bot");
            return map;
        }

        blogMapper.deleteById(bot_id);
        try {
            blogImageStorage.deletePrefix("blog-images/" + user.getId() + "/" + bot_id + "/");
        } catch (Exception cleanupError) {
            LOGGER.warn("博客 {} 已删除，但 MinIO 图片清理失败", bot_id, cleanupError);
        }
        map.put("error_message", "success");

        return map;
    }
}
