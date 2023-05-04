# Quiz 4.2.b: Digital signatures
The Python Crypto Library is a third-party library that provides cryptographic primitives and algorithms for Python programming language. It is also known as the PyCrypto library. The library supports a wide range of cryptographic algorithms and protocols, including symmetric and asymmetric encryption, digital signatures, hash functions, key derivation functions, and more.

### Task
Fix the broken script `rsa_sign.py`, to enable it to sign the message hash correctly using the provided key `private.pem`.
Note that the key `private.pem` was generated using the following command:

```
key = RSA.generate(2048)
pem = key.export_key(format='PEM', passphrase='dees')
f = open('private.pem','wb')
f.write(pem)
f.close() 
```


## Important notes:
1. Usage: `sudo python3 rsa_sign.py`
1. Your script is expected to print the signature of the hash of the plaintext message.
1. Your program will be manually tested for correctness.
1. Your program should execute with no errors and warnings.