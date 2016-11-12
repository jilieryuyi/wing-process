<?php
/**
 * @author yuyi
 * @created 2016/11/12 21:12
 * @email 297341015@qq.com
 */
$process = new wing_process(__DIR__."/run.php",__DIR__."/process.log");
$process->run();