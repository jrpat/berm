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
	mkdir -p .build/$(dep-lxqt) ; cd $$_ ; $(cmake) $(root)/$(dep-lxqt)
	cd .build/$(dep-lxqt) ; make && make install

$(out-term): $(out-lxqt)
	mkdir -p .build/$(dep-term) ; cd $$_ ; $(cmake) $(root)/$(dep-term)
	cd .build/$(dep-term) ; make && make install


app: $(out-term)
	mkdir -p .build/qak ; cd $$_ ; $(cmake) $(root)
	cd .build/qak ; make


clean:
	rm -rf .build

reset: clean
	rm -rf .opt

