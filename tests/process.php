<?php
/**
 * Created by PhpStorm.
 * User: Administrator
 * Date: 2017/7/16
 * Time: 12:18
 */
$c  = 0;
while(1) {
	echo "hello\r\n";
	if ($c++ > 1000) break;
}
exit(999);