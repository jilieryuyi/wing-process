<?php
/**
 * @author yuyi
 * @created 2016/11/13 17:33
 * @email 297341015@qq.com
 */
interface wing_process{
    /**
     * @__construct all file path onle support full path
     *
     * @param string|int $mixed command or process_id
     * @param string $output_file process output will write into this file
     */
    public function __construct( $mixed, $output_file = '');
    /**
     * @run as deamon process
     */
    public function run();

    public function wait();
    public function getProcessId();
    public function getThreadId();
    public function getCommandLine();
    public function kill();
}