BUILD ?= $(PWD)/tmp
ROOT = $(PWD)

export LDFLAGS = -L$(BUILD)/prefix/lib
export CPPFLAGS = -I$(BUILD)/prefix/include


all:: build-cxxtools build-xz build-tntdb build-zimlib build-zimwriter

# --- cxxtools
configure-cxxtools:: $(BUILD)/cxxtools-configure-done

build-cxxtools:: configure-cxxtools $(BUILD)/cxxtools-build-done

$(BUILD)/cxxtools-build-done:
	cd $(BUILD)/cxxtools; make install
	touch $@

$(BUILD)/cxxtools-configure-done:
	mkdir -p $(BUILD)/cxxtools
	cd $(BUILD)/cxxtools; sh $(ROOT)/cxxtools/configure --disable-shared --with-pic --prefix $(BUILD)/prefix --disable-demos --disable-unittest
	touch $@

# --- xz
configure-xz:: $(BUILD)/xz-configure-done

build-xz:: configure-xz $(BUILD)/xz-build-done

$(BUILD)/xz-build-done:
	cd $(BUILD)/xz; make install
	touch $@

$(BUILD)/xz-configure-done:
	mkdir -p $(BUILD)/xz
	cd $(BUILD)/xz; sh $(ROOT)/xz/configure --disable-shared --with-pic --prefix $(BUILD)/prefix
	touch $@

# --- tntdb
configure-tntdb:: $(BUILD)/tntdb-configure-done

build-tntdb:: configure-tntdb $(BUILD)/tntdb-build-done build-cxxtools build-xz

$(BUILD)/tntdb-build-done:
	cd $(BUILD)/tntdb; make install
	touch $@

$(BUILD)/tntdb-configure-done:
	mkdir -p $(BUILD)/tntdb
	cd $(BUILD)/tntdb; sh $(ROOT)/tntdb/configure --disable-shared --with-pic --prefix $(BUILD)/prefix --without-postgresql --without-mysql --without-sqlite --without-doxygen
	touch $@

# --- zimlib
configure-zimlib:: $(BUILD)/zimlib-configure-done

build-zimlib:: configure-zimlib $(BUILD)/zimlib-build-done

$(BUILD)/zimlib-build-done:
	cd $(BUILD)/zimlib; make install
	touch $@

$(BUILD)/zimlib-configure-done:
	mkdir -p $(BUILD)/zimlib
	cd $(BUILD)/zimlib; sh $(ROOT)/zimlib/configure --disable-shared --with-pic --prefix $(BUILD)/prefix
	touch $@

# --- zimwriter
configure-zimwriter:: $(BUILD)/zimwriter-configure-done

build-zimwriter:: configure-zimwriter $(BUILD)/zimwriter-build-done

$(BUILD)/zimwriter-build-done:
	cd $(BUILD)/zimwriter; make install
	touch $@

$(BUILD)/zimwriter-configure-done:
	mkdir -p $(BUILD)/zimwriter
	cd $(BUILD)/zimwriter; sh $(ROOT)/zimwriter/configure --disable-shared --with-pic --prefix $(BUILD)/prefix
	touch $@
