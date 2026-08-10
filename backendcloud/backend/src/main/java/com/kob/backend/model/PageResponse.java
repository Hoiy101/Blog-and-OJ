package com.kob.backend.model;

import java.util.List;

public class PageResponse<T> {
    private final List<T> records;
    private final long currentPage;
    private final long pageSize;
    private final long total;
    private final long totalPages;

    public PageResponse(List<T> records, long currentPage, long pageSize, long total, long totalPages) {
        this.records = records;
        this.currentPage = currentPage;
        this.pageSize = pageSize;
        this.total = total;
        this.totalPages = totalPages;
    }

    public List<T> getRecords() {
        return records;
    }

    public long getCurrentPage() {
        return currentPage;
    }

    public long getPageSize() {
        return pageSize;
    }

    public long getTotal() {
        return total;
    }

    public long getTotalPages() {
        return totalPages;
    }
}
