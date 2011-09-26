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
import os
import subprocess
import sys

common = {
	'cflags' : [ '-O3', '-ggdb', '-ffast-math', '-fomit-frame-pointer', '-fno-strict-aliasing', '-Wall', '-pedantic' ]
	}

archs = [
		# x86_64
		{ 'desc' : 'x86_64 generic with sse2',
		  'prefix' : '',
		  'cflags' : [ '-m64', '-mtune=generic', '-msse2' ],
		  'objdump-flags' : [ '-M', 'intel-mnemonic' ] },

		{ 'desc' : 'x86_64 generic without sse2',
		  'prefix' : '',
		  'cflags' : [ '-m64', '-mtune=generic', '-mno-sse2' ],
		  'objdump-flags' : [ '-M', 'intel-mnemonic' ] },

		# x86
		{ 'desc' : 'x86 with sse2',
		  'prefix' : '',
		  'cflags' : [ '-m32', '-mtune=i686', '-msse2' ],
		  'objdump-flags' : [ '-M', 'intel-mnemonic' ] },

		{ 'desc' : 'x86 without sse2',
		  'prefix' : '',
		  'cflags' : [ '-m32', '-mtune=i686', '-mno-sse2' ],
		  'objdump-flags' : [ '-M', 'intel-mnemonic' ] },

		# ARM
		{ 'desc' : 'ARMv7 with NEON-fp16, thumb (linaro)',
		  'prefix' : 'arm-linux-gnueabi-',
		  'cflags' : [ '-mthumb', '-march=armv7-a', '-mfpu=neon-fp16' ]},

		{ 'desc' : 'ARMv7 with NEON-fp16 (linaro)',
		  'prefix' : 'arm-linux-gnueabi-',
		  'cflags' : [ '-marm', '-march=armv7-a', '-mfpu=neon-fp16' ]},

		{ 'desc' : 'ARMv7 with NEON (linaro)',
		  'prefix' : 'arm-linux-gnueabi-',
		  'cflags' : [ '-marm', '-march=armv7-a', '-mfpu=neon' ]},

		{ 'desc' : 'ARMv7 with VFP (linaro)',
		  'prefix' : 'arm-linux-gnueabi-',
		  'cflags' : [ '-marm', '-march=armv7-a', '-mfpu=vfp' ]},

		{ 'desc' : 'ARMv7 with NEON-fp16, thumb (upstream GCC)',
		  'prefix' : 'arm-unknown-linux-gnueabi-',
		  'cflags' : [ '-mthumb', '-march=armv7-a', '-mfpu=neon-fp16' ]},

		{ 'desc' : 'ARMv7 with NEON-fp16 (upstream GCC)',
		  'prefix' : 'arm-unknown-linux-gnueabi-',
		  'cflags' : [ '-marm', '-march=armv7-a', '-mfpu=neon-fp16' ]},

		{ 'desc' : 'ARMv7 with NEON (upstream GCC)',
		  'prefix' : 'arm-unknown-linux-gnueabi-',
		  'cflags' : [ '-marm', '-march=armv7-a', '-mfpu=neon' ]},

		{ 'desc' : 'ARMv7 with VFP (upstream GCC)',
		  'prefix' : 'arm-unknown-linux-gnueabi-',
		  'cflags' : [ '-marm', '-march=armv7-a', '-mfpu=vfp' ]},

		# PowerPC
		{ 'desc' : 'PowerPC 64-bit with AltiVec',
		  'prefix' : 'powerpc-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=powerpc64', '-maltivec' ] },

		{ 'desc' : 'PowerPC 64-bit without AltiVec',
		  'prefix' : 'powerpc-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=powerpc64', '-mno-altivec' ] },

		{ 'desc' : 'PowerPC 32-bit',
		  'prefix' : 'powerpc-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=powerpc' ] },

		# MIPS
		{ 'desc' : 'MIPS 64-bit',
		  'prefix' : 'mipsel-unknown-linux-gnu-',
		  'cflags' : [ '-march=mips64r2', '-mfp64', '-mips3d' ] },

		# SPARC
		{ 'desc' : 'UltraSPARC T2 32-bit with VIS',
		  'prefix' : 'sparc-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=niagara2', '-mvis' ] },

		{ 'desc' : 'UltraSPARC T2 32-bit without VIS',
		  'prefix' : 'sparc-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=niagara2', '-mno-vis' ] },

		# SPARC64
		{ 'desc' : 'UltraSPARC T2 64-bit with VIS',
		  'prefix' : 'sparc64-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=niagara2', '-mvis' ] },

		{ 'desc' : 'UltraSPARC T2 64-bit without VIS',
		  'prefix' : 'sparc64-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=niagara2', '-mno-vis' ] },
	]

def exec_cmd_chain(l):
	for cmd in l:
		print ' '.join(cmd)
		try:
			p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
			stdout, stderr = p.communicate()
			print stdout
		except:
			print "Couldn't create subprocess for some reason."


def main():
	target = sys.argv[1]
	for a in archs:
		print "\n%s" % ("=" * 80)
		print a['desc']
		obj = target + '.o'
		# Another option for dump_mix is to just add '-Wa,-a,-an,-ad' to these cflags.
		cc = [ a['prefix'] + 'gcc' ] + common['cflags'] + a['cflags'] + [ '-c', '-o', obj, target]
		dump_flags = []
		if 'objdump-flags' in a:
			dump_flags = a['objdump-flags']
		dump_mix = [ a['prefix'] + 'objdump' ] + dump_flags + [ '-S', obj ]
		dump_asm = [ a['prefix'] + 'objdump' ] + dump_flags + [ '-d', obj ]
		cmds = [ cc, dump_asm, dump_mix ]
		print "\nCommand list:"
		for cmd in cmds:
			print '  %s' % (' '.join(cmd))
		print "=" * 80
		exec_cmd_chain(cmds)
		try:
			os.remove(obj)
		except:
			pass


if __name__ == "__main__":
	main()
