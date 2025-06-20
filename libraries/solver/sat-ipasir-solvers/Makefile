.PHONY: clean all

DIR_BUILD=builds
DIR_BIOTOMAS=ipasir_biotomas/sat
DIR_CADICAL=cadical
DIR_CMS=crypto-minisat
TEST=$(time)

all: \
	$(DIR_BUILD)/biotomas/libipasirlingelingbcj.so \
	$(DIR_BUILD)/biotomas/libipasirminisat220.so \
	$(DIR_BUILD)/biotomas/libipasirpicosat961.so \
	$(DIR_BUILD)/biotomas/libipasirlawa.so \
	$(DIR_BUILD)/libipasircadical.so \
	$(DIR_BUILD)/libipasircryptominisat5.so

test: $(DIR_BUILD)/biotomas/libipasirlingelingbcj.so



$(DIR_BUILD)/biotomas/libipasir%.so: $(DIR_BUILD)/biotomas/libipasir%.a
	./compile_so.sh $(DIR_BUILD)/biotomas/libipasir$*.a

.PRECIOUS: $(DIR_BUILD)/biotomas/libipasir%.a
$(DIR_BUILD)/biotomas/libipasir%.a: | $(DIR_BUILD)
	if ! [ -d $(DIR_BIOTOMAS)/$* ]; then echo "Directory $(DIR_BIOTOMAS)/$* does not exist"; exit 1; fi 	
	cd $(DIR_BIOTOMAS)/$*; make;
	mv $(DIR_BIOTOMAS)/$*/libipasir$*.a $(DIR_BUILD)/biotomas




$(DIR_BUILD)/libipasircadical.so: $(DIR_BUILD)/libipasircadical.a
	./compile_so.sh $(DIR_BUILD)/libipasircadical.a


$(DIR_BUILD)/libipasircadical.a: | $(DIR_BUILD)
	if ! [ -d $(DIR_CADICAL) ]; then echo "Directory $(DIR_CADICAL) does not exist"; exit 1; fi
	cd $(DIR_CADICAL); ./configure -fPIC && make
	mv $(DIR_CADICAL)/build/libcadical.a $(DIR_BUILD)/libipasircadical.a

$(DIR_BUILD)/libipasircryptominisat5.so: | $(DIR_BUILD)/libipasircryptominisat5.a
	./compile_so.sh $(DIR_BUILD)/libipasircryptominisat5.a

$(DIR_BUILD)/libipasircryptominisat5.a: | $(DIR_BUILD)
	if ! [ -d $(DIR_CMS) ]; then echo "Directory $(DIR_CMS) does not exist"; exit 1; fi
	cd $(DIR_CMS); make;
	cp $(DIR_CMS)/libipasircryptominisat5.a $(DIR_BUILD)


$(DIR_BUILD):
	mkdir -p $(DIR_BUILD)/biotomas


clean: \
	biotomas/clean \
	cadical/clean \
	cryptominisat5/clean
	rm -rf $(DIR_BUILD)


biotomas/clean:
	cd $(DIR_BIOTOMAS)/..; make clean

cadical/clean:
	if [ -f $(DIR_CADICAL)/makefile ]; then cd $(DIR_CADICAL) && make clean; fi

cryptominisat5/clean:
	cd $(DIR_CMS); make clean





