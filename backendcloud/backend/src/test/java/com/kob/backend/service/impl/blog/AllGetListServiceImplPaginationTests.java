package com.kob.backend.service.impl.blog;

import com.baomidou.mybatisplus.core.conditions.Wrapper;
import com.baomidou.mybatisplus.core.MybatisConfiguration;
import com.baomidou.mybatisplus.core.metadata.TableInfoHelper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.model.PageResponse;
import com.kob.backend.pojo.Blog;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;
import org.apache.ibatis.builder.MapperBuilderAssistant;

import java.util.Collections;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@ExtendWith(MockitoExtension.class)
class AllGetListServiceImplPaginationTests {
    @Mock
    private BlogMapper blogMapper;

    private AllGetListServiceImpl service;

    @BeforeAll
    static void initializeMybatisTableMetadata() {
        TableInfoHelper.initTableInfo(
                new MapperBuilderAssistant(new MybatisConfiguration(), "blog-pagination-test"),
                Blog.class
        );
    }

    @BeforeEach
    void setUp() {
        service = new AllGetListServiceImpl(blogMapper);
    }

    @Test
    void requestsTenBlogsWithFilteredStableOrdering() {
        when(blogMapper.selectPage(any(Page.class), any())).thenAnswer(invocation -> {
            Page<Blog> page = invocation.getArgument(0);
            page.setTotal(12L);
            page.setRecords(Collections.singletonList(new Blog()));
            return page;
        });

        PageResponse<Blog> response = service.getAll(2L, " Java ");

        ArgumentCaptor<Page<Blog>> pageCaptor = ArgumentCaptor.forClass(Page.class);
        ArgumentCaptor<Wrapper<Blog>> wrapperCaptor = ArgumentCaptor.forClass(Wrapper.class);
        verify(blogMapper).selectPage(pageCaptor.capture(), wrapperCaptor.capture());
        assertEquals(2L, pageCaptor.getValue().getCurrent());
        assertEquals(10L, pageCaptor.getValue().getSize());
        String sql = wrapperCaptor.getValue().getSqlSegment().toLowerCase();
        assertTrue(sql.contains("title") && sql.contains("description"));
        assertTrue(sql.indexOf("modifytime") < sql.lastIndexOf("id"));
        assertEquals(12L, response.getTotal());
        assertEquals(2L, response.getTotalPages());
        assertEquals(1, response.getRecords().size());
    }

    @Test
    void requeriesTheLastBlogPageWhenRequestedPageOverflows() {
        when(blogMapper.selectPage(any(Page.class), any())).thenAnswer(invocation -> {
            Page<Blog> page = invocation.getArgument(0);
            page.setTotal(21L);
            page.setRecords(Collections.singletonList(new Blog()));
            return page;
        });

        PageResponse<Blog> response = service.getAll(8L, "");

        ArgumentCaptor<Page<Blog>> pageCaptor = ArgumentCaptor.forClass(Page.class);
        verify(blogMapper, times(2)).selectPage(pageCaptor.capture(), any());
        assertEquals(8L, pageCaptor.getAllValues().get(0).getCurrent());
        assertEquals(3L, pageCaptor.getAllValues().get(1).getCurrent());
        assertEquals(3L, response.getCurrentPage());
        assertEquals(21L, response.getTotal());
    }
}
