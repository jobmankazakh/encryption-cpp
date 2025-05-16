# xorc - Simple File XOR Encryptor/Decryptor

**xorc** is a fast, simple C++ utility for encrypting and decrypting files using a user-supplied, arbitrary-length decimal key.  
It processes all files in a directory, using a rolling XOR with your key.

## Features

- Encrypts or decrypts all files in a directory.
- Arbitrary-length decimal key, converted to a byte sequence.
- Efficient: 4MB buffer for fast SSD performance.
- Outputs to separate directories for encrypted/decrypted files.

## Usage

### 1. Build

Requires C++17 or newer.
```bash
g++ -std=c++17 xor.cpp -o xorc
```


### 2. Prepare Your Files

- To **encrypt**, place files in the `./raw` directory.
- To **decrypt**, place `.enc` files in the `./encrypted` directory.

### 3. Run

- `./xorc`


Follow the prompts:
- Enter `enc` to encrypt or `dec` to decrypt.
- Enter your decimal key (any length).

### 4. Output

- Encrypted files are saved in `./encrypted` (with `.enc` extension).
- Decrypted files are saved in `./decrypted`.

## Example

```
enc/dec: enc
digital key (decimal, arbitrary length): 12345678901234567890
```


## License

See [LICENSE](/LICENSE) for detailed usage restrictions.

---

**Author:** Sklyar Y. V. (aka JobManKazakh)
