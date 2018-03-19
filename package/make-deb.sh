#!/bin/bash
#
# package/make-deb.sh - Make debian package
#
# Usage:
#
#   vagrant init debian/jessie64
#   vagrant ssh --command /vagrant/package/make-deb.sh

set -e

pkgname=libscep
upstreamver=0.3
buildname=$pkgname-$upstreamver

source_package=$pkgname
upstream_version=0.3
downstream_version=1
downstream_arch=amd64
origname=${source_package}_${upstream_version}
origtarball=$origname.orig.tar.gz

pkgdeps="git-core dh-make cmake openssl pkg-config check liburiparser1 liburiparser-dev libcurl4-openssl-dev libssl-dev dh-make-perl"

missingdeps=

for i in $pkgdeps; do
    if ! dpkg -l $i >/dev/null 2>&1; then
        missingdeps="$missingdeps $i"
    fi
done

if [ -n "$missingdeps" ]; then
    echo "INFO: Installing missing dependencies: $missingdeps"
    sudo aptitude update
    sudo aptitude install -y $missingdeps
fi

# Confirm the dependencies were actually installed...
for i in $pkgdeps; do
    if ! dpkg -l $i >/dev/null 2>&1; then
        echo "Failed to install package $i -- aborting" 1>&2
        exit 1
    fi
done

rm -rf ~/build
mkdir -p ~/build/$buildname
(cd /vagrant && git archive --output=$HOME/build/$origtarball HEAD)
(cd ~/build/$buildname && tar xzf $HOME/build/$origtarball)
(cd ~/build/$buildname && dpkg-buildpackage -us -uc)

echo "INFO: Installing library package"
sudo dpkg -i ~/build/${origname}-${downstream_version}_${downstream_arch}.deb

(cd /vagrant/src/clients/perl/Crypt-LibSCEP && git archive --output=$HOME/build/Crypt-LibSCEP-${upstream_version}.tar.gz --prefix=Crypt-LibSCEP-${upstream_version}/ HEAD .)
(cd ~/build && tar xzf Crypt-LibSCEP-${upstream_version}.tar.gz)
(cd ~/build && dh-make-perl Crypt-LibSCEP-${upstream_version}/)
(cd ~/build/Crypt-LibSCEP-${upstream_version} && dpkg-buildpackage -us -uc)
echo "INFO: Installing Perl library package"
sudo dpkg -i ~/build/libcrypt-libscep-perl_${upstream_version}-1_amd64.deb

echo "INFO: running our own tests"
# Make sure we use the installed modules
cp -a /vagrant/src/clients/perl/Crypt-LibSCEP/t ~/build/
(cd ~/build && prove t/*.t)

echo "INFO: Copy debian packages to host system"
cp -v ~/build/*.deb /vagrant/

