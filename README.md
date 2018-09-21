```sh
./configure --prefix=/usr/local/nginx --add-module=/somewhere/ngx_go_bridge_module  --with-debug
make
sudo make install

/usr/local/nginx/sbin/nginx
```

```
http {
  ...
  server {
    ...
    location ~ ^/dummy {
      go_bridge lib_name function_name;
    }
  }
}
```

