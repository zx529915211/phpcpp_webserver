<?php
$counter = new WebServer();
$counter->start("127.0.0.1","8989",function(){
        sleep(5);
       return "HTTP/1.1 200 OK\r\n".
              "Content-Type: text/html\r\n".
              "\r\n" .
              "<h1>my first webserver</h1>";
});
