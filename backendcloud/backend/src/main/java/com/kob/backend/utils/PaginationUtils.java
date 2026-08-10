package com.kob.backend.utils;

public final class PaginationUtils {
    private PaginationUtils() {
    }

    public static long parsePage(String page) {
        if (page == null) {
            return 1L;
        }
        try {
            long parsed = Long.parseLong(page.trim());
            return parsed > 0L ? parsed : 1L;
        } catch (NumberFormatException exception) {
            return 1L;
        }
    }

    public static String normalizeKeyword(String keyword) {
        return keyword == null ? "" : keyword.trim();
    }
}
