<?php
/**
 * Created by PhpStorm.
 * User: yuyi
 * Date: 17/8/25
 * Time: 20:07
 */
//echo WING_PROCESS_PHP, "\r\n";

$process = new \wing\wing_process(__DIR__."/runner.php");
echo $process->run(__DIR__."/log.log"),"\r\n";
//echo $process->wait();
echo \wing\wing_process::getCurrentProcessId(),"\r\n";
echo $process->getCommandLine(),"\r\n";
$process->kill();
