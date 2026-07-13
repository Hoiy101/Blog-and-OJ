package com.kob.backend.service.impl.user.blog.storage;

import io.minio.*;
import io.minio.messages.Item;
import org.springframework.stereotype.Component;

import java.io.InputStream;

@Component
public class MinioBlogImageStorage implements BlogImageStorage {
    private static final String ENDPOINT = "http://47.105.40.164:9000";
    private static final String ACCESS_KEY = "wuyanzu";
    private static final String SECRET_KEY = "bo@DwF1mzr_wF7am";
    private static final String BUCKET = "blog";

    private MinioClient client() {
        return MinioClient.builder().endpoint(ENDPOINT).credentials(ACCESS_KEY, SECRET_KEY).build();
    }

    private void ensureBucket(MinioClient client) throws Exception {
        if (!client.bucketExists(BucketExistsArgs.builder().bucket(BUCKET).build())) {
            client.makeBucket(MakeBucketArgs.builder().bucket(BUCKET).build());
        }
    }

    @Override
    public String upload(String objectName, InputStream stream, long size, String contentType) throws Exception {
        MinioClient client = client();
        ensureBucket(client);
        client.putObject(PutObjectArgs.builder().bucket(BUCKET).object(objectName)
                .stream(stream, size, -1).contentType(contentType).build());
        return ENDPOINT + "/" + BUCKET + "/" + objectName;
    }

    @Override
    public void deletePrefix(String prefix) throws Exception {
        MinioClient client = client();
        if (!client.bucketExists(BucketExistsArgs.builder().bucket(BUCKET).build())) return;
        Iterable<Result<Item>> objects = client.listObjects(ListObjectsArgs.builder()
                .bucket(BUCKET).prefix(prefix).recursive(true).build());
        for (Result<Item> result : objects) {
            client.removeObject(RemoveObjectArgs.builder().bucket(BUCKET).object(result.get().objectName()).build());
        }
    }

    @Override
    public void deleteObject(String objectName) throws Exception {
        client().removeObject(RemoveObjectArgs.builder().bucket(BUCKET).object(objectName).build());
    }
}
