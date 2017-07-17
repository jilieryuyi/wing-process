<?php

$key = 1;//ftok(__FILE__, 'h');
$mode = 'c';
$permissions = 0777;
$size = 1024;
$shmid = shmop_open($key, $mode, $permissions, $size);

var_dump($shmid);

//shmop_write($shmid, "Hello World!", 0);
$len = strlen(("Hello World!"));
var_dump((shmop_read($shmid, 0, $len)));

//shmop_delete($shmid);
shmop_close($shmid);