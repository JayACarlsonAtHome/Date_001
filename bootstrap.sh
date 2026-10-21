#!/usr/bin/env bash
#
# Date_001 bootstrap: find (or offer to install) the tools, build, and run the tests.
#
#   1. Detect the OS family (rhel / fedora / linuxmint / ubuntu / debian).
#   2. Find a C++ compiler that can build this project. A small probe program (chrono
#      calendar, operator<=>, constexpr std::optional) is compiled with each candidate,
#      newest first, instead of guessing from version numbers. Also needs CMake 3.26+;
#      Ninja is used when present.
#   3. Anything missing: show the install command for this OS and offer it [y/N] (sudo).
#   4. Configure + build into build/<compiler><major>-release (e.g. build/gcc15-release, the
#      same folders DateRunner uses), then run the tests with ctest.
#   5. With --gui (optional): the U++ GUI build libraries ([y/N], sudo), U++ itself into
#      $HOME/upp_basefix through upp_basefix (U++ pinned to a GitHub commit plus fixes; its
#      own bootstrap lists the system packages U++ needs, [y/N] sudo), then the DateRunner
#      GUI into gui/out/DateRunner. A plain bootstrap never does this.
#
# Idempotent: re-run it any time; it rebuilds only what changed.
set -eu

ROOT="$(cd "$(dirname "$0")" && pwd)"

WANT_GUI=0
for arg in "$@"; do
  case "$arg" in
    --gui) WANT_GUI=1 ;;
    -h|--help)
      echo "usage: ./bootstrap.sh [--gui]"
      echo "  builds into ./build and runs the tests"
      echo "  --gui   also install U++ (upp_basefix, into \$HOME/upp_basefix) and build the DateRunner GUI"
      exit 0 ;;
    *) echo "unknown argument: $arg (try --help)" >&2; exit 2 ;;
  esac
done

# run only from the project directory, so ./build lands next to the sources
if [ "$(pwd -P)" != "$(cd "$ROOT" && pwd -P)" ]; then
  echo "bootstrap.sh must be run from the Date_001 directory:" >&2
  echo "  cd \"$ROOT\" && ./bootstrap.sh $*" >&2
  exit 2
fi

lc() { printf '%s' "$1" | tr '[:upper:]' '[:lower:]'; }

# --- 1. OS family ---------------------------------------------------------------------
family="unknown"
if [ -r /etc/os-release ]; then
  # shellcheck disable=SC1091
  . /etc/os-release 2>/dev/null || true
  id_l=$(lc "${ID:-}"); like_l=$(lc "${ID_LIKE:-}")
  case "$id_l" in
    linuxmint|ubuntu|debian|fedora) family="$id_l" ;;
    rhel|centos|rocky|almalinux|ol) family=rhel ;;
    *)
      case "$like_l" in
        *rhel*|*fedora*) family=rhel ;;
        *ubuntu*|*debian*) family=debian ;;
      esac ;;
  esac
fi
echo "Platform family: $family"

# install commands, per family:  recipe <tools|gui>
recipe() {
  case "$1.$family" in
    tools.rhel)   echo "sudo dnf install -y gcc-toolset-15-gcc-c++ cmake" ;;
    tools.fedora) echo "sudo dnf install -y gcc-c++ cmake ninja-build" ;;
    tools.linuxmint|tools.ubuntu|tools.debian)
                  echo "sudo apt-get install -y g++ cmake ninja-build" ;;
    gui.rhel|gui.fedora)
                  echo "sudo dnf install -y 'pkgconfig(gtk+-3.0)' 'pkgconfig(libnotify)' 'pkgconfig(freetype2)' 'pkgconfig(fontconfig)' 'pkgconfig(x11)' 'pkgconfig(xinerama)' 'pkgconfig(xrender)' 'pkgconfig(xft)' 'pkgconfig(xcb)' 'pkgconfig(xext)' bzip2-devel zlib-devel openssl-devel make" ;;
    gui.linuxmint|gui.ubuntu|gui.debian)
                  echo "sudo apt-get install -y libgtk-3-dev libnotify-dev libfreetype-dev libfontconfig-dev libx11-dev libxinerama-dev libxrender-dev libxft-dev libxcb1-dev libxext-dev libbz2-dev zlib1g-dev libssl-dev make pkg-config" ;;
    *) echo "" ;;
  esac
}

# offer_install <what> <recipe-name>: show the command, ask [y/N], run it
offer_install() {
  cmd=$(recipe "$2")
  if [ -z "$cmd" ]; then
    echo "Missing: $1. No install recipe for '$family'; install it by hand, then re-run ./bootstrap.sh"
    return 1
  fi
  echo "Missing: $1. To install:"
  echo "  $cmd"
  if [ ! -t 0 ]; then echo "(non-interactive: not installing; run that, then ./bootstrap.sh)"; return 1; fi
  printf "Install now? [y/N] "
  read -r ans || ans=""
  case "$ans" in
    [yY]|[yY][eE][sS]) sh -c "$cmd" || { echo "The install failed."; return 1; } ;;
    *) echo "Skipped."; return 1 ;;
  esac
}

# --- 2. compiler and CMake -----------------------------------------------------------
PROBE_DIR=$(mktemp -d)
trap 'rm -rf "$PROBE_DIR"' EXIT
cat > "$PROBE_DIR/probe.cpp" <<'EOF'
#include <chrono>
#include <compare>
#include <optional>
struct D { int y = 0, m = 0, d = 0; auto operator<=>(const D&) const = default; };
constexpr std::optional<int> f(int v) { return v > 0 ? std::optional<int>{v} : std::nullopt; }
int main()
{
    using namespace std::chrono;
    constexpr year_month_day ymd{year{2016} / February / 29};
    static_assert(ymd.ok() && *f(1) == 1 && D{1, 2, 3} < D{1, 2, 4});
    return weekday{sys_days{ymd}}.c_encoding() == 1 ? 0 : 1;   // a Monday
}
EOF

cxx_ok() { [ -x "$(command -v "$1" 2>/dev/null)" ] && "$1" -std=c++23 -o "$PROBE_DIR/probe" "$PROBE_DIR/probe.cpp" >/dev/null 2>&1 && "$PROBE_DIR/probe"; }

find_cxx() {
  CXX_FOUND=""
  for c in /opt/rh/gcc-toolset-15/root/usr/bin/g++ g++-15 g++-14 /opt/rh/gcc-toolset-14/root/usr/bin/g++ \
           clang++ g++-13 g++; do
    if cxx_ok "$c"; then CXX_FOUND=$(command -v "$c"); return 0; fi
  done
  return 1
}

cmake_ok() {
  v=$(cmake --version 2>/dev/null | sed -n 's/^cmake version \([0-9.]*\).*/\1/p')
  [ -n "$v" ] && [ "$(printf '3.26\n%s\n' "$v" | sort -V | head -n1)" = "3.26" ]
}

missing=""
find_cxx || missing="$missing compiler"
cmake_ok || missing="$missing cmake"
if [ -n "$missing" ]; then
  offer_install "${missing# } (a C++23 compiler and CMake 3.26+)" tools || exit 1
  find_cxx || { echo "Still no compiler that builds the probe (C++23 with <chrono> calendar)."; exit 1; }
  cmake_ok || { echo "Still no CMake 3.26+."; exit 1; }
fi
echo "Compiler: $CXX_FOUND ($("$CXX_FOUND" --version | head -n1))"
echo "CMake:    $(cmake --version | head -n1)"

# --- 4. build + test -------------------------------------------------------------------
# one folder per compiler and build type, so a build made with another compiler is never reused
kind=gcc; "$CXX_FOUND" --version 2>/dev/null | head -n1 | grep -qi clang && kind=clang
major=$("$CXX_FOUND" -dumpversion 2>/dev/null | cut -d. -f1)
BUILD_NAME="$kind$major-release"
BUILD="$ROOT/build/$BUILD_NAME"
GEN=""
command -v ninja >/dev/null 2>&1 && GEN="-G Ninja"
# a folder first configured with the other generator (ninja installed or removed since) cannot be reused
if [ -f "$BUILD/CMakeCache.txt" ]; then
  old_gen=$(sed -n 's/^CMAKE_GENERATOR:[A-Z]*=//p' "$BUILD/CMakeCache.txt")
  want_gen="Unix Makefiles"; [ -n "$GEN" ] && want_gen="Ninja"
  if [ "$old_gen" != "$want_gen" ]; then
    echo "build/$BUILD_NAME was made for $old_gen: starting it over"
    rm -rf "$BUILD"
  fi
fi
echo
echo "=== build (Release) -> build/$BUILD_NAME ==="
# shellcheck disable=SC2086
cmake -S "$ROOT" -B "$BUILD" $GEN -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="$CXX_FOUND" >/dev/null
cmake --build "$BUILD" -j"$(nproc 2>/dev/null || echo 2)"
echo
echo "=== tests ==="
test_rc=0
ctest --test-dir "$BUILD" --output-on-failure || test_rc=$?
"$BUILD/Date" --verbosity quiet 2>/dev/null | tail -n 2 || true

# --- 5. optional GUI --------------------------------------------------------------------
# U++ comes from upp_basefix (github.com/JayACarlsonAtHome/upp_basefix): U++ pinned to a GitHub
# commit, plus patches that make it build with 0 warnings and run clean under the sanitizers
# and valgrind. Its patch 0001 also fixes the GCC 15 start-up crash of plain U++ 2026.1, so the
# GUI is built with GCC. An install is $UPP_DIR/{umk,uppsrc,GCC.bm,CLANG.bm}. (Same as jac313.)
UPP_DIR="${UPP_DIR:-$HOME/upp_basefix}"
UPP_BASEFIX_REPO="${UPP_BASEFIX_REPO:-https://github.com/JayACarlsonAtHome/upp_basefix.git}"
UPP_BASEFIX_SRC="${UPP_BASEFIX_SRC:-$HOME/upp_basefix_src}"   # its clone (the script, the patches)
UPP_METHOD="GCC"

upp_gui_libs_ok() {
  command -v pkg-config >/dev/null 2>&1 \
    && pkg-config --exists gtk+-3.0 libnotify freetype2 fontconfig x11 xinerama xrender xft xcb xext
}

install_upp() {  # U++ into $UPP_DIR via upp_basefix; keeps an existing install with umk + uppsrc
  if [ -x "$UPP_DIR/umk" ] && [ -d "$UPP_DIR/uppsrc" ]; then
    echo "U++ found at $UPP_DIR ($(head -1 "$UPP_DIR/VERSION" 2>/dev/null || echo 'umk + uppsrc')): using it."; return 0
  fi
  if [ -e "$UPP_DIR" ] && [ -n "$(ls -A "$UPP_DIR" 2>/dev/null)" ]; then
    echo "$UPP_DIR exists but has no umk + uppsrc. Not touching it: move it aside and re-run."
    return 1
  fi
  if [ ! -x "$UPP_BASEFIX_SRC/upp_basefix.sh" ]; then
    command -v git >/dev/null 2>&1 || { echo "git missing: cannot fetch upp_basefix."; return 1; }
    echo "Fetching upp_basefix -> $UPP_BASEFIX_SRC..."
    git clone -q "$UPP_BASEFIX_REPO" "$UPP_BASEFIX_SRC" || { echo "git clone failed (network?)."; return 1; }
  fi
  # its bootstrap: the pinned U++ from GitHub, umk built from it, the patches; it lists the
  # system packages U++ needs and installs them only when asked (sudo)
  if ! "$UPP_BASEFIX_SRC/upp_basefix.sh" bootstrap; then
    if [ ! -t 0 ]; then echo "(non-interactive: not installing; run $UPP_BASEFIX_SRC/upp_basefix.sh bootstrap --install)"; return 1; fi
    printf "Install those packages now (sudo)? [y/N] "
    read -r ans || ans=""
    case "$ans" in
      [yY]|[yY][eE][sS]) "$UPP_BASEFIX_SRC/upp_basefix.sh" bootstrap --install || { echo "upp_basefix bootstrap failed."; return 1; } ;;
      *) echo "Skipped: U++ is not installed."; return 1 ;;
    esac
  fi
  "$UPP_BASEFIX_SRC/upp_basefix.sh" install "$UPP_DIR" || { echo "upp_basefix install failed."; return 1; }
  echo "Installed $(head -1 "$UPP_DIR/VERSION") -> $UPP_DIR"
}

build_gui() {  # [-a]: -a rebuilds every package (U++ shares one object cache, which can go stale)
  mkdir -p "$ROOT/gui/out"
  "$UPP_DIR/umk" "$ROOT/gui,$UPP_DIR/uppsrc" DateRunner "$UPP_METHOD" "${1:--}brsH" "$ROOT/gui/out/DateRunner" \
    && [ -x "$ROOT/gui/out/DateRunner" ]
}

bootstrap_gui() {
  echo
  echo "=== GUI (--gui): U++ (upp_basefix) + DateRunner ==="
  if ! upp_gui_libs_ok; then
    offer_install "the GTK3 / X11 build libraries U++ needs" gui || return 1
    upp_gui_libs_ok || { echo "The GUI libraries are still missing (pkg-config)."; return 1; }
  fi
  install_upp || return 1
  echo "Building DateRunner (umk, $UPP_METHOD)..."
  if ! build_gui -; then
    echo "Retrying with a clean rebuild (stale U++ cache)..."
    build_gui -a || { echo "DateRunner build failed."; return 1; }
  fi
  echo "DateRunner: $ROOT/gui/out/DateRunner"
}

gui_rc=0
if [ "$WANT_GUI" = 1 ]; then bootstrap_gui || gui_rc=1; fi

echo
if [ "$test_rc" = 0 ]; then echo "bootstrap: OK, all tests passed."; else echo "bootstrap: TESTS FAILED (ctest rc $test_rc)."; fi
[ "$WANT_GUI" = 1 ] && [ "$gui_rc" = 0 ] && echo "Start the GUI: ./gui/out/DateRunner"
[ "$WANT_GUI" = 0 ] && echo "For the test runner window: ./bootstrap.sh --gui"
[ "$test_rc" = 0 ] && [ "$gui_rc" = 0 ]
