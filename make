all:
	cd test && $(MAKE -f makefile_testCallGenerator)

clean:
	cd test && $(MAKE -f makefile_testCallGenerator) clean
