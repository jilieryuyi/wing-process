<?php namespace wing;
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
	 */
	public function run($output_file = null)
	{

	}

	public function wait($timeout = 0)
	{

	}
	public function getProcessId()
	{

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
     */
	public function kill()
	{

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