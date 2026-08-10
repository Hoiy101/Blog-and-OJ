package com.kob.backend.service.impl.user.account;

import com.baomidou.mybatisplus.core.conditions.update.UpdateWrapper;
import com.kob.backend.mapper.UserMapper;
import com.kob.backend.pojo.User;
import com.kob.backend.service.impl.utils.UserDetailsImpl;
import com.kob.backend.service.user.account.AvatarService;
import io.minio.*;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;

import java.util.HashMap;
import java.util.Map;

@Service
public class AvatarServiceImpl implements AvatarService {
    private static final long MAX_AVATAR_SIZE = 5 * 1024 * 1024;

    @Autowired
    private UserMapper userMapper;
    private final String endpoint = "http://47.119.128.174:9000";
    private final String accessKey = "wuyanzu";
    private final String secretKey = "bo@DwF1mzr_wF7am";
    private final String bucket = "blog";
    private final String avatarPrefix = "image";

    @Override
    public Map<String, String> uploadAvatar(MultipartFile file) {
        Map<String, String> map = new HashMap<>();

        User user = getCurrentUser();
        if (user == null) {
            map.put("error_message", "用户未登录");
            return map;
        }

        String errorMessage = validateFile(file);
        if (errorMessage != null) {
            map.put("error_message", errorMessage);
            return map;
        }

        try {
            MinioClient minioClient = MinioClient.builder()
                    .endpoint(endpoint)
                    .credentials(accessKey, secretKey)
                    .build();

            ensureBucketExists(minioClient);

            String objectName = buildObjectName(user.getId(), file.getContentType());
            minioClient.putObject(PutObjectArgs.builder()
                    .bucket(bucket)
                    .object(objectName)
                    .stream(file.getInputStream(), file.getSize(), -1)
                    .contentType(file.getContentType())
                    .build());

            String photo = userMapper.selectById(user.getId()).getPhoto();
            minioClient.removeObject(
                    RemoveObjectArgs.builder()
                            .bucket("blog")
                            .object(photo.substring(31, photo.length()))
                            .build()
            );

            String newphoto = buildObjectUrl(objectName);
            UpdateWrapper<User> updateWrapper = new UpdateWrapper<>();
            updateWrapper.eq("id", user.getId()).set("photo", newphoto);
            userMapper.update(null, updateWrapper);
            user.setPhoto(newphoto);

            map.put("error_message", "success");
            map.put("photo", newphoto);
            return map;
        } catch (Exception e) {
            map.put("error_message", "头像上传失败：" + e.getMessage());
            return map;
        }
    }

    private User getCurrentUser() {
        UsernamePasswordAuthenticationToken authentication =
                (UsernamePasswordAuthenticationToken) SecurityContextHolder.getContext().getAuthentication();

        if (authentication == null || !(authentication.getPrincipal() instanceof UserDetailsImpl)) {
            return null;
        }

        UserDetailsImpl userDetails = (UserDetailsImpl) authentication.getPrincipal();
        return userDetails.getUser();
    }

    private String validateFile(MultipartFile file) {
        if (file == null || file.isEmpty()) {
            return "请选择头像文件";
        }

        if (file.getSize() > MAX_AVATAR_SIZE) {
            return "头像不能超过5MB";
        }

        String contentType = file.getContentType();
        if (!"image/png".equals(contentType) && !"image/jpeg".equals(contentType) && !"image/webp".equals(contentType)) {
            return "头像格式只支持PNG、JPG或WebP";
        }

        return null;
    }

    private void ensureBucketExists(MinioClient minioClient) throws Exception {
        boolean found = minioClient.bucketExists(BucketExistsArgs.builder().bucket(bucket).build());
        if (!found) {
            minioClient.makeBucket(MakeBucketArgs.builder().bucket(bucket).build());
        }
    }

    private String buildObjectName(Integer userId, String contentType) {
        String prefix = avatarPrefix;
        if (!prefix.endsWith("/")) {
            prefix += "/";
        }
        return prefix + userId + getSuffix(contentType);
    }

    private String getSuffix(String contentType) {
        if ("image/png".equals(contentType)) {
            return ".png";
        }
        if ("image/webp".equals(contentType)) {
            return ".webp";
        }
        return ".jpg";
    }

    private String buildObjectUrl(String objectName) {
        String baseUrl = endpoint;
        while (baseUrl.endsWith("/")) {
            baseUrl = baseUrl.substring(0, baseUrl.length() - 1);
        }
        return baseUrl + "/" + bucket + "/" + objectName;
    }
}
