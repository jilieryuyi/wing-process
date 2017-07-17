<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2017/7/16
 * Time: 12:17
 */

$process = new \wing\wing_process(__DIR__."/process.php --author yuyi", __DIR__."/log.txt");
$process->run(1);
//如果这里的参数是1 process.php的输出全部会被写到 log.txt，如果是0，全部输出将被输出到终端

//得到 process.php 的退出码
//$exit = $process->wait();

//echo $exit,"\r\n";
//
//$process = new \wing\wing_process("D:\Program Files (x86)\Tencent\QQ\Bin\QQScLauncher.exe", __DIR__."/log.txt");
//echo $process->run(0);

echo $process->getCommandLine();