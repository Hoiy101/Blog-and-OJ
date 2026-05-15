package com.evaluatesystem.service.utils;


import cn.hutool.core.date.StopWatch;
import cn.hutool.core.io.FileUtil;
import cn.hutool.core.util.ArrayUtil;
import com.alibaba.fastjson2.JSONObject;
import com.github.dockerjava.api.DockerClient;
import com.github.dockerjava.api.async.ResultCallback;
import com.github.dockerjava.api.command.*;
import com.github.dockerjava.api.model.*;
import com.github.dockerjava.core.DockerClientBuilder;
import com.github.dockerjava.core.command.ExecStartResultCallback;
import com.github.dockerjava.core.exec.ExecStartCmdExec;
import org.omg.CORBA.TIMEOUT;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;
import org.springframework.web.client.RestTemplate;

import javax.swing.*;
import java.io.Closeable;
import java.io.File;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.sql.SQLOutput;
import java.util.ArrayList;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;

@Component
public class EvaluatePool {

    public static RestTemplate restTemplate;


    @Autowired
    public void setEvaluatePool(RestTemplate restTemplate) {
        EvaluatePool.restTemplate = restTemplate;
    }

    private final String EvaluatePassUrl = "http://127.0.0.1:3000/oj/evaluate/end/";

    private void EvaluatePass(JSONObject jsonObject) {
        restTemplate.postForObject(EvaluatePassUrl, jsonObject, String.class);
        System.out.println(jsonObject.get("user_id") + "EvaluatePass Success");
    }

    private String EvaluateFile(String code) {

        String userDir = System.getProperty("user.dir");

        String globalCodePathName = userDir + File.separator + "globalCode";

        if(!FileUtil.exist(globalCodePathName)){
            FileUtil.mkdir(globalCodePathName);
        }

        String userCodeParenPath = globalCodePathName + File.separator + UUID.randomUUID();

        String userCodePath = userCodeParenPath + File.separator + "Main.java";

        File userCodeFile = FileUtil.writeString(code, userCodePath, StandardCharsets.UTF_8);
        return userCodeParenPath;
    }

    public EvaluateMessage compileFile(File userCodeFile) {
        String compileCmp = String.format("javac -encoding utf-8 %s",  userCodeFile.getAbsolutePath());

        try {
            Process compileProcess = Runtime.getRuntime().exec(compileCmp);

            EvaluateMessage evaluateMessage = ProcessUtils.runProcessAndGetMessage(compileProcess, "编译");

            if(evaluateMessage.getExitValue() != 0){
                evaluateMessage.setErrorMessage("编译错误");
            }

            return evaluateMessage;

        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private String addDocker(String language, String userCodeParenPath) {
        DockerClient dockerClient = DockerClientBuilder.getInstance().build();
        String image = "";
        boolean imageExists = false;
        if(language.equals("java")){ // 根据语言不同，选择不一样的docker镜像下载
            image = "eclipse-temurin:8-jdk";
        }
        try {
            dockerClient.inspectImageCmd(image).exec();
            imageExists = true;
        }
        catch (Exception e) {
            System.out.println("镜像不存在，开始下载docker镜像");
        }
        if(!imageExists){
            PullImageCmd pullImageCmd = dockerClient.pullImageCmd(image);
            PullImageResultCallback pullImageResultCallback = new PullImageResultCallback() {
                @Override
                public void onNext(PullResponseItem item){
                    System.out.println("下载镜像:" + item.getStatus());
                    super.onNext(item);
                }
            };

            try {
                pullImageCmd.exec(pullImageResultCallback).awaitCompletion();
            } catch (InterruptedException e) {
                System.out.println("拉取镜像错误");
                throw new RuntimeException(e);
            }
        }
        CreateContainerCmd containerCmd = dockerClient.createContainerCmd(image);

        HostConfig hostConfig = new HostConfig();
        hostConfig.withMemory(100 * 1000 * 1000L);
        hostConfig.withMemorySwap(0L);
        hostConfig.withCpuCount(1L);
        hostConfig.setBinds(new Bind(userCodeParenPath, new Volume("/app")));

        CreateContainerResponse createContainerResponse = containerCmd
                .withHostConfig(hostConfig)
                .withNetworkDisabled(true)          // 禁用网络
                .withReadonlyRootfs(true)           // 只读根文件系统
                .withAttachStdin(true)
                .withAttachStderr(true)
                .withAttachStdout(true)
                .withTty(true)
                .exec();
        String container_id = createContainerResponse.getId();
        dockerClient.startContainerCmd(createContainerResponse.getId()).exec();
        return container_id;
    }

    private List<EvaluateMessage> addEvaluate(List<String> inputList, String container_id){
        List<EvaluateMessage> evaluateList = new ArrayList<>();

        for(String input : inputList) {
            StopWatch stopWatch = new StopWatch();

            String[] inputArr = input.split(" ");

            String[] cmpArr = ArrayUtil.append(new String[]{"java", "-cp", "/app", "Main"}, inputArr);
            DockerClient dockerClient = DockerClientBuilder.getInstance().build();
            ExecCreateCmdResponse execCreateCmdResponse = dockerClient.execCreateCmd(container_id)
                    .withCmd(cmpArr)
                    .withAttachStderr(true)
                    .withAttachStdin(true)
                    .withAttachStdout(true)
                    .exec();
            final String[] errorMessage = {null};
            final String[] Message = {null};
            Long maxTime = 0L;
            final Long[] maxMemory = {0L};
            String exec_id = execCreateCmdResponse.getId();

            ExecStartResultCallback execStartResultCallback = new ExecStartResultCallback() {
                @Override
                public void onComplete() {
                    super.onComplete();
                }

                @Override
                public void onNext(Frame frame) {
                    StreamType streamType = frame.getStreamType();
                    if (streamType.STDERR.equals(streamType)) {
                        errorMessage[0] = new String(frame.getPayload());
                    } else {
                        Message[0] = new String(frame.getPayload());
                    }
                    super.onNext(frame);
                }

            };
            StatsCmd statsCmd = dockerClient.statsCmd(container_id);
            ResultCallback<Statistics> resultCallback = statsCmd.exec(
                    new ResultCallback<Statistics>() {
                        @Override
                        public void close() throws IOException {
                        }

                        @Override
                        public void onNext(Statistics statistics) {
                            maxMemory[0] = Math.max(maxMemory[0], statistics.getMemoryStats().getUsage());
                        }

                        @Override
                        public void onStart(Closeable closeable) {
                        }

                        @Override
                        public void onError(Throwable throwable) {
                        }

                        @Override
                        public void onComplete() {
                        }
                    });

            try {
                stopWatch.start();

                dockerClient.execStartCmd(exec_id)
                        .exec(execStartResultCallback)
                        .awaitCompletion(2000, TimeUnit.MILLISECONDS);

                stopWatch.stop();

                maxTime = Math.max(maxTime, stopWatch.getTotalTimeMillis());
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }finally {
                statsCmd.close();
            }

            EvaluateMessage evaluateMessage = new EvaluateMessage();
            evaluateMessage.setErrorMessage(errorMessage[0]);
            evaluateMessage.setMessage(Message[0]);
            evaluateMessage.setTime(maxTime);
            evaluateMessage.setMemory(maxMemory[0]);

            evaluateList.add(evaluateMessage);
        }
        return evaluateList;
    }



    public void startEvaluate(JSONObject jsonObject) {
        Integer user_id = jsonObject.getInteger("user_id");
        Integer evaluate_id = jsonObject.getInteger("evaluate_id");
        String code = jsonObject.getString("code");
        String language =  jsonObject.getString("language");
        List<String> inputList = jsonObject.getJSONArray("inputList").toJavaList(String.class);
        List<String> outputList = jsonObject.getJSONArray("outputList").toJavaList(String.class);

        JSONObject message = new JSONObject();
        message.put("user_id", user_id);
        message.put("evaluate_id", evaluate_id);

        String userCodeParenPath =  EvaluateFile(code);//将用户代码打包成文件

        File userCodeFile = new File(userCodeParenPath + File.separator + "Main.java");

        EvaluateMessage evaluateMessage = compileFile(userCodeFile);//将用户代码进行编译
        if(evaluateMessage.getErrorMessage().equals("编译错误")){
            message.put("state", "Compile Error");
            message.put("score", 0);
            EvaluatePass(message);
            return;
        }

        String container_id = addDocker(language, userCodeParenPath);//检测用户判题的语言的docker镜像是否存在，如果不存在则下载

        List<EvaluateMessage> evaluateMessagesList = addEvaluate(inputList, container_id);// 使用docker容器开始判题,并且将判题的结果返回

        Integer score = 0;
        String state = "Wrong Answer";
        for(int i = 0; i < evaluateMessagesList.size(); i++){
            EvaluateMessage evaluateMessage1 = evaluateMessagesList.get(i);
            String output = outputList.get(i);
            String input = evaluateMessage1.getMessage();
            String error = evaluateMessage1.getErrorMessage();
            Long time = evaluateMessage1.getTime();
            Long memory = evaluateMessage1.getMemory();

            if(error != null){
                message.put("state", "Runtime Error");
                message.put("score", 0);
                EvaluatePass(message);
                return;
            }
            if(output.equals(input)) score += 10;
        }
        if (score == 100) {
            message.put("state", "Accepted");
            message.put("score", score);
        }
        else if (score < 100) {
            message.put("state", "Wrong Answer");
            message.put("score", score);
        }
        System.out.println(user_id + " " + evaluate_id + " " + state + " " + score);
        EvaluatePass(message);
        FileUtil.del(userCodeParenPath);
    }
}
