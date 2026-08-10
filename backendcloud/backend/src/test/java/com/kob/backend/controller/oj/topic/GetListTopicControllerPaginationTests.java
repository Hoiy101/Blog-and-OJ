package com.kob.backend.controller.oj.topic;

import com.kob.backend.model.PageResponse;
import com.kob.backend.pojo.Topic;
import com.kob.backend.service.oj.topic.GetListTopicService;
import org.junit.jupiter.api.Test;

import java.util.Collections;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertSame;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

class GetListTopicControllerPaginationTests {
    @Test
    void preservesLegacyArrayAndUsesPaginationWhenPageIsPresent() {
        GetListTopicService service = mock(GetListTopicService.class);
        GetListTopicController controller = new GetListTopicController(service);
        List<Topic> legacy = Collections.singletonList(new Topic());
        PageResponse<Topic> page = new PageResponse<>(legacy, 2L, 20L, 21L, 2L);
        when(service.getList()).thenReturn(legacy);
        when(service.getList(2L, "Java")).thenReturn(page);

        assertSame(legacy, controller.getlist(null, ""));
        assertSame(page, controller.getlist("2", "Java"));
        verify(service).getList();
        verify(service).getList(2L, "Java");
    }
}
