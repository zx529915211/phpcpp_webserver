<?php
$server = new WebServer("127.0.0.1","8888");
$server->on("request",function(){
       $text = "66666";
       ob_start();
       include "./test.html";
       $contents=ob_get_contents();
       ob_end_clean();
       return $contents;
});
$server->start();
