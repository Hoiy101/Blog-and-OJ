package com.evaluatesystem.service.utils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

public class ProcessUtils {

    public static EvaluateMessage runProcessAndGetMessage(Process process, String operationName) {
        long startTime = System.currentTimeMillis();
        StringBuilder message = new StringBuilder();
        StringBuilder errorMessage = new StringBuilder();

        Thread outputThread = new Thread(() -> readStream(process.getInputStream(), message));
        Thread errorThread = new Thread(() -> readStream(process.getErrorStream(), errorMessage));
        outputThread.start();
        errorThread.start();

        try {
            int exitValue = process.waitFor();
            outputThread.join();
            errorThread.join();

            EvaluateMessage evaluateMessage = new EvaluateMessage();
            evaluateMessage.setExitValue(exitValue);
            evaluateMessage.setMessage(message.toString());
            evaluateMessage.setErrorMessage(errorMessage.toString());
            evaluateMessage.setTime(System.currentTimeMillis() - startTime);
            return evaluateMessage;
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
            throw new RuntimeException(operationName + "进程被中断", e);
        }
    }

    private static void readStream(InputStream inputStream, StringBuilder result) {
        try (BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream, StandardCharsets.UTF_8))) {
            String line;
            while ((line = reader.readLine()) != null) {
                result.append(line).append(System.lineSeparator());
            }
        } catch (IOException e) {
            throw new RuntimeException("读取进程输出失败", e);
        }
    }
}
