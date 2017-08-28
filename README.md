# wing\wing_process

This library provides a better API to work with daemon processes on windows、mac and linux systems using PHP.


## Interface

	interface wing_process{
		/**
		 * @__construct all file path onle support full path
		 *
		 * @param string|int $mixed command or process_id
		 */
		public function __construct($mixed);
		/**
		 * run as deamon process
		 * @param string $output_file if $output_file is not empty and is a file, process will run as a daemon process
		 */
		public function run($output_file = '');

		public function wait();
		public function getProcessId();
		public function getThreadId();
		public function getCommandLine();
		public function kill();
		public function getMemory();
		public static function getCurrentProcessId();
	}
	
## Demo

#### run.php
	$count = 0;
	while( 1 )
	{
		echo $count,"\r\n";
		sleep(1);
		$count++;
	}

#### process.php
	//run .php file as a deamon process
	$process = new \wing\wing_process(__DIR__."/run.php");
	echo "process_id=",$process->run(__DIR__."/process.log"),"\r\n";
	echo "process_id=",$process->getProcessId(),"\r\n";
	echo "thread_id=",$process->getThreadId(),"\r\n";
	echo "command line=",$process->getCommandLine(),"\r\n";
	file_put_contents("process.pid",$process->getProcessId());

	//var_dump($process->kill());

	//$exit = $process->wait();
	//echo "process exit code=",$exit,"\r\n";

	/*$process = new wing_process("D:/Wing/Release/Wing.exe",__DIR__."/Wing.log");
	echo "process_id=",$process->run(),"\r\n";*/

	
#### create_process_by_pid.php
	$process_id = file_get_contents("process.pid");
	//create process by pid
	$process = new \wing\wing_process($process_id);
	echo "process_id=",$process->getProcessId(),"\r\n";
	echo "thread_id=",$process->getThreadId(),"\r\n";
	echo "command line=",$process->getCommandLine(),"\r\n";
	//$process->kill();

### Help
For more information QQ group 535218312	

### Release Dll
release dlls is in the root dir /dll