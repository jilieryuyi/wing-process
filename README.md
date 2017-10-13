# wing\wing_process

This library provides a better API to work with daemon processes on windows、mac and linux systems using PHP.


## Prototype
    namespace wing;
	class wing_process
	{
		/**
		 * __construct only support full path like "C:/php/runer.php" or "C:/exe/some.exe"
		 * @param string|int $mixed command or process_id
		 */
		public function __construct($mixed);
		
		/**
		 * run as deamon process
		 * @param string $output_file   if $output_file is not empty and is a file, process will run as a daemon process
		 * @return int process id
		 */
		public function run($output_file = '');

		public function wait();
		public function getProcessId();
		public function getThreadId();
		public function getCommandLine();
		public function kill();
		
		/**
		 * return memory useage, unit k
		 * @return int
		 */
		public function getMemory();
		public static function getCurrentProcessId();
	}
	
## Demo

#### run.php
	$count = 0;
	while (1) {
		echo $count, "\r\n";
		sleep(1);
		$count++;
	}

#### process.php
	//run .php file as a deamon process
	$process = new \wing\wing_process(__DIR__."/run.php");
	
	echo "process_id = ",   $process->run(__DIR__."/process.log"), "\r\n";
	echo "process_id = ",   $process->getProcessId(), "\r\n";
	echo "thread_id = ",    $process->getThreadId(), "\r\n";
	echo "command line = ", $process->getCommandLine(), "\r\n";
	
	file_put_contents("process.pid", $process->getProcessId());

	//var_dump($process->kill());
	//$exit_code = $process->wait();
	//echo "process exit code=", $exit_code, "\r\n";

	/*
	//create process in windows by using a binary exe file
	$process = new wing_process("D:/Wing/Release/Wing.exe");
	echo "process_id = ", $process->run(__DIR__."/Wing.log"), "\r\n";
	*/

	
#### create_process_by_pid.php
    //create a process by an exists process pid
	$process_id = file_get_contents("process.pid");
	$process    = new \wing\wing_process($process_id);
	
	echo "process_id = ",    $process->getProcessId(), "\r\n";
	echo "thread_id = ",     $process->getThreadId(), "\r\n";
	echo "command line = ",  $process->getCommandLine(),"\r\n";
	//$process->kill();

### Help
For more information QQ group 535218312	

### Release Dll
release dlls is in the root dir /dll