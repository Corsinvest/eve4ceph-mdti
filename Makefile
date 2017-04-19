PACKAGE=eve4ceph-mdti
VERSION=0.0.1
DATE=$(shell LANG=en_us_8859_1; date '+%b %d, %Y')
DESTDIR=
PREFIX=/usr
SBINDIR=${PREFIX}/sbin
DOCDIR=${PREFIX}/share/doc/${PACKAGE}

DEB=${PACKAGE}_${VERSION}_all.deb

all: ${DEB}

.PHONY: dinstall
dinstall: deb
	dpkg -i ${DEB}

.PHONY: install
install:
	install -d ${DESTDIR}${SBINDIR}
	install -m 0755 ${PACKAGE} ${DESTDIR}${SBINDIR}

.PHONY: deb ${DEB}
deb ${DEB}:
	rm -rf debian
	mkdir debian

	cc ${PACKAGE}.c `pkg-config --cflags glib-2.0` -o ${PACKAGE}

	make DESTDIR=${CURDIR}/debian install
	install -d -m 0755 debian/DEBIAN
	sed -e s/@@VERSION@@/${VERSION}/ -e s/@@PACKAGE@@/${PACKAGE}/  <control.in >debian/DEBIAN/control
	install -D -m 0644 copyright debian/${DOCDIR}/copyright
	install -m 0644 changelog debian/${DOCDIR}/
	gzip -n -9 debian/${DOCDIR}/changelog
	dpkg-deb --build debian
	mv debian.deb ${DEB}
	#lintian ${DEB}
	rm -rf debian

.PHONY: clean
clean:
	rm -rf debian *.deb ${PACKAGE}-*.tar.gz dist ${PACKAGE}.o ${PACKAGE}
	find . -name '*~' -exec rm {} ';'

.PHONY: distclean
distclean: clean