package com.kob.backend.service.impl.oj.topic;

import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.model.PageResponse;
import com.kob.backend.pojo.Topic;
import com.kob.backend.service.oj.topic.GetListTopicService;
import com.kob.backend.utils.PaginationUtils;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class GetListTopicServiceImpl implements GetListTopicService {
    private static final long PAGE_SIZE = 20L;

    private final TopicMapper topicMapper;

    public GetListTopicServiceImpl(TopicMapper topicMapper) {
        this.topicMapper = topicMapper;
    }

    @Override
    public List<Topic> getList() {
        return topicMapper.selectList(null);
    }

    @Override
    public PageResponse<Topic> getList(long page, String keyword) {
        long requestedPage = Math.max(1L, page);
        String normalizedKeyword = PaginationUtils.normalizeKeyword(keyword);
        LambdaQueryWrapper<Topic> query = new LambdaQueryWrapper<>();
        if (!normalizedKeyword.isEmpty()) {
            Integer topicId = parseTopicId(normalizedKeyword);
            query.and(item -> {
                if (topicId != null) {
                    item.eq(Topic::getId, topicId).or();
                }
                item.like(Topic::getTitle, normalizedKeyword)
                        .or()
                        .like(Topic::getDescription, normalizedKeyword);
            });
        }
        query.orderByAsc(Topic::getId);

        Page<Topic> result = topicMapper.selectPage(new Page<>(requestedPage, PAGE_SIZE), query);
        long total = result.getTotal();
        long totalPages = result.getPages();
        long currentPage = totalPages == 0L ? 1L : Math.min(requestedPage, totalPages);
        if (currentPage != requestedPage) {
            result = topicMapper.selectPage(new Page<>(currentPage, PAGE_SIZE, false), query);
        }

        return new PageResponse<>(result.getRecords(), currentPage, PAGE_SIZE, total, totalPages);
    }

    private Integer parseTopicId(String keyword) {
        if (!keyword.matches("\\d+")) {
            return null;
        }
        try {
            return Integer.valueOf(keyword);
        } catch (NumberFormatException exception) {
            return null;
        }
    }
}
