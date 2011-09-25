#!/usr/bin/env python
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
import subprocess
import sys

common = {
	'cflags' : [ '-O3', '-ggdb', '-ffast-math', '-fomit-frame-pointer', '-fno-strict-aliasing', '-Wall', '-pedantic' ]
	}

archs = [
		{ 'prefix' : '', 'cflags' : [ '-m64', '-march=core2' ], 'objdump-flags' : [ '-M', 'intel-mnemonic' ] },
		{ 'prefix' : '', 'cflags' : [ '-m32', '-march=core2' ], 'objdump-flags' : [ '-M', 'intel-mnemonic' ] },
		{ 'prefix' : 'arm-linux-gnueabi-', 'cflags' : [ '-mthumb', '-mcpu=cortex-a9', '-mfpu=neon' ] },
		{ 'prefix' : 'arm-unknown-linux-gnueabi-', 'cflags' : [ '-mthumb', '-mcpu=cortex-a9', '-mfpu=neon' ] },
		{ 'prefix' : 'powerpc-unknown-linux-gnu-', 'cflags' : [ '-mcpu=G5', '-maltivec' ] },
		{ 'prefix' : 'mipsel-unknown-linux-gnu-', 'cflags' : [ '-march=mips64r2', '-mfp64', '-mips3d' ] },
		{ 'prefix' : 'sparc-unknown-linux-gnu-', 'cflags' : [ '-mcpu=v9', '-mvis' ] },
	]

def exec_cmd_chain(l):
	for cmd in l:
		print cmd
		try:
			p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
			stdout, stderr = p.communicate()
			print stdout
		except:
			print "Couldn't create subprocess for some reason."


def main():
	target = sys.argv[1]
	for a in archs:
		# Another option for dump_mix is to just add '-Wa,-a,-an,-ad' to these cflags.
		cc = [ a['prefix'] + 'gcc' ] + common['cflags'] + a['cflags'] + [ '-c', '-o', target + '.o', target]
		dump_flags = []
		if 'objdump-flags' in a:
			dump_flags = a['objdump-flags']
		dump_mix = [ a['prefix'] + 'objdump' ] + dump_flags + [ '-S', target + '.o' ]
		dump_asm = [ a['prefix'] + 'objdump' ] + dump_flags + [ '-d', target + '.o' ]
		cmds = [ cc, dump_asm, dump_mix ]
		exec_cmd_chain(cmds)
		print "=" * 80


if __name__ == "__main__":
	main()
