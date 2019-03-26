# Modele de Makefile : a adapter à vos besoins, mais rapport.pdf doit rester en haut du repertoire et
# le fichier a tester doit correspondre aux spécifications et etre dans le repertoire ../TPAODjustify/bin/APDjustify

CC = clang -O3 -std=gnu99 -Wall -Wextra -g -lm

all: bin/AODjustify 

doc: src/AODjustify.c  # A COMPLETER
	doxygen Doxyfile

eval: bin/AODjustify
	cd ..; source /matieres/4MMAOD6/install/AODjustify-calcul-note.bash # 2&> /tmp/foo

bin/AODjustify: src/AODjustify.c  # A COMPLETER
	$(CC) -o bin/AODjustify src/AODjustify.c
	# chmod 0111 bin/AODjustify


clean:
	rm bin/AODjustify  

