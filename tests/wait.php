<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2017/7/16
 * Time: 12:17
 */
var_dump(class_exists("wing\\wing_process"));
$process = new \wing\wing_process(__DIR__."/process.php", __DIR__."/log.txt");
$process->run(0);

$exit = $process->wait();

echo $exit,"\r\n";