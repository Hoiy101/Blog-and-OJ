package com.kob.backend.service.impl.user.blog;

import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.pojo.Blog;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.user.blog.storage.BlogImageStorage;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;

import java.util.Collections;
import java.util.Date;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.mockito.Mockito.*;

class RemoveServiceImplTests {
    private BlogMapper mapper;
    private BlogImageStorage storage;

    @BeforeEach
    void setUp() {
        mapper = mock(BlogMapper.class);
        storage = mock(BlogImageStorage.class);
        User user = new User(7, "tester", "password", "photo", "user");
        SecurityContextHolder.getContext().setAuthentication(
                new UsernamePasswordAuthenticationToken(new UserDetailsImpl(user), null));
    }

    @AfterEach
    void tearDown() {
        SecurityContextHolder.clearContext();
    }

    @Test
    void deletesDatabaseBeforeCleaningImagePrefix() throws Exception {
        Blog blog = new Blog(12, 7, "t", "d", "c", new Date(), new Date());
        when(mapper.selectById(12)).thenReturn(blog);

        assertEquals("success", new RemoveServiceImpl(mapper, storage)
                .remove(Collections.singletonMap("bot_id", "12")).get("error_message"));

        org.mockito.InOrder order = inOrder(mapper, storage);
        order.verify(mapper).deleteById(12);
        order.verify(storage).deletePrefix("blog-images/7/12/");
    }

    @Test
    void cleanupFailureDoesNotUndoSuccessfulDelete() throws Exception {
        Blog blog = new Blog(12, 7, "t", "d", "c", new Date(), new Date());
        when(mapper.selectById(12)).thenReturn(blog);
        doThrow(new RuntimeException("minio unavailable")).when(storage).deletePrefix(anyString());

        assertEquals("success", new RemoveServiceImpl(mapper, storage)
                .remove(Collections.singletonMap("bot_id", "12")).get("error_message"));
        verify(mapper).deleteById(12);
    }
}
