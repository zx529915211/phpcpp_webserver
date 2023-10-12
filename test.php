<?php
$server = new WebServer("127.0.0.1","8888");
$server->set([
    'daemon' => 0
]);
$server->on("request",function(){
        set_error_handler(function ($errno, $errstr, $errfile, $errline){
           file_put_contents("/mnt/d/wsl_data/phpcpp-demo/phpcpp_webserver/error.log",$errstr . PHP_EOL,FILE_APPEND);
        });
//        $root_path = realpath("./");
//        file_put_contents("/mnt/d/wsl_data/phpcpp-demo/phpcpp_webserver/error.log",$root_path . PHP_EOL,FILE_APPEND);
       $text = "66666";
       ob_start();
       include "test.html";
       $contents=ob_get_contents();
       ob_end_clean();
       return $contents;
});
$server->start();
