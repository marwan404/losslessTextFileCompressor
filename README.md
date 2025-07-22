# Lossless Text File Compressor

> Compress and decompress text files like a boss — no data lost, no rocket science needed.

---

## What is this?

A simple C++ program that **compresses** text files by replacing repeated words with indexes, then **decompresses** them back perfectly. Perfect for anyone who wants to play with basic file compression algorithms without diving into black-box libraries.

---

## Features

- Lossless compression of text files  
- Uses a frequency map to replace repeated words with indexes  
- Supports decompression to get the original file back  
- Console-based, lightweight, zero dependencies (besides C++ standard lib)  
- Easy to build and run on Windows

---

## How to use

1. Clone the repo or download the code  
2. Compile the program (check your C++ compiler or use VS)  
3. Run the executable in the same directory as your target text files  
4. Choose mode:  
   - `1` to compress a text file  
   - `2` to decompress a compressed file  
5. Enter the full path to your file when prompted  
6. Your output files will be created in the same directory (or current working directory)

---

## Example

```bash
Choose mode: [1] Compress  [2] Decompress
> 1
Please enter the path of the chosen file: C:\Users\Marwan\Desktop\test.txt
File compressed!

Choose mode: [1] Compress  [2] Decompress
> 2
Please enter the path of the compressed file: C:\Users\Marwan\Desktop\test_compressed.txt
File decompressed!
