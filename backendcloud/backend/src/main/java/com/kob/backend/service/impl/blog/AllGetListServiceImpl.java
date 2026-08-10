package com.kob.backend.service.impl.blog;

import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.model.PageResponse;
import com.kob.backend.pojo.Blog;
import com.kob.backend.service.blog.AllGetListService;
import com.kob.backend.utils.PaginationUtils;
import org.springframework.stereotype.Service;

@Service
public class AllGetListServiceImpl implements AllGetListService {
    private static final long PAGE_SIZE = 10L;

    private final BlogMapper blogMapper;

    public AllGetListServiceImpl(BlogMapper blogMapper) {
        this.blogMapper = blogMapper;
    }

    @Override
    public PageResponse<Blog> getAll(long page, String keyword) {
        long requestedPage = Math.max(1L, page);
        String normalizedKeyword = PaginationUtils.normalizeKeyword(keyword);
        LambdaQueryWrapper<Blog> query = new LambdaQueryWrapper<>();
        if (!normalizedKeyword.isEmpty()) {
            query.and(item -> item.like(Blog::getTitle, normalizedKeyword)
                    .or()
                    .like(Blog::getDescription, normalizedKeyword));
        }
        query.orderByDesc(Blog::getModifytime).orderByDesc(Blog::getId);

        Page<Blog> result = blogMapper.selectPage(new Page<>(requestedPage, PAGE_SIZE), query);
        long total = result.getTotal();
        long totalPages = result.getPages();
        long currentPage = totalPages == 0L ? 1L : Math.min(requestedPage, totalPages);
        if (currentPage != requestedPage) {
            result = blogMapper.selectPage(new Page<>(currentPage, PAGE_SIZE, false), query);
        }

        return new PageResponse<>(result.getRecords(), currentPage, PAGE_SIZE, total, totalPages);
    }
}
