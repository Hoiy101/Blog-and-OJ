package com.kob.backend.utils;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class PaginationUtilsTests {
    @Test
    void parsesPositivePageAndFallsBackToOne() {
        assertEquals(3L, PaginationUtils.parsePage("3"));
        assertEquals(1L, PaginationUtils.parsePage(null));
        assertEquals(1L, PaginationUtils.parsePage("0"));
        assertEquals(1L, PaginationUtils.parsePage("-2"));
        assertEquals(1L, PaginationUtils.parsePage("abc"));
    }

    @Test
    void trimsNullableKeyword() {
        assertEquals("Java", PaginationUtils.normalizeKeyword("  Java  "));
        assertEquals("", PaginationUtils.normalizeKeyword(null));
    }
}
