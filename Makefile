# just move into the temp directory and run qmake from there, this
# keeps all the temporary and object files out of the main directory
bin/main:
	@cd 3rdParty; make 
	@mkdir -p temp
	@cd temp; qmake ../build/build.pro; make

clean:
	rm -fr temp

clean3rdParty:
	@cd 3rdParty; make clean
	rm ./3rdParty/glimg/lib -rf

.PHONY:bin/main clean

