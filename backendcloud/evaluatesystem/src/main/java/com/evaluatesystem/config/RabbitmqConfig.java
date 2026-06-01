package com.evaluatesystem.config;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.amqp.core.*;
import org.springframework.amqp.rabbit.config.SimpleRabbitListenerContainerFactory;
import org.springframework.amqp.rabbit.connection.CachingConnectionFactory;
import org.springframework.amqp.rabbit.connection.CorrelationData;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.amqp.support.converter.Jackson2JsonMessageConverter;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.autoconfigure.amqp.SimpleRabbitListenerContainerFactoryConfigurer;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class RabbitmqConfig {

    private static final Logger log = LoggerFactory.getLogger(RabbitmqConfig.class);

    @Autowired
    private CachingConnectionFactory connectionFactory;

    @Autowired
    private SimpleRabbitListenerContainerFactoryConfigurer factoryConfigurer;

    // ==================== 队列 / 交换机 / 绑定 ====================
    // 两个服务声明完全一致，谁先启动谁创建，后启动的复用

    @Bean
    public Queue evaluateTaskQueue() {
        return QueueBuilder.durable("evaluate.task.queue").build();
    }

    @Bean
    public DirectExchange evaluateTaskExchange() {
        return new DirectExchange("evaluate.task.exchange", true, false);
    }

    @Bean
    public Binding evaluateTaskBinding() {
        return BindingBuilder.bind(evaluateTaskQueue())
                .to(evaluateTaskExchange()).with("evaluate.task");
    }

    @Bean
    public Queue evaluateResultQueue() {
        return QueueBuilder.durable("evaluate.result.queue").build();
    }

    @Bean
    public DirectExchange evaluateResultExchange() {
        return new DirectExchange("evaluate.result.exchange", true, false);
    }

    @Bean
    public Binding evaluateResultBinding() {
        return BindingBuilder.bind(evaluateResultQueue())
                .to(evaluateResultExchange()).with("evaluate.result");
    }

    // ==================== 消费者工厂 ====================

    /**
     * 单消费者 + 手动 ACK
     * 适用场景：Evaluatesystem 消费判题任务（Docker 资源有限，一次只判一道）
     */
    @Bean
    public SimpleRabbitListenerContainerFactory singleListenerContainer() {
        SimpleRabbitListenerContainerFactory factory = new SimpleRabbitListenerContainerFactory();
        factory.setConnectionFactory(connectionFactory);
        factory.setMessageConverter(new Jackson2JsonMessageConverter());
        factory.setConcurrentConsumers(1);
        factory.setMaxConcurrentConsumers(1);
        factory.setPrefetchCount(1);
        factory.setAcknowledgeMode(AcknowledgeMode.MANUAL);
        return factory;
    }

    /**
     * 多消费者 + 自动 ACK（并发/预取数可调）
     * 适用场景：Backend 消费判题结果（轻量写库 + WebSocket 推送，支持并发）
     */
    @Bean
    public SimpleRabbitListenerContainerFactory multiListenerContainer() {
        SimpleRabbitListenerContainerFactory factory = new SimpleRabbitListenerContainerFactory();
        factoryConfigurer.configure(factory, connectionFactory);  // 继承 Spring Boot 默认值
        factory.setMessageConverter(new Jackson2JsonMessageConverter());
        factory.setAcknowledgeMode(AcknowledgeMode.AUTO);
        return factory;
    }

    // ==================== RabbitTemplate（生产者端） ====================

    @Bean
    public RabbitTemplate rabbitTemplate() {
        RabbitTemplate template = new RabbitTemplate(connectionFactory);
        template.setMandatory(true);
        template.setMessageConverter(new Jackson2JsonMessageConverter());

        // 消息到达 Broker 的回调
        template.setConfirmCallback((CorrelationData cd, boolean ack, String cause) -> {
            if (!ack) {
                log.error("消息发送失败: correlationData={}, cause={}", cd, cause);
            }
        });

        // 消息无法路由的回调（Mandatory=true 才有）
        template.setReturnsCallback(returned ->
                log.error("消息路由失败: exchange={}, route={}, replyCode={}, replyText={}",
                        returned.getExchange(), returned.getRoutingKey(),
                        returned.getReplyCode(), returned.getReplyText())
        );

        return template;
    }
}
