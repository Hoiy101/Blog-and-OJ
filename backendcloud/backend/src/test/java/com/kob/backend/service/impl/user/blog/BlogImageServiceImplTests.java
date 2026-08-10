package com.kob.backend.service.impl.user.blog;

import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.pojo.Blog;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.blog.BlogImageServiceImpl;
import com.kob.backend.service.impl.blog.storage.BlogImageStorage;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.springframework.mock.web.MockMultipartFile;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;

import java.util.Date;
import java.util.Map;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.ArgumentMatchers.*;
import static org.mockito.Mockito.*;

class BlogImageServiceImplTests {
    private BlogMapper blogMapper;
    private BlogImageStorage storage;
    private BlogImageServiceImpl service;

    @BeforeEach
    void setUp() {
        blogMapper = mock(BlogMapper.class);
        storage = mock(BlogImageStorage.class);
        service = new BlogImageServiceImpl(blogMapper, storage);
        User user = new User(7, "tester", "password", "photo", "user", "false");
        SecurityContextHolder.getContext().setAuthentication(
                new UsernamePasswordAuthenticationToken(new UserDetailsImpl(user), null));
    }

    @AfterEach
    void tearDown() {
        SecurityContextHolder.clearContext();
    }

    @Test
    void uploadsOwnedPngUsingScopedObjectName() throws Exception {
        when(blogMapper.selectById(12)).thenReturn(blog(12, 7));
        when(storage.upload(anyString(), any(), anyLong(), eq("image/png")))
                .thenAnswer(invocation -> "http://minio/blog/" + invocation.getArgument(0));
        MockMultipartFile file = new MockMultipartFile("file", "diagram.PNG", "image/png",
                new byte[]{(byte) 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a});

        Map<String, String> result = service.upload(12, file);

        assertEquals("success", result.get("error_message"));
        assertTrue(result.get("url").matches("http://minio/blog/blog-images/7/12/[a-f0-9-]+\\.png"));
        verify(storage).upload(matches("blog-images/7/12/[a-f0-9-]+\\.png"), any(), eq(8L), eq("image/png"));
    }

    @Test
    void rejectsImageOwnedByAnotherUser() {
        when(blogMapper.selectById(12)).thenReturn(blog(12, 8));
        MockMultipartFile file = png("x.png", "image/png");

        assertEquals("没有权限向此博客上传图片", service.upload(12, file).get("error_message"));
        verifyNoInteractions(storage);
    }

    @Test
    void rejectsMimeExtensionMismatch() {
        when(blogMapper.selectById(12)).thenReturn(blog(12, 7));

        assertEquals("图片扩展名与文件内容不一致",
                service.upload(12, png("x.jpg", "image/png")).get("error_message"));
        verifyNoInteractions(storage);
    }

    @Test
    void rejectsSpoofedMagicBytes() {
        when(blogMapper.selectById(12)).thenReturn(blog(12, 7));
        MockMultipartFile file = new MockMultipartFile("file", "x.png", "image/png", "not-png".getBytes());

        assertEquals("图片文件内容与声明格式不一致", service.upload(12, file).get("error_message"));
        verifyNoInteractions(storage);
    }

    @Test
    void rejectsFilesLargerThanFiveMiB() {
        when(blogMapper.selectById(12)).thenReturn(blog(12, 7));
        byte[] content = new byte[5 * 1024 * 1024 + 1];
        MockMultipartFile file = new MockMultipartFile("file", "x.png", "image/png", content);

        assertEquals("图片不能超过5MB", service.upload(12, file).get("error_message"));
    }

    @Test
    void doesNotExposeStorageFailureDetails() throws Exception {
        when(blogMapper.selectById(12)).thenReturn(blog(12, 7));
        when(storage.upload(anyString(), any(), anyLong(), anyString()))
                .thenThrow(new RuntimeException("secret endpoint and bucket"));

        assertEquals("图片上传失败", service.upload(12, png("x.png", "image/png")).get("error_message"));
    }

    @Test
    void removesUploadedObjectIfBlogWasDeletedDuringUpload() throws Exception {
        when(blogMapper.selectById(12)).thenReturn(blog(12, 7), null);
        when(storage.upload(anyString(), any(), anyLong(), anyString())).thenReturn("http://minio/x.png");

        assertEquals("博客已被删除，图片未保存", service.upload(12, png("x.png", "image/png")).get("error_message"));
        verify(storage).deleteObject(matches("blog-images/7/12/[a-f0-9-]+\\.png"));
    }

    private MockMultipartFile png(String name, String contentType) {
        return new MockMultipartFile("file", name, contentType,
                new byte[]{(byte) 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a});
    }

    private Blog blog(int id, int userId) {
        return new Blog(id, userId, "title", "description", "content", new Date(), new Date());
    }
}
