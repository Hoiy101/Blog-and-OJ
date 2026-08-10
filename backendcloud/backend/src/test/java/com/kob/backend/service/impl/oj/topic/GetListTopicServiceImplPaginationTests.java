package com.kob.backend.service.impl.oj.topic;

import com.baomidou.mybatisplus.core.MybatisConfiguration;
import com.baomidou.mybatisplus.core.conditions.Wrapper;
import com.baomidou.mybatisplus.core.metadata.TableInfoHelper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.model.PageResponse;
import com.kob.backend.pojo.Topic;
import org.apache.ibatis.builder.MapperBuilderAssistant;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import java.util.Collections;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@ExtendWith(MockitoExtension.class)
class GetListTopicServiceImplPaginationTests {
    @Mock
    private TopicMapper topicMapper;

    private GetListTopicServiceImpl service;

    @BeforeAll
    static void initializeMybatisTableMetadata() {
        TableInfoHelper.initTableInfo(
                new MapperBuilderAssistant(new MybatisConfiguration(), "topic-pagination-test"),
                Topic.class
        );
    }

    @BeforeEach
    void setUp() {
        service = new GetListTopicServiceImpl(topicMapper);
    }

    @Test
    void requestsTwentyTopicsWithIdTitleDescriptionSearch() {
        when(topicMapper.selectPage(any(Page.class), any())).thenAnswer(invocation -> {
            Page<Topic> page = invocation.getArgument(0);
            page.setTotal(25L);
            page.setRecords(Collections.singletonList(new Topic()));
            return page;
        });

        PageResponse<Topic> response = service.getList(2L, "12");

        ArgumentCaptor<Page<Topic>> pageCaptor = ArgumentCaptor.forClass(Page.class);
        ArgumentCaptor<Wrapper<Topic>> wrapperCaptor = ArgumentCaptor.forClass(Wrapper.class);
        verify(topicMapper).selectPage(pageCaptor.capture(), wrapperCaptor.capture());
        assertEquals(2L, pageCaptor.getValue().getCurrent());
        assertEquals(20L, pageCaptor.getValue().getSize());
        String sql = wrapperCaptor.getValue().getSqlSegment().toLowerCase();
        assertTrue(sql.contains("id") && sql.contains("title") && sql.contains("description"));
        assertTrue(sql.contains("order by") && sql.contains("asc"));
        assertEquals(2L, response.getCurrentPage());
        assertEquals(20L, response.getPageSize());
    }

    @Test
    void requeriesTheLastTopicPageWhenRequestedPageOverflows() {
        when(topicMapper.selectPage(any(Page.class), any())).thenAnswer(invocation -> {
            Page<Topic> page = invocation.getArgument(0);
            page.setTotal(41L);
            page.setRecords(Collections.singletonList(new Topic()));
            return page;
        });

        PageResponse<Topic> response = service.getList(9L, "");

        ArgumentCaptor<Page<Topic>> pageCaptor = ArgumentCaptor.forClass(Page.class);
        verify(topicMapper, times(2)).selectPage(pageCaptor.capture(), any());
        assertEquals(9L, pageCaptor.getAllValues().get(0).getCurrent());
        assertEquals(3L, pageCaptor.getAllValues().get(1).getCurrent());
        assertEquals(3L, response.getCurrentPage());
        assertEquals(41L, response.getTotal());
    }

    @Test
    void signedKeywordDoesNotUseExactTopicIdFilter() {
        when(topicMapper.selectPage(any(Page.class), any())).thenAnswer(invocation -> {
            Page<Topic> page = invocation.getArgument(0);
            page.setTotal(0L);
            page.setRecords(Collections.emptyList());
            return page;
        });

        service.getList(1L, "+12");

        ArgumentCaptor<Wrapper<Topic>> wrapperCaptor = ArgumentCaptor.forClass(Wrapper.class);
        verify(topicMapper).selectPage(any(Page.class), wrapperCaptor.capture());
        String sql = wrapperCaptor.getValue().getSqlSegment().toLowerCase();
        assertFalse(sql.contains("id ="));
        assertTrue(sql.contains("title") && sql.contains("description"));
    }
}
