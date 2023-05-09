# define the target for running the script
.PHONY: run
run:
	# run the rsa.py script
	python3 rsa.py

# define the target for removing the files
.PHONY: clean
clean:
	# remove the private.pem file
	rm -f private.pem

	# remove the public.pem file
	rm -f public.pem

	# remove the encrypted.txt file
	rm -f encrypted.txt

	# remove the decrypted.txt file
	rm -f decrypted.txt

	# remove the sus_decrypted.txt file
	rm -f sus_decrypted.txt

	# remove the signed_msg.txt file
	rm -f signed_msg.txt

# define target for installing PyCryptodome
.PHONY: install
install:
	# install PyCryptodome
	python3 -m pip install pycryptodome
