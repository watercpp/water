#! /usr/bin/php
<?php

array_shift($argv); // scriptname
foreach($argv as $arg) {
	$json = json_decode(file_get_contents($arg));
	echo $arg . "\n";
	print_r($json);
	echo "\n\n";
	}