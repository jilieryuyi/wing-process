<?php
/**
 * Created by PhpStorm.
 * User: yuyi
 * Date: 17/8/25
 * Time: 20:12
 */
while (1) {
    file_put_contents(__DIR__."/runner.log", date("Y-m-d H:i:s"));
    sleep(1);
}