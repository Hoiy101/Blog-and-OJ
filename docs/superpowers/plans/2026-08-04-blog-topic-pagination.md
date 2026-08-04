# Blog and Topic Pagination Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add database-backed pagination to the public blog and topic lists so the browser requests and stores only one page at a time.

**Architecture:** Both existing GET endpoints accept `page` and `keyword` query parameters and return a shared `PageResponse<T>` envelope. MyBatis-Plus performs filtered, stably ordered page queries; Vue pages keep only the returned `records` and use a shared frontend pagination utility for URL parameters and page clamping.

**Tech Stack:** Java 8, Spring Boot 2.6, MyBatis-Plus 3.5.15, JUnit 5, Mockito, Vue 3 Composition API, jQuery AJAX, Node test runner, Bootstrap 5.

## Global Constraints

- Blog page size is fixed at 10 and topic page size is fixed at 20.
- The frontend must never fetch, cache, concatenate, or slice the complete result set.
- Blog search covers title and description and sorts by `modifytime DESC, id DESC`.
- Topic search uses an exact ID match for numeric keywords plus contains matching on title and description, and sorts by `id ASC`.
- Search resets to page 1; invalid page values become 1; values above the last page resolve to the last page.
- The pagination control remains at the bottom-right and contains a left arrow, numeric page input, and right arrow.
- Existing blog row navigation, keyboard accessibility, blog detail rendering, and topic row navigation must remain unchanged.
- Preserve all unrelated staged and unstaged changes; every commit command uses an explicit pathspec.

---

### Task 1: Shared backend pagination contract and MyBatis-Plus configuration

**Files:**
- Modify: `backendcloud/backend/pom.xml`
- Create: `backendcloud/backend/src/main/java/com/kob/backend/config/MybatisPlusConfig.java`
- Create: `backendcloud/backend/src/main/java/com/kob/backend/model/PageResponse.java`
- Create: `backendcloud/backend/src/main/java/com/kob/backend/utils/PaginationUtils.java`
- Test: `backendcloud/backend/src/test/java/com/kob/backend/utils/PaginationUtilsTests.java`

**Interfaces:**
- Produces: `PaginationUtils.parsePage(String): long`
- Produces: `PaginationUtils.normalizeKeyword(String): String`
- Produces: `new PageResponse<T>(List<T>, long currentPage, long pageSize, long total, long totalPages)` with JSON getters named `records`, `currentPage`, `pageSize`, `total`, and `totalPages`.
- Produces: a Spring `MybatisPlusInterceptor` bean containing `PaginationInnerInterceptor(DbType.MYSQL)`.

- [ ] **Step 1: Write the failing pagination utility test**

```java
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
```

- [ ] **Step 2: Run the test and verify RED**

Run:

```bash
cd backendcloud
mvn -pl backend -Dtest=PaginationUtilsTests test
```

Expected: compilation fails because `PaginationUtils` does not exist.

- [ ] **Step 3: Implement the utility and response contract**

```java
package com.kob.backend.utils;

public final class PaginationUtils {
    private PaginationUtils() {
    }

    public static long parsePage(String page) {
        if (page == null) return 1L;
        try {
            long parsed = Long.parseLong(page.trim());
            return parsed > 0 ? parsed : 1L;
        } catch (NumberFormatException exception) {
            return 1L;
        }
    }

    public static String normalizeKeyword(String keyword) {
        return keyword == null ? "" : keyword.trim();
    }
}
```

```java
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

    public List<T> getRecords() { return records; }
    public long getCurrentPage() { return currentPage; }
    public long getPageSize() { return pageSize; }
    public long getTotal() { return total; }
    public long getTotalPages() { return totalPages; }
}
```

Add the pagination parser module required by MyBatis-Plus 3.5.15:

```xml
<dependency>
    <groupId>com.baomidou</groupId>
    <artifactId>mybatis-plus-jsqlparser-4.9</artifactId>
    <version>3.5.15</version>
</dependency>
```

Create the interceptor configuration:

```java
package com.kob.backend.config;

import com.baomidou.mybatisplus.annotation.DbType;
import com.baomidou.mybatisplus.extension.plugins.MybatisPlusInterceptor;
import com.baomidou.mybatisplus.extension.plugins.inner.PaginationInnerInterceptor;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class MybatisPlusConfig {
    @Bean
    public MybatisPlusInterceptor mybatisPlusInterceptor() {
        MybatisPlusInterceptor interceptor = new MybatisPlusInterceptor();
        interceptor.addInnerInterceptor(new PaginationInnerInterceptor(DbType.MYSQL));
        return interceptor;
    }
}
```

- [ ] **Step 4: Run the focused test and context test**

Run:

```bash
cd backendcloud
mvn -pl backend -Dtest=PaginationUtilsTests,BackendApplicationTests test
```

Expected: both test classes pass; the application context creates the pagination interceptor.

- [ ] **Step 5: Commit only Task 1 paths**

```bash
git add backendcloud/backend/pom.xml backendcloud/backend/src/main/java/com/kob/backend/config/MybatisPlusConfig.java backendcloud/backend/src/main/java/com/kob/backend/model/PageResponse.java backendcloud/backend/src/main/java/com/kob/backend/utils/PaginationUtils.java backendcloud/backend/src/test/java/com/kob/backend/utils/PaginationUtilsTests.java
git commit -m "feat: add pagination infrastructure" -- backendcloud/backend/pom.xml backendcloud/backend/src/main/java/com/kob/backend/config/MybatisPlusConfig.java backendcloud/backend/src/main/java/com/kob/backend/model/PageResponse.java backendcloud/backend/src/main/java/com/kob/backend/utils/PaginationUtils.java backendcloud/backend/src/test/java/com/kob/backend/utils/PaginationUtilsTests.java
```

### Task 2: Paginate the blog endpoint

**Files:**
- Modify: `backendcloud/backend/src/main/java/com/kob/backend/controller/blog/AllGetListController.java`
- Modify: `backendcloud/backend/src/main/java/com/kob/backend/service/blog/AllGetListService.java`
- Modify: `backendcloud/backend/src/main/java/com/kob/backend/service/impl/blog/AllGetListServiceImpl.java`
- Test: `backendcloud/backend/src/test/java/com/kob/backend/service/impl/blog/AllGetListServiceImplPaginationTests.java`

**Interfaces:**
- Consumes: `PaginationUtils`, `PageResponse<Blog>`, and `BlogMapper.selectPage(IPage<Blog>, Wrapper<Blog>)`.
- Produces: `AllGetListService.getAll(long page, String keyword): PageResponse<Blog>`.
- Produces: `GET /user/bot/all/getlist/?page=<value>&keyword=<value>`.

- [ ] **Step 1: Write failing service tests for page size, filtering, sorting, and overflow**

```java
package com.kob.backend.service.impl.blog;

import com.baomidou.mybatisplus.core.metadata.IPage;
import com.baomidou.mybatisplus.core.toolkit.support.SFunction;
import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.kob.backend.mapper.BlogMapper;
import com.kob.backend.model.PageResponse;
import com.kob.backend.pojo.Blog;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import java.util.Collections;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@ExtendWith(MockitoExtension.class)
class AllGetListServiceImplPaginationTests {
    @Mock private BlogMapper blogMapper;
    private AllGetListServiceImpl service;

    @BeforeEach
    void setUp() {
        service = new AllGetListServiceImpl(blogMapper);
    }

    @Test
    void requestsTenBlogsAndBuildsFilteredStableOrdering() {
        when(blogMapper.selectPage(any(Page.class), any())).thenAnswer(invocation -> {
            Page<Blog> page = invocation.getArgument(0);
            page.setTotal(12L);
            page.setRecords(Collections.singletonList(new Blog()));
            return page;
        });

        PageResponse<Blog> response = service.getAll(2L, " Java ");

        ArgumentCaptor<Page<Blog>> pageCaptor = ArgumentCaptor.forClass(Page.class);
        ArgumentCaptor<com.baomidou.mybatisplus.core.conditions.Wrapper<Blog>> wrapperCaptor = ArgumentCaptor.forClass(com.baomidou.mybatisplus.core.conditions.Wrapper.class);
        verify(blogMapper).selectPage(pageCaptor.capture(), wrapperCaptor.capture());
        assertEquals(2L, pageCaptor.getValue().getCurrent());
        assertEquals(10L, pageCaptor.getValue().getSize());
        String sql = wrapperCaptor.getValue().getSqlSegment().toLowerCase();
        assertTrue(sql.contains("title") && sql.contains("description"));
        assertTrue(sql.contains("modifytime") && sql.contains("id"));
        assertEquals(12L, response.getTotal());
        assertEquals(2L, response.getTotalPages());
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
```

- [ ] **Step 2: Run the blog pagination tests and verify RED**

Run:

```bash
cd backendcloud
mvn -pl backend -Dtest=AllGetListServiceImplPaginationTests test
```

Expected: compilation fails because the constructor and paginated `getAll` signature do not exist.

- [ ] **Step 3: Implement the blog service and controller contract**

Change the interface to:

```java
PageResponse<Blog> getAll(long page, String keyword);
```

Add constructor injection and implement the service with this algorithm:

```java
private static final long PAGE_SIZE = 10L;

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
            .or().like(Blog::getDescription, normalizedKeyword));
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
```

Change the controller method to:

```java
@GetMapping("/user/bot/all/getlist/")
public PageResponse<Blog> getAll(
        @RequestParam(defaultValue = "1") String page,
        @RequestParam(defaultValue = "") String keyword) {
    return allGetListService.getAll(PaginationUtils.parsePage(page), keyword);
}
```

- [ ] **Step 4: Run the focused blog tests**

Run:

```bash
cd backendcloud
mvn -pl backend -Dtest=AllGetListServiceImplPaginationTests,PaginationUtilsTests test
```

Expected: all focused tests pass.

- [ ] **Step 5: Commit only the blog pagination paths**

```bash
git add backendcloud/backend/src/main/java/com/kob/backend/controller/blog/AllGetListController.java backendcloud/backend/src/main/java/com/kob/backend/service/blog/AllGetListService.java backendcloud/backend/src/main/java/com/kob/backend/service/impl/blog/AllGetListServiceImpl.java backendcloud/backend/src/test/java/com/kob/backend/service/impl/blog/AllGetListServiceImplPaginationTests.java
git commit -m "feat: paginate blog queries" -- backendcloud/backend/src/main/java/com/kob/backend/controller/blog/AllGetListController.java backendcloud/backend/src/main/java/com/kob/backend/service/blog/AllGetListService.java backendcloud/backend/src/main/java/com/kob/backend/service/impl/blog/AllGetListServiceImpl.java backendcloud/backend/src/test/java/com/kob/backend/service/impl/blog/AllGetListServiceImplPaginationTests.java
```

### Task 3: Paginate the topic endpoint

**Files:**
- Modify: `backendcloud/backend/src/main/java/com/kob/backend/controller/manage/topic/GetListTopicController.java`
- Modify: `backendcloud/backend/src/main/java/com/kob/backend/service/manage/topic/GetListTopicService.java`
- Modify: `backendcloud/backend/src/main/java/com/kob/backend/service/impl/manage/topic/GetListTopicServiceImpl.java`
- Test: `backendcloud/backend/src/test/java/com/kob/backend/service/impl/manage/topic/GetListTopicServiceImplPaginationTests.java`

**Interfaces:**
- Consumes: `PaginationUtils`, `PageResponse<Topic>`, and `TopicMapper.selectPage(IPage<Topic>, Wrapper<Topic>)`.
- Produces: `GetListTopicService.getList(long page, String keyword): PageResponse<Topic>`.
- Produces: `GET /oj/topic/getlist/?page=<value>&keyword=<value>`.

- [ ] **Step 1: Write failing topic pagination tests**

Create the complete Mockito test class:

```java
package com.kob.backend.service.impl.manage.topic;

import com.baomidou.mybatisplus.extension.plugins.pagination.Page;
import com.kob.backend.mapper.TopicMapper;
import com.kob.backend.model.PageResponse;
import com.kob.backend.pojo.Topic;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.extension.ExtendWith;
import org.mockito.ArgumentCaptor;
import org.mockito.Mock;
import org.mockito.junit.jupiter.MockitoExtension;

import java.util.Collections;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

@ExtendWith(MockitoExtension.class)
class GetListTopicServiceImplPaginationTests {
    @Mock private TopicMapper topicMapper;
    private GetListTopicServiceImpl service;

    @BeforeEach
    void setUp() {
        service = new GetListTopicServiceImpl(topicMapper);
    }

    @Test
    void requestsTwentyTopicsAndBuildsIdTitleDescriptionSearch() {
        when(topicMapper.selectPage(any(Page.class), any())).thenAnswer(invocation -> {
            Page<Topic> page = invocation.getArgument(0);
            page.setTotal(25L);
            page.setRecords(Collections.singletonList(new Topic()));
            return page;
        });

        PageResponse<Topic> response = service.getList(2L, "12");

        ArgumentCaptor<Page<Topic>> pageCaptor = ArgumentCaptor.forClass(Page.class);
        ArgumentCaptor<com.baomidou.mybatisplus.core.conditions.Wrapper<Topic>> wrapperCaptor = ArgumentCaptor.forClass(com.baomidou.mybatisplus.core.conditions.Wrapper.class);
        verify(topicMapper).selectPage(pageCaptor.capture(), wrapperCaptor.capture());
        assertEquals(2L, pageCaptor.getValue().getCurrent());
        assertEquals(20L, pageCaptor.getValue().getSize());
        String sql = wrapperCaptor.getValue().getSqlSegment().toLowerCase();
        assertTrue(sql.contains("id") && sql.contains("title") && sql.contains("description"));
        assertTrue(sql.contains("order by") && sql.contains("asc"));
        assertEquals(2L, response.getCurrentPage());
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
        assertEquals(20L, response.getPageSize());
    }
}
```

- [ ] **Step 2: Run the topic test and verify RED**

Run:

```bash
cd backendcloud
mvn -pl backend -Dtest=GetListTopicServiceImplPaginationTests test
```

Expected: compilation fails because the paginated `getList` signature and constructor do not exist.

- [ ] **Step 3: Implement topic pagination and URL parameters**

Change the interface to:

```java
PageResponse<Topic> getList(long page, String keyword);
```

Use constructor injection and implement:

```java
private static final long PAGE_SIZE = 20L;

@Override
public PageResponse<Topic> getList(long page, String keyword) {
    long requestedPage = Math.max(1L, page);
    String normalizedKeyword = PaginationUtils.normalizeKeyword(keyword);
    LambdaQueryWrapper<Topic> query = new LambdaQueryWrapper<>();
    if (!normalizedKeyword.isEmpty()) {
        Integer topicId = parseTopicId(normalizedKeyword);
        query.and(item -> {
            if (topicId != null) item.eq(Topic::getId, topicId).or();
            item.like(Topic::getTitle, normalizedKeyword)
                .or().like(Topic::getDescription, normalizedKeyword);
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
    try {
        return Integer.valueOf(keyword);
    } catch (NumberFormatException exception) {
        return null;
    }
}
```

Change the controller method to parse the two request parameters:

```java
@GetMapping("/oj/topic/getlist/")
public PageResponse<Topic> getlist(
        @RequestParam(defaultValue = "1") String page,
        @RequestParam(defaultValue = "") String keyword) {
    return getListTopicService.getList(PaginationUtils.parsePage(page), keyword);
}
```

- [ ] **Step 4: Run both backend pagination test classes**

Run:

```bash
cd backendcloud
mvn -pl backend -Dtest=AllGetListServiceImplPaginationTests,GetListTopicServiceImplPaginationTests,PaginationUtilsTests test
```

Expected: all tests pass.

- [ ] **Step 5: Commit only the topic pagination paths**

```bash
git add backendcloud/backend/src/main/java/com/kob/backend/controller/manage/topic/GetListTopicController.java backendcloud/backend/src/main/java/com/kob/backend/service/manage/topic/GetListTopicService.java backendcloud/backend/src/main/java/com/kob/backend/service/impl/manage/topic/GetListTopicServiceImpl.java backendcloud/backend/src/test/java/com/kob/backend/service/impl/manage/topic/GetListTopicServiceImplPaginationTests.java
git commit -m "feat: paginate topic queries" -- backendcloud/backend/src/main/java/com/kob/backend/controller/manage/topic/GetListTopicController.java backendcloud/backend/src/main/java/com/kob/backend/service/manage/topic/GetListTopicService.java backendcloud/backend/src/main/java/com/kob/backend/service/impl/manage/topic/GetListTopicServiceImpl.java backendcloud/backend/src/test/java/com/kob/backend/service/impl/manage/topic/GetListTopicServiceImplPaginationTests.java
```

### Task 4: Shared frontend pagination state helpers

**Files:**
- Create: `web/src/utils/pagination.mjs`
- Test: `web/tests/unit/pagination.test.mjs`

**Interfaces:**
- Produces: `paginationQuery(page, keyword): {page: number, keyword: string}`.
- Produces: `clampPage(page, totalPages): number`.
- Produces: `normalizePageResponse(response, fallbackPageSize): {records, currentPage, pageSize, total, totalPages}`; throws `TypeError('分页数据格式不正确')` for an invalid envelope.

- [ ] **Step 1: Write the failing frontend utility tests**

```js
import test from 'node:test'
import assert from 'node:assert/strict'
import { clampPage, normalizePageResponse, paginationQuery } from '../../src/utils/pagination.mjs'

test('builds trimmed current-page query parameters', () => {
  assert.deepEqual(paginationQuery(3, ' Java '), { page: 3, keyword: 'Java' })
  assert.deepEqual(paginationQuery(0, ''), { page: 1, keyword: '' })
})

test('clamps numeric page input to available pages', () => {
  assert.equal(clampPage(-1, 5), 1)
  assert.equal(clampPage(9, 5), 5)
  assert.equal(clampPage('abc', 5), 1)
  assert.equal(clampPage(3, 0), 1)
})

test('normalizes a page response without retaining other pages', () => {
  const page = normalizePageResponse({
    records: [{ id: 11 }], currentPage: 2, pageSize: 10, total: 21, totalPages: 3
  }, 10)
  assert.deepEqual(page.records, [{ id: 11 }])
  assert.equal(page.currentPage, 2)
  assert.equal(page.totalPages, 3)
})

test('rejects a response without current-page records', () => {
  assert.throws(() => normalizePageResponse([], 10), /分页数据格式不正确/)
})
```

- [ ] **Step 2: Run the utility tests and verify RED**

Run:

```bash
cd web
node --test tests/unit/pagination.test.mjs
```

Expected: module-not-found failure for `src/utils/pagination.mjs`.

- [ ] **Step 3: Implement the shared helpers**

```js
export function clampPage(page, totalPages) {
  const maximum = Number.isFinite(Number(totalPages)) && Number(totalPages) > 0
    ? Math.floor(Number(totalPages))
    : 1
  const parsed = Number.parseInt(page, 10)
  if (!Number.isFinite(parsed) || parsed < 1) return 1
  return Math.min(parsed, maximum)
}

export function paginationQuery(page, keyword) {
  return {
    page: Math.max(1, Number.parseInt(page, 10) || 1),
    keyword: String(keyword || '').trim()
  }
}

export function normalizePageResponse(response, fallbackPageSize) {
  if (!response || !Array.isArray(response.records)) {
    throw new TypeError('分页数据格式不正确')
  }
  const totalPages = Math.max(0, Number.parseInt(response.totalPages, 10) || 0)
  return {
    records: response.records,
    currentPage: clampPage(response.currentPage, totalPages),
    pageSize: Math.max(1, Number.parseInt(response.pageSize, 10) || fallbackPageSize),
    total: Math.max(0, Number.parseInt(response.total, 10) || 0),
    totalPages
  }
}
```

- [ ] **Step 4: Run the focused frontend tests**

Run:

```bash
cd web
node --test tests/unit/pagination.test.mjs
```

Expected: 4 tests pass.

- [ ] **Step 5: Commit only the helper and its tests**

```bash
git add web/src/utils/pagination.mjs web/tests/unit/pagination.test.mjs
git commit -m "feat: add frontend pagination helpers" -- web/src/utils/pagination.mjs web/tests/unit/pagination.test.mjs
```

### Task 5: Paginate the blog page without changing detail behavior

**Files:**
- Modify: `web/src/views/pk/PkIndexView.vue`
- Modify: `web/tests/unit/BlogListNavigation.test.mjs`

**Interfaces:**
- Consumes: `paginationQuery`, `clampPage`, and `normalizePageResponse`.
- Sends: `{page: requestedPage, keyword: activeKeyword}` as jQuery GET query parameters.
- Stores: only `page.records` in `records`.

- [ ] **Step 1: Extend the existing source test and verify RED**

Add assertions that require the page-only API flow and controls:

```js
test('requests and renders only the current blog page', () => {
  assert.match(source, /data:\s*paginationQuery\(requestedPage, activeKeyword\.value\)/)
  assert.match(source, /records\.value\s*=\s*page\.records/)
  assert.doesNotMatch(source, /filteredRecords|\.slice\(/)
  assert.match(source, /const currentPage = ref\(1\)/)
  assert.match(source, /const totalPages = ref\(0\)/)
})

test('renders bottom-right accessible blog pagination controls', () => {
  assert.match(source, /class="pagination-controls[^"]*"/)
  assert.match(source, /aria-label="上一页"/)
  assert.match(source, /type="number"/)
  assert.match(source, /aria-label="跳转页码"/)
  assert.match(source, /aria-label="下一页"/)
  assert.match(source, /justify-content:\s*flex-end/)
})

test('resets blog searches to page one and disables boundary arrows', () => {
  assert.match(source, /activeKeyword\.value = searchKeyword\.value\.trim\(\)/)
  assert.match(source, /getBlogList\(1\)/)
  assert.match(source, /:disabled="loading \|\| currentPage <= 1"/)
  assert.match(source, /:disabled="loading \|\| totalPages === 0 \|\| currentPage >= totalPages"/)
})
```

- [ ] **Step 2: Run the blog view test and verify RED**

Run:

```bash
cd web
node --test tests/unit/BlogListNavigation.test.mjs
```

Expected: new pagination assertions fail while the existing navigation assertions pass.

- [ ] **Step 3: Replace local filtering with page state and backend queries**

Import the shared utilities and add:

```js
const currentPage = ref(1)
const pageInput = ref(1)
const total = ref(0)
const totalPages = ref(0)
const activeKeyword = ref('')

const applyPage = resp => {
  const page = normalizePageResponse(resp, 10)
  records.value = page.records
  currentPage.value = page.currentPage
  pageInput.value = page.currentPage
  total.value = page.total
  totalPages.value = page.totalPages
}

const getBlogList = (requestedPage = currentPage.value) => {
  loading.value = true
  error.value = null
  $.ajax({
    url: 'http://127.0.0.1:3000/user/bot/all/getlist/',
    type: 'GET',
    headers: { Authorization: 'Bearer ' + store.state.user.token },
    data: paginationQuery(requestedPage, activeKeyword.value),
    success(resp) {
      try {
        applyPage(resp)
      } catch (responseError) {
        records.value = []
        total.value = 0
        totalPages.value = 0
        currentPage.value = 1
        pageInput.value = 1
        error.value = responseError.message
      }
    },
    error(jqXHR, textStatus, errorThrown) {
      error.value = listRequestErrorMessage(jqXHR, errorThrown)
    },
    complete() { loading.value = false }
  })
}

const listRequestErrorMessage = (jqXHR, errorThrown) => {
  if (jqXHR.status === 0) return '无法连接到服务器'
  if (jqXHR.status === 401) return '登录已过期'
  if (jqXHR.status === 404) return 'API接口不存在'
  return `错误: ${jqXHR.status} ${errorThrown || ''}`.trim()
}

const handleSearch = () => {
  activeKeyword.value = searchKeyword.value.trim()
  getBlogList(1)
}
const changePage = offset => getBlogList(currentPage.value + offset)
const goToPage = value => getBlogList(clampPage(value, totalPages.value))
```

Remove `filteredRecords`; render `records`; display `total`; return all new refs and handlers from `setup()`.

Replace the footer with this structure while retaining the count text:

```html
<div class="card-footer pagination-footer">
  <span class="text-muted">共 {{ total }} 篇文章</span>
  <div class="pagination-controls" aria-label="博客分页">
    <button type="button" class="page-arrow" aria-label="上一页"
            :disabled="loading || currentPage <= 1" @click="changePage(-1)">←</button>
    <input v-model.number="pageInput" type="number" min="1" :max="Math.max(totalPages, 1)"
           aria-label="跳转页码" :disabled="loading || totalPages === 0"
           @change="goToPage(pageInput)" @keyup.enter="goToPage(pageInput)">
    <button type="button" class="page-arrow" aria-label="下一页"
            :disabled="loading || totalPages === 0 || currentPage >= totalPages"
            @click="changePage(1)">→</button>
  </div>
</div>
```

Add these scoped footer rules:

```css
.pagination-footer {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 1rem;
}
.pagination-controls {
  display: flex;
  justify-content: flex-end;
  align-items: center;
  gap: 0.5rem;
}
.pagination-controls input {
  width: 72px;
  height: 36px;
  text-align: center;
  border: 1px solid #ced4da;
  border-radius: 6px;
}
.page-arrow {
  width: 36px;
  height: 36px;
  border: 1px solid #0d6efd;
  border-radius: 6px;
  background: #fff;
  color: #0d6efd;
}
.page-arrow:disabled {
  border-color: #adb5bd;
  color: #adb5bd;
  cursor: not-allowed;
}
@media (max-width: 576px) {
  .pagination-footer { flex-wrap: wrap; }
  .pagination-controls { width: 100%; }
}
```

- [ ] **Step 4: Run blog and shared pagination tests**

Run:

```bash
cd web
node --test tests/unit/pagination.test.mjs tests/unit/BlogListNavigation.test.mjs
```

Expected: all new and existing tests pass.

- [ ] **Step 5: Commit only the blog frontend paths**

```bash
git add web/src/views/pk/PkIndexView.vue web/tests/unit/BlogListNavigation.test.mjs
git commit -m "feat: paginate blog list" -- web/src/views/pk/PkIndexView.vue web/tests/unit/BlogListNavigation.test.mjs
```

### Task 6: Paginate the topic page

**Files:**
- Modify: `web/src/views/ranklist/RanKlistIndexView.vue`
- Modify: `web/tests/unit/ProblemListNavigation.test.mjs`

**Interfaces:**
- Consumes: `paginationQuery`, `clampPage`, and `normalizePageResponse` from `web/src/utils/pagination.mjs`.
- Sends: `{page: requestedPage, keyword: activeKeyword}` to `/oj/topic/getlist/`.
- Stores: only `page.records` in `problems`.

- [ ] **Step 1: Extend the existing topic source tests and verify RED**

Add these topic-specific tests:

```js
test('requests and renders only the current topic page', () => {
  assert.match(source, /data:\s*paginationQuery\(requestedPage, activeKeyword\.value\)/)
  assert.match(source, /problems\.value\s*=\s*page\.records/)
  assert.doesNotMatch(source, /filteredProblems|\.slice\(/)
  assert.match(source, /normalizePageResponse\(resp, 20\)/)
})

test('renders bottom-right accessible topic pagination controls', () => {
  assert.match(source, /aria-label="上一页"/)
  assert.match(source, /aria-label="跳转页码"/)
  assert.match(source, /aria-label="下一页"/)
  assert.match(source, /justify-content:\s*flex-end/)
})

test('resets topic searches to page one', () => {
  assert.match(source, /activeKeyword\.value = searchKeyword\.value\.trim\(\)/)
  assert.match(source, /getProblemList\(1\)/)
})
```

- [ ] **Step 2: Run the topic view test and verify RED**

Run:

```bash
cd web
node --test tests/unit/ProblemListNavigation.test.mjs
```

Expected: pagination assertions fail while existing row-navigation tests pass.

- [ ] **Step 3: Implement topic page-only state and controls**

Import the helpers and add the complete page state and request flow:

```js
const currentPage = ref(1)
const pageInput = ref(1)
const total = ref(0)
const totalPages = ref(0)
const activeKeyword = ref('')

const applyPage = resp => {
  const page = normalizePageResponse(resp, 20)
  problems.value = page.records
  currentPage.value = page.currentPage
  pageInput.value = page.currentPage
  total.value = page.total
  totalPages.value = page.totalPages
}

const getProblemList = (requestedPage = currentPage.value) => {
  loading.value = true
  error.value = null
  const headers = {}
  if (store.state.user.token && store.state.user.token.trim()) {
    headers.Authorization = 'Bearer ' + store.state.user.token
  }
  $.ajax({
    url: 'http://127.0.0.1:3000/oj/topic/getlist/',
    type: 'GET',
    headers,
    data: paginationQuery(requestedPage, activeKeyword.value),
    success(resp) {
      try {
        applyPage(resp)
      } catch (responseError) {
        problems.value = []
        total.value = 0
        totalPages.value = 0
        currentPage.value = 1
        pageInput.value = 1
        error.value = responseError.message
      }
    },
    error(jqXHR) {
      if (jqXHR.status === 0) error.value = '无法连接到服务器'
      else if (jqXHR.status === 401) error.value = '登录已过期'
      else if (jqXHR.status === 404) error.value = 'API接口不存在'
      else error.value = `错误: ${jqXHR.status}`
    },
    complete() { loading.value = false }
  })
}

const handleSearch = () => {
  activeKeyword.value = searchKeyword.value.trim()
  getProblemList(1)
}
const changePage = offset => getProblemList(currentPage.value + offset)
const goToPage = value => getProblemList(clampPage(value, totalPages.value))
```

Render `problems` directly in the table and use `total` for both count labels. Return all page refs and handlers from `setup()`. Add this footer:

```html
<div class="card-footer pagination-footer">
  <span class="text-muted">共 {{ total }} 道题目</span>
  <div class="pagination-controls" aria-label="题库分页">
    <button type="button" class="page-arrow" aria-label="上一页"
            :disabled="loading || currentPage <= 1" @click="changePage(-1)">←</button>
    <input v-model.number="pageInput" type="number" min="1" :max="Math.max(totalPages, 1)"
           aria-label="跳转页码" :disabled="loading || totalPages === 0"
           @change="goToPage(pageInput)" @keyup.enter="goToPage(pageInput)">
    <button type="button" class="page-arrow" aria-label="下一页"
            :disabled="loading || totalPages === 0 || currentPage >= totalPages"
            @click="changePage(1)">→</button>
  </div>
</div>
```

Add the following scoped styles under the existing `.oj-card .card-footer` rule:

```css
.pagination-footer {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 1rem;
}
.pagination-controls {
  display: flex;
  justify-content: flex-end;
  align-items: center;
  gap: 0.5rem;
}
.pagination-controls input {
  width: 72px;
  height: 36px;
  text-align: center;
  border: 1px solid #ced4da;
  border-radius: 6px;
}
.page-arrow {
  width: 36px;
  height: 36px;
  border: 1px solid #0d6efd;
  border-radius: 6px;
  background: #fff;
  color: #0d6efd;
}
.page-arrow:disabled {
  border-color: #adb5bd;
  color: #adb5bd;
  cursor: not-allowed;
}
@media (max-width: 576px) {
  .pagination-footer { flex-wrap: wrap; }
  .pagination-controls { width: 100%; }
}
```

Keep `handleView`, row mouse handlers, row keyboard handlers, and difficulty helpers unchanged.

- [ ] **Step 4: Run all pagination and navigation frontend tests**

Run:

```bash
cd web
node --test tests/unit/pagination.test.mjs tests/unit/BlogListNavigation.test.mjs tests/unit/ProblemListNavigation.test.mjs
```

Expected: all tests pass.

- [ ] **Step 5: Commit only the topic frontend paths**

```bash
git add web/src/views/ranklist/RanKlistIndexView.vue web/tests/unit/ProblemListNavigation.test.mjs
git commit -m "feat: paginate topic list" -- web/src/views/ranklist/RanKlistIndexView.vue web/tests/unit/ProblemListNavigation.test.mjs
```

### Task 7: Full regression verification

**Files:**
- Verify only; do not modify unrelated files.

**Interfaces:**
- Consumes: all backend and frontend deliverables from Tasks 1-6.
- Produces: fresh evidence for tests, lint, build, response contract, and clean pagination-specific diffs.

- [ ] **Step 1: Run the complete backend module tests**

```bash
cd backendcloud
mvn -pl backend test
```

Expected: Maven exits 0 with zero failed tests.

- [ ] **Step 2: Run the complete frontend unit suite**

```bash
cd web
npm run test:unit
```

Expected: Node test runner reports zero failed tests.

- [ ] **Step 3: Run frontend lint**

```bash
cd web
npm run lint
```

Expected: ESLint exits 0 with no errors.

- [ ] **Step 4: Run the production build**

```bash
cd web
npm run build
```

Expected: Vue CLI exits 0 and creates the production bundle.

- [ ] **Step 5: Review only pagination-related changes**

```bash
git diff --check
git status --short
git diff -- backendcloud/backend/pom.xml backendcloud/backend/src/main/java/com/kob/backend/config/MybatisPlusConfig.java backendcloud/backend/src/main/java/com/kob/backend/model/PageResponse.java backendcloud/backend/src/main/java/com/kob/backend/utils/PaginationUtils.java backendcloud/backend/src/main/java/com/kob/backend/controller/blog/AllGetListController.java backendcloud/backend/src/main/java/com/kob/backend/service/blog/AllGetListService.java backendcloud/backend/src/main/java/com/kob/backend/service/impl/blog/AllGetListServiceImpl.java backendcloud/backend/src/main/java/com/kob/backend/controller/manage/topic/GetListTopicController.java backendcloud/backend/src/main/java/com/kob/backend/service/manage/topic/GetListTopicService.java backendcloud/backend/src/main/java/com/kob/backend/service/impl/manage/topic/GetListTopicServiceImpl.java web/src/utils/pagination.mjs web/src/views/pk/PkIndexView.vue web/src/views/ranklist/RanKlistIndexView.vue
```

Expected: no whitespace errors; status still shows the user's unrelated pre-existing changes without staging-state loss; reviewed diffs satisfy every requirement in the design spec.
