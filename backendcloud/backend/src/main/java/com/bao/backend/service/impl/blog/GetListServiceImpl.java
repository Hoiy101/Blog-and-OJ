package com.bao.backend.service.impl.blog;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.bao.backend.mapper.BlogMapper;
import com.bao.backend.pojo.Blog;
import com.bao.backend.pojo.User;
import com.bao.backend.service.impl.utils.UserDetailsImpl;
import com.bao.backend.service.blog.GetListService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class GetListServiceImpl implements GetListService {
    @Autowired
    private BlogMapper blogMapper;

    @Override
    public List<Blog> getList() {
        UsernamePasswordAuthenticationToken authenticationToken=
                (UsernamePasswordAuthenticationToken) SecurityContextHolder.getContext().getAuthentication();
        UserDetailsImpl loginUser = (UserDetailsImpl) authenticationToken.getPrincipal();
        User user = loginUser.getUser();

        QueryWrapper<Blog> queryWrapper = new QueryWrapper<>();
        queryWrapper.eq("user_id",user.getId());

        return blogMapper.selectList(queryWrapper);
    }
}
