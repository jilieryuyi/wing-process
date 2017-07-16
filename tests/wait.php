<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2017/7/16
 * Time: 12:17
 */
var_dump(class_exists("wing\\wing_process"));
$process = new \wing\wing_process(__DIR__."/process.php", __DIR__."/log.txt");
$process->run(1);
//如果这里的参数是1 process.php的输出全部会被写到 log.txt，如果是0，全部输出将被输出到终端

//得到 process.php 的退出码
$exit = $process->wait();

echo $exit,"\r\n";