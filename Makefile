# vim: set tw=80 :

default: app

root := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
Qt5_PATH := /opt/homebrew/Cellar/qt@5/5.15.5_1

cmake := cmake \
	-DCMAKE_PREFIX_PATH='$(Qt5_PATH)' \
	-DCMAKE_INSTALL_PREFIX='$(root)/.opt'


dep-lxqt := lxqt-build-tools
dep-term := qtermwidget

out-lxqt := .opt/share/cmake/$(dep-lxqt)
out-term := .opt/lib/libqtermwidget5.dylib


$(out-lxqt):
	mkdir -p .build/$(dep-lxqt) ; cd $$_ ; \
		$(cmake) $(root)/$(dep-lxqt) && make && make install

$(out-term): $(out-lxqt)
	cd $(dep-term) ; git checkout . ; git apply ../*.patch
	mkdir -p .build/$(dep-term) ; cd $$_ ; \
		$(cmake) $(root)/$(dep-term) && make && make install

app: $(out-term)
	mkdir -p .build/app ; cd $$_ ; $(cmake) $(root)
	cd .build/app ; make

term: $(out-term)


clean:
	rm -rf .build

reset: clean
	rm -rf .opt

