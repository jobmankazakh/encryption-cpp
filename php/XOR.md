
# xorc-php - Simple File XOR Encryptor/Decryptor (PHP CLI)

**xorc-php** is a fast, simple PHP command-line utility for encrypting and decrypting files using a user-supplied, arbitrary-length decimal key.  
It processes all files in a directory, using a rolling XOR with your key.

---

## Features

- Encrypts or decrypts all files in a directory.
- Arbitrary-length decimal key, converted to a byte sequence.
- Efficient: 4MB buffer for fast SSD performance.
- Outputs to separate directories for encrypted/decrypted files.
- Designed for use in the terminal (CLI).

---

## Usage

### 1. Requirements

- PHP 7.0 or newer with CLI support.

### 2. Prepare Your Files

- To **encrypt**, place files in the `./raw` directory.
- To **decrypt**, place `.enc` files in the `./encrypted` directory.

### 3. Run

```bash
php xor.php
```
or (if you made it executable):
```bash
./xor.php
```

You will be prompted to:
- Enter `enc` to encrypt or `dec` to decrypt.
- Enter your decimal key (any length).

### 4. Output

- Encrypted files are saved in `./encrypted` (with `.enc` extension).
- Decrypted files are saved in `./decrypted`.

---

## Example Session

```
enc/dec: enc
digital key (decimal, arbitrary length): 12345678901234567890
Processed: myfile.txt -> myfile.txt.enc
Total time: 0.123 seconds
```

---

## License

See [LICENSE](/LICENSE) for detailed usage restrictions.

---

**Author:** Sklyar Y. V. (aka JobManKazakh)
