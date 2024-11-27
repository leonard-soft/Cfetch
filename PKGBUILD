# Maintainer: leonard-soft - leoberfo@gmail.com
pkgname=cfetch
pkgver=1.0
pkgrel=1
pkgdesc="A simple program to get system information"
arch=('x86_64')
license=('MIT')
depends=('gcc' 'grep' 'pciutils' 'libx11')
source=("main.c" "LICENSE")
md5sums=('SKIP' 'SKIP')

build() {
    gcc -O3 -o main main.c -lX11
}

package() {
    install -Dm755 main "$pkgdir/usr/bin/cfetch"
    install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
