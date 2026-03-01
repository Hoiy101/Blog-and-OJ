package com.kob.backend.controller;

import com.aliyun.oss.OSS;
import com.aliyun.oss.OSSException;
import com.aliyun.oss.model.PutObjectRequest;
import com.aliyun.oss.model.PutObjectResult;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;
import com.aliyuncs.exceptions.ClientException;

import java.io.IOException;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.UUID;

@RestController
@RequestMapping("/file") // 添加基础路径
public class FileController {

    @Autowired
    private OSS ossClient;

    @Value("${aliyunoss.bucketName}")
    private String bucketName;

    @Value("${aliyunoss.endpoint}")
    private String endpoint;

    /**
     * 文件上传接口
     * @param file 上传的文件
     * @return 文件访问URL
     */
    @PostMapping("/upload")
    public String uploadFile(@RequestParam("file") MultipartFile file) {
        if (file == null || file.isEmpty()) {
            return "error: 文件不能为空";
        }

        // 生成唯一文件名，防止覆盖
        String originalFilename = file.getOriginalFilename();
        String fileExtension = originalFilename.substring(originalFilename.lastIndexOf("."));
        String newFilename = UUID.randomUUID().toString().replace("-", "") + fileExtension;

        // 按日期分目录存储
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy/MM/dd");
        String datePath = sdf.format(new Date());
        String objectKey = datePath + "/" + newFilename;

        try (InputStream inputStream = file.getInputStream()) {
            // 创建PutObjectRequest对象
            PutObjectRequest putObjectRequest = new PutObjectRequest(bucketName, objectKey, inputStream);

            // 可选的：设置文件元数据
            // ObjectMetadata metadata = new ObjectMetadata();
            // metadata.setContentType(file.getContentType());
            // metadata.setContentLength(file.getSize());
            // putObjectRequest.setMetadata(metadata);

            // 上传文件
            PutObjectResult result = ossClient.putObject(putObjectRequest);
            System.out.println("文件上传成功，ETag: " + result.getETag());

            // 构建文件访问URL
            // 格式：https://bucket.endpoint/objectKey
            String fileUrl = "https://" + bucketName + "." + endpoint + "/" + objectKey;

            return fileUrl;

        } catch (OSSException oe) {
            System.err.println("OSS错误: " + oe.getMessage());
            System.err.println("错误代码: " + oe.getErrorCode());
            System.err.println("请求ID: " + oe.getRequestId());
            return "error: OSS服务异常 - " + oe.getErrorCode();

        } catch (IOException e) {
            System.err.println("IO错误: " + e.getMessage());
            return "error: 文件读取失败";
        }
        // 注意：删除了finally块中关闭ossClient的代码
    }
}
