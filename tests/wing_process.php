<?php namespace wing;
define("WING_PROCESS_PHP", "/usr/bin/php");
define("WING_PROCESS_VERSION", "2.0.0");
class wing_process
{
	/**
	 * @__construct all file path onle support full path
	 *
	 * @param string|int $mixed command or process_id
	 * @param string $output_file process output will write into this file
	 */
	public function __construct($mixed)
	{

	}
	/**
	 * run as daemon process if $output_file was set
     * @return int
	 */
	public function run($output_file = null)
	{
        return 0;
	}

	/**
	 * @return int
     */
	public function wait($timeout = 0)
	{
        return 0;
	}
	/**
	 * @return int
     */
	public function getProcessId()
	{
        return 0;
	}
	public function getThreadId()
	{

	}
	/**
	 * @return string
	 */
	public function getCommandLine()
	{
		return "";
	}

	/**
	 * kill a process
     * @return bool
     */
	public function kill()
	{
        return true;
	}

	/**
	 * @return int memory use byte
     */
	public function getMemory()
	{
        return 0;
	}
	/**
	 * 得到当前进程id
     * @return int
     */
	public static function getCurrentProcessId()
    {
        return 0;
    }
}