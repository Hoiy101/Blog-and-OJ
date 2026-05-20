package com.evaluatesystem.service.utils;

import com.alibaba.fastjson2.JSONObject;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class EvaluatePool extends Thread {
    private final ReentrantLock lock = new ReentrantLock();
    private final Condition condition = lock.newCondition();
    private Queue<JSONObject> jsonObjectsQueue = new ConcurrentLinkedQueue<>();
    public void addEvaluate(JSONObject evaluate) {
        lock.lock();
        try {
            jsonObjectsQueue.add(evaluate);
            condition.signalAll();
        }finally {
            lock.unlock();
        }
    }

    private void consumer(JSONObject jsonObject) {
        Consumer consumer = new Consumer();
        consumer.startEvaluate(jsonObject);
    }

    @Override
    public void run() {
        while (true) {
            lock.lock();
            if(jsonObjectsQueue.isEmpty()) {
                try {
                    condition.await();
                } catch (InterruptedException e) {
                    lock.unlock();
                    throw new RuntimeException(e);
                }
            }
            else{
                JSONObject evaluate = jsonObjectsQueue.remove();
                lock.unlock();
                consumer(evaluate);
            }
        }
    }
}
