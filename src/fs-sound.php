#!/usr/bin/php
<?php
/**
 * @author franckysolo
 * 
 * @version prototype function implementation from C version
 * 
 * @see http://soundfile.sapp.org/doc/WaveFormat/
 */

// test create a wave file in PHP
define('MAX_AMP', 32767);

$timer = microtime(true);
$args = $_SERVER['argv'];

if (count($args) != 4) {
	fs_sound_usage();	
	exit;
}

array_shift($args);
list($filename, $frequency, $duration) = $args;

/**
 * Dipslay Help
 * 
 * @return void
 */
function fs_sound_usage() {	
	echo <<<EOS
fs-sound usage : 
	\e[33mfs-sound <filename> <frequency> <duration>\e[0m
ex : fs-sound audio.wav 440.0 5.0
EOS;
	echo PHP_EOL;
}

/**
 * Create a sound in wave format
 * 
 * @param string $filename
 * @param float $t1 start time
 * @param float $t2 end time
 * @param float $f frequency
 * @param float $amp amplitude
 */
function fs_sound($filename, $t1, $t2, $f, $amp) {

	// Paramètres
	$PCM = 1; 				// Compression PCM
	$channel_count 	= 2; 		// Stéréo
	$bits_per_sample = 16; 		// 16 BIT
	$sample_rate = 44100; 		// Taux d'échantillonage
	$duration = $t2;		// Durée de l'échantillon 5 sec

	// Binary file
	$handle = fopen($filename, 'wb');

	// calcul des paramètres d'entêtes
	$size = $sample_rate * $duration;
	$sub_chunk_size = ($bits_per_sample / 8) * $channel_count * $size;
	$byte_rate = $sample_rate * $channel_count * ($bits_per_sample / 8);
	$block_align = $channel_count * ($bits_per_sample / 8);

	// Entete du fichier Wave
	$headers = array (
		hexdec(bin2hex("RIFF")),
		36 + $sub_chunk_size,
		hexdec(bin2hex("WAVE")),
		hexdec(bin2hex("fmt ")),
		16,
		$PCM,
		$channel_count,
		$sample_rate,
		$byte_rate,
		$block_align,
		$bits_per_sample,
		hexdec(bin2hex("data")),
		$size * $channel_count * ($bits_per_sample / 8)
	);

	// Onde sonore de min à max
	$min = intval($t1 * $sample_rate);
	$max = intval($t2 * $sample_rate);

	// Calcul de l'amplitude de la Fréquence
	$amplitude = $amp * MAX_AMP;

	// Fréquence de l'échantillon 440Hz
	$w = 2 * M_PI * $f / $sample_rate;

	$samples = array();
	// Répartition du codage de l'onde sonore sur les canaux stéréos
	for ($n = $min; $n < $max; $n++) {
		$samples[] = (int)($amplitude *  sin($n * $w)); // left
		$samples[] = (int)($amplitude *  sin($n * $w)); // right
	}

	// On encode en binaire
	$format = array("NVNNVvvVVvvNVv*");
	$merge 	= array_merge($format, $headers, $samples);
	$string = call_user_func_array('pack', $merge);

	// On écrit dans le fichier les données binaires
	fwrite($handle, $string);
	fclose($handle);
}

// Génération d'un son a $frequence Hz pendant $duration sec
fs_sound($filename, 0.0, $duration, $frequency, 1.0);

echo "[fs-sound] : Audio file is create...\e[32m[ok]\e[0m";
echo PHP_EOL;
echo sprintf('Execution time : %s sec', round(microtime(true) - $timer, 6));
echo PHP_EOL;
exit;