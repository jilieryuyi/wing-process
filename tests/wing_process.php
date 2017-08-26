<?php namespace wing;
class wing_process
{
	/**
	 * @__construct all file path onle support full path
	 *
	 * @param string|int $mixed command or process_id
	 * @param string $output_file process output will write into this file
	 */
	public function __construct( $mixed, $output_file = '')
	{

	}
	/**
	 * @run as deamon process
	 */
	public function run( $redirect_ouput = 1)
	{

	}

	public function wait()
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
	public function kill()
	{

	}
	public function getMemory()
	{

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