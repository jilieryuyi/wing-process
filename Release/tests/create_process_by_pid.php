<?php
/**
 * @author yuyi
 * @created 2016/11/13 15:08
 * @email 297341015@qq.com
 */
$process_id = file_get_contents("process.pid");
//create process by pid
$process = new wing_process($process_id);
echo "command line=",$process->getCommandLine(),"\r\n";
//$process->kill();