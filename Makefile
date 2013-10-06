# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/main:
	@mkdir -p temp
	@cd temp; qmake ../build/build.pro; make

clean:
	rm -fr temp

.PHONY:bin/main clean

