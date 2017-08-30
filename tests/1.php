<?php
/**
 * Created by PhpStorm.
 * User: yuyi
 * Date: 17/8/31
 * Time: 06:46
 */
$p = new \wing\wing_process(__DIR__."/1 2.php");
echo $p->run();