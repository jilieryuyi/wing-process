<?php
/**
 * @author yuyi
 * @created 2016/11/12 21:12
 * @email 297341015@qq.com
 */

//run .php file as a deamon process
$process = new wing_process(__DIR__."/run.php",__DIR__."/process.log");
echo "process_id=",$process->run(),"\r\n";
echo "process_id=",$process->getProcessId(),"\r\n";
echo "thread_id=",$process->getThreadId(),"\r\n";
echo "command line=",$process->getCommandLine(),"\r\n";
file_put_contents("process.pid",$process->getProcessId());

//var_dump($process->kill());

//$exit = $process->wait();
//echo "process exit code=",$exit,"\r\n";

/*$process = new wing_process("D:/Wing/Release/Wing.exe",__DIR__."/Wing.log");
echo "process_id=",$process->run(),"\r\n";*/


