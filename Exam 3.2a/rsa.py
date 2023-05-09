import os

from Crypto.Cipher import PKCS1_OAEP
from Crypto.Hash import SHA256
from Crypto.PublicKey import RSA
from Crypto.Signature import pkcs1_15


# ======== support functions for part b and d ========
def find_decrypt_key(message: str, dir="key_pairs/") -> str:
    # iterate through all the keys in the directory
    for file in os.listdir(dir):
        # check if the file is a private key
        if not file.endswith("priv.pem"):
            continue  # skip to the next file
        # if the decryption is successful
        elif decrypt_message(message, dir + file, "temp.txt"):
            os.remove("temp.txt")  # remove the temporary file
            return dir + file  # return the private key file name


def find_sign_key(message: str, signature: str, dir="key_pairs/") -> str:
    # iterate through all the keys in the directory
    for file in os.listdir(dir):
        # check if the file is a public key
        if not file.endswith("pub.pem"):
            continue  # skip to the next file
        # check if the public key verifies the signature
        elif verify_message(message, signature, dir + file):
            return dir + file  # return the public key file name

# ======== main functions ========
# Generate a public/private key pair using 2048 bits key length


def generate_keys(public_fname="public.pem", private_fname="private.pem") -> None:
    # generate the key pair
    key = RSA.generate(2048)

    # ======= public key =======
    # extract the public key
    public_key = key.publickey().export_key()

    # save the public key in a file called public.pem
    with open(public_fname, "wb") as file:
        file.write(public_key)

    # ======= private key =======
    # extract the private key
    private_key = key.export_key()

    # save the private key in a file called private.pem
    with open(private_fname, "wb") as file:
        file.write(private_key)

# Encrypt a message using a public key


def encrypt_message(message: bytes, pub_key_path: str, out_fname="encrypted.txt") -> None:
    # open the file to write the encrypted message
    with open(out_fname, "wb") as file:
        # read the public key from the file
        pub_key = open(pub_key_path, "rb").read()
        # import the public key and generate cipher using PKCS1_OAEP
        key = RSA.import_key(pub_key)
        cipher = PKCS1_OAEP.new(key)
        # encrypt the message with the public RSA key using PKCS1_OAEP
        ciphertext = cipher.encrypt(message)
        # write the encrypted message to the file
        file.write(ciphertext)

# Decrypt a message using a private key


def decrypt_message(message: str, priv_key_path: str, out_fname="decrypted.txt") -> bool:
    # open the file to write the decrypted message
    with open(out_fname, "wb") as f:
        # decrypt the message with the private RSA key using PKCS1_OAEP
        try:
            # import private key and generate cipher using PKCS1_OAEP
            with open(priv_key_path, "rb") as key_file:
                private_key = RSA.import_key(key_file.read())
                cipher = PKCS1_OAEP.new(private_key)

            # write the decrypted message to the file
            decrypted = cipher.decrypt(message)
            f.write(decrypted)

            print("The private key is valid.")
            return True

        except ValueError:
            # return False if decryption is unsuccessful
            print("The private key is invalid.")
            return False


# Sign a message using a private key
def sign_message(message: str, priv_key_path: str, out_fname="signed_msg.txt") -> None:
    # open the file to write the signature
    with open(out_fname, "wb") as f:
        # import private key
        with open(priv_key_path, "rb") as key_file:
            private_key = RSA.import_key(key_file.read())

        # hash the message with SHA256
        h = SHA256.new(message.encode())

        # sign the message with the private RSA key using pkcs1_15
        signature = pkcs1_15.new(private_key).sign(h)

        # write the signature to the file
        f.write(signature)

    print(f"The message was signed and saved in {out_fname}")


# Verify a message using a public key
def verify_message(message: bytes, signature: str, public_key_path: str) -> bool:
    # import public key
    with open(public_key_path, 'rb') as file:
        public_key = RSA.import_key(file.read())

    # hash the message with SHA256
    hashed_message = SHA256.new(message)

    # verify the signature with the public RSA key using pkcs1_15
    try:
        pkcs1_15.new(public_key).verify(hashed_message, signature)

        print("The signature is valid.")
        return True
    except (ValueError, TypeError):
        print("The signature is not valid.")
        return False


def print_menu() -> None:
    """Prints the menu of options"""
    print("*******************Main Menu*******************")
    print('a. Generate public and private keys')
    print('b. Find the right key and decrypt the message in sus.txt')
    print('c. Sign a message and verify it')
    print('d. Find Miss Reveille\'s key pair that she used to sign rev.txt')
    print('q. Quit')
    print('***********************************************\n')


if __name__ == "__main__":
    while True:
        print_menu()
        option = input('Choose a menu option: ')
        if option == "a":
            # part a.1: generate public and private keys
            generate_keys()

            # part a.2: ask a message to be encrypted and encrypt it
            message = input("Enter a message to be encrypted: ")
            message = message.encode()
            public_key_path = "public.pem"
            encrypt_message(message, public_key_path)

            # part a.3: decrypt that exact message and output it to a file
            #           called decrypted.txt
            private_key_path = "private.pem"
            encrypted_message = open("encrypted.txt", "rb").read()
            decrypt_message(encrypted_message, private_key_path)

        elif option == "b":
            # part b: decrypt the message given in sus.txt using one of the keys in key_pairs
            #         and output the decrypted message to a file called sus_decrypted.txt
            #         HINT: use the find_decrypt_key function to your advantage
            message = open("sus.txt", "rb").read()
            key: str = find_decrypt_key(message)

            decrypt_message(message, key, "sus_decrypted.txt")

        elif option == "c":
            # part c.1: sign a message using the private key from part a.1
            #           and export the signature to a file called signed_msg.txt
            message = input("Enter a message to be signed: ")
            private_key_path = "private.pem"
            sign_message(message, private_key_path)

            # part c.2: verify the signature of the message using
            #           the public key from part a.1
            public_key_path = "public.pem"
            signature = open("signed_msg.txt", "rb").read()
            verify_message(message, signature, public_key_path)

        elif option == "d":
            # part d: identify the real Reveille's signature
            #         by verifying the signature of the message in
            #         sus_decrypted.txt
            #         HINT:
            #         - think about how to find the correct key IRL (trial and error)
            #         - you are more than welcome to write a helper function to find the key
            #           and if you do, you can write find_sign_key() function
            #         - whatever method you use, as long as we select this option and get the
            #           correct key, you will get full credit
            message = open("sus_decrypted.txt", "rb").read()
            signature = open("rev.txt", "rb").read()
            public_key_path = find_sign_key(message, signature)
            valid_sig = verify_message(message, signature, public_key_path)
            if valid_sig:
                print(f"The signature in ${public_key_path} is valid.")
            else:
                print(f"The signature in ${public_key_path} is not valid.")

        elif option == "q":
            break
