package com.kob.backend.service.impl.user.blog;

import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.pojo.Blog;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.user.blog.storage.BlogImageStorage;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.service.user.blog.BlogImageService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.InputStream;
import java.util.*;

@Service
public class BlogImageServiceImpl implements BlogImageService {
    private static final Logger LOGGER = LoggerFactory.getLogger(BlogImageServiceImpl.class);
    private static final long MAX_SIZE = 5L * 1024 * 1024;
    private static final Map<String, String> EXTENSIONS = new HashMap<>();
    static {
        EXTENSIONS.put("image/png", "png");
        EXTENSIONS.put("image/jpeg", "jpg");
        EXTENSIONS.put("image/webp", "webp");
        EXTENSIONS.put("image/gif", "gif");
    }

    private final BlogMapper blogMapper;
    private final BlogImageStorage storage;

    @Autowired
    public BlogImageServiceImpl(BlogMapper blogMapper, BlogImageStorage storage) {
        this.blogMapper = blogMapper;
        this.storage = storage;
    }

    @Override
    public Map<String, String> upload(Integer blogId, MultipartFile file) {
        Map<String, String> result = new HashMap<>();
        User user = currentUser();
        if (user == null) return error(result, "用户未登录");
        Blog blog = blogId == null ? null : blogMapper.selectById(blogId);
        if (blog == null) return error(result, "博客不存在或已被删除");
        if (!user.getId().equals(blog.getUserId())) return error(result, "没有权限向此博客上传图片");
        if (file == null || file.isEmpty()) return error(result, "请选择图片文件");
        if (file.getSize() > MAX_SIZE) return error(result, "图片不能超过5MB");

        String mime = file.getContentType() == null ? "" : file.getContentType().toLowerCase(Locale.ROOT);
        String expectedExtension = EXTENSIONS.get(mime);
        if (expectedExtension == null) return error(result, "图片格式只支持PNG、JPG、WebP或GIF");
        String actualExtension = extension(file.getOriginalFilename());
        if ("jpeg".equals(actualExtension)) actualExtension = "jpg";
        if (!expectedExtension.equals(actualExtension)) return error(result, "图片扩展名与文件内容不一致");

        try {
            if (!matchesMagic(file, expectedExtension)) return error(result, "图片文件内容与声明格式不一致");
            String objectName = "blog-images/" + user.getId() + "/" + blogId + "/"
                    + UUID.randomUUID().toString() + "." + expectedExtension;
            String url = storage.upload(objectName, file.getInputStream(), file.getSize(), mime);
            Blog currentBlog = blogMapper.selectById(blogId);
            if (currentBlog == null || !user.getId().equals(currentBlog.getUserId())) {
                try {
                    storage.deleteObject(objectName);
                } catch (Exception cleanupError) {
                    LOGGER.warn("Failed to remove orphaned blog image {}", objectName, cleanupError);
                }
                return error(result, "博客已被删除，图片未保存");
            }
            result.put("error_message", "success");
            result.put("url", url);
        } catch (Exception e) {
            LOGGER.error("Failed to upload image for blog {}", blogId, e);
            return error(result, "图片上传失败");
        }
        return result;
    }

    private User currentUser() {
        Authentication authentication = SecurityContextHolder.getContext().getAuthentication();
        if (authentication == null || !(authentication.getPrincipal() instanceof UserDetailsImpl)) return null;
        return ((UserDetailsImpl) authentication.getPrincipal()).getUser();
    }

    private Map<String, String> error(Map<String, String> result, String message) {
        result.put("error_message", message);
        return result;
    }

    private String extension(String filename) {
        if (filename == null) return "";
        int dot = filename.lastIndexOf('.');
        return dot < 0 ? "" : filename.substring(dot + 1).toLowerCase(Locale.ROOT);
    }

    private boolean matchesMagic(MultipartFile file, String extension) throws Exception {
        byte[] header = new byte[12];
        int count = 0;
        try (InputStream input = file.getInputStream()) {
            while (count < header.length) {
                int read = input.read(header, count, header.length - count);
                if (read < 0) break;
                count += read;
            }
        }
        if ("png".equals(extension)) return count >= 8 && header[0] == (byte) 0x89 && header[1] == 0x50
                && header[2] == 0x4e && header[3] == 0x47 && header[4] == 0x0d && header[5] == 0x0a
                && header[6] == 0x1a && header[7] == 0x0a;
        if ("jpg".equals(extension)) return count >= 3 && header[0] == (byte) 0xff && header[1] == (byte) 0xd8 && header[2] == (byte) 0xff;
        if ("gif".equals(extension)) return count >= 6 && new String(header, 0, 6, "US-ASCII").matches("GIF8[79]a");
        return count >= 12 && new String(header, 0, 4, "US-ASCII").equals("RIFF")
                && new String(header, 8, 4, "US-ASCII").equals("WEBP");
    }
}
