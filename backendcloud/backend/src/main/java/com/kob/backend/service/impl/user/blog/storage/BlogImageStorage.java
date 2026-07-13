package com.kob.backend.service.impl.user.blog.storage;

import java.io.InputStream;

public interface BlogImageStorage {
    String upload(String objectName, InputStream stream, long size, String contentType) throws Exception;
    void deleteObject(String objectName) throws Exception;
    void deletePrefix(String prefix) throws Exception;
}
