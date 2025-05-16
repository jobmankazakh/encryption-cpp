#!/usr/bin/env php
<?php
/**
 * Simple File Encryptor/Decryptor (PHP CLI)
 *
 * Processes all files in a directory (./raw for encryption, ./encrypted for decryption)
 * using a user-supplied arbitrary-length decimal key.
 *
 * Usage:
 *   - Place raw files in ./raw directory for encryption.
 *   - Place .enc files in ./encrypted directory for decryption.
 *   - The program will output to ./encrypted (for encryption) or ./decrypted (for decryption).
 *
 * Author: Sklyar Y. V. (aka JobManKazakh)
 * License: See LICENSE file for details.
 */

// Divide decimal string by 256, return [quotient, remainder]
function divmod256($decimal) {
    $quotient = '';
    $remainder = 0;
    $len = strlen($decimal);
    for ($i = 0; $i < $len; ++$i) {
        $digit = (int)$decimal[$i];
        $value = $remainder * 10 + $digit;
        $q = intdiv($value, 256);
        $remainder = $value % 256;
        if ($quotient !== '' || $q > 0) {
            $quotient .= (string)$q;
        }
    }
    if ($quotient === '') $quotient = '0';
    return [$quotient, $remainder];
}

// Convert decimal string to array of bytes (base 256)
function decimalStringToBytes($decimal) {
    $bytes = [];
    while ($decimal !== "0") {
        list($quotient, $remainder) = divmod256($decimal);
        $bytes[] = $remainder;
        $decimal = $quotient;
    }
    if (empty($bytes)) {
        $bytes[] = 0;
    }
    return array_reverse($bytes);
}

function processFile($inputPath, $key, $mode) {
    $BUFFER_SIZE = 4 * 1024 * 1024; // 4 MB buffer
    $input = fopen($inputPath, 'rb');
    if (!$input) {
        fwrite(STDERR, "Failed to open input file: $inputPath\n");
        return;
    }

    $outputDir = ($mode === "enc") ? "./encrypted" : "./decrypted";
    if (!is_dir($outputDir)) mkdir($outputDir, 0777, true);

    $filename = basename($inputPath);
    if ($mode === "enc") {
        $outputPath = $outputDir . "/" . $filename . ".enc";
    } elseif ($mode === "dec") {
        $outputPath = $outputDir . "/" . (substr($filename, -4) === ".enc" ? substr($filename, 0, -4) : $filename);
    } else {
        fwrite(STDERR, "Unknown mode: $mode\n");
        fclose($input);
        return;
    }

    $output = fopen($outputPath, 'wb');
    if (!$output) {
        fwrite(STDERR, "Failed to open output file: $outputPath\n");
        fclose($input);
        return;
    }

    $keyLen = count($key);
    $step = 0;

    while (!feof($input)) {
        $buffer = fread($input, $BUFFER_SIZE);
        if ($buffer === false || $buffer === '') break;
        $bytes = unpack('C*', $buffer);
        $outBytes = [];
        $keyIndex = $step % $keyLen;

        foreach ($bytes as $i => $dataByte) {
            $keyByte = $key[$keyIndex];
            $resultByte = ($mode === "enc")
                ? ($dataByte + $keyByte) % 256
                : (256 + $dataByte - $keyByte) % 256;
            $outBytes[] = $resultByte;
            $keyIndex = ($keyIndex + 1) % $keyLen;
        }
        fwrite($output, pack('C*', ...$outBytes));
        $step += count($bytes);
    }

    fclose($input);
    fclose($output);

    echo "Processed: $filename -> " . basename($outputPath) . "\n";
}

// ---- MAIN ----

function prompt($msg) {
    echo $msg;
    return trim(fgets(STDIN));
}

$mode = prompt("enc/dec: ");
if ($mode !== "enc" && $mode !== "dec") {
    fwrite(STDERR, "Invalid mode. Use 'enc' or 'dec'.\n");
    exit(1);
}

$keyInputStr = prompt("digital key (decimal, arbitrary length): ");
if ($keyInputStr === "" || preg_match('/\D/', $keyInputStr)) {
    fwrite(STDERR, "Invalid key input. Must be a non-empty numeric decimal string.\n");
    exit(1);
}
$key = decimalStringToBytes($keyInputStr);

$inputDir = ($mode === "enc") ? "./raw" : "./encrypted";
if (!is_dir($inputDir)) {
    fwrite(STDERR, "Input directory does not exist: $inputDir\n");
    exit(1);
}

$start = microtime(true);

$dir = opendir($inputDir);
while (($file = readdir($dir)) !== false) {
    if ($file === "." || $file === "..") continue;
    $path = $inputDir . "/" . $file;
    if (!is_file($path)) continue;
    processFile($path, $key, $mode);
}
closedir($dir);

$end = microtime(true);
echo "Total time: " . round($end - $start, 3) . " seconds\n";
