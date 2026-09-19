package com.bao.backend.service.impl.user.account;

import com.bao.backend.pojo.User;
import com.bao.backend.service.impl.utils.UserDetailsImpl;
import com.bao.backend.service.user.account.LoginService;
import com.bao.backend.utils.JwtUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.stereotype.Service;

import java.util.HashMap;
import java.util.Map;

@Service
public class LoginServiceImpl implements LoginService {

    @Autowired
    private AuthenticationManager authenticationManager;

    @Override
    public Map<String, String> getToken(String username, String password) {
        UsernamePasswordAuthenticationToken authenticationToken =
                new UsernamePasswordAuthenticationToken(username, password);

        Authentication authentication = authenticationManager.authenticate(authenticationToken);

        UserDetailsImpl loginUser = (UserDetailsImpl) authentication.getPrincipal();
        User user = loginUser.getUser();
        String jwt = JwtUtil.createJWT(user.getId().toString());

        Map<String, String> map = new HashMap<>();
        if(user.getBanned().equals("true")){
            map.put("error_message", "该账号已被封禁");
            return map;
        }
        if(jwt == null){
            map.put("error_message", "账号或密码错误");
            return map;
        }
        map.put("error_message", "success");
        map.put("token", jwt);
        System.out.println(jwt);

        return map;
    }
}
