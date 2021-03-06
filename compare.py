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
	'cflags' : [ '-Os', '-ggdb', '-ftree-vectorize', '-ftree-vectorizer-verbose=3', '-ffast-math', '-fomit-frame-pointer', '-fno-strict-aliasing', '-Wall', '-pedantic' ]
	}

archs = [
		# x86_64
		{ 'desc' : 'x86_64 generic with sse2',
		  'prefix' : 'x86_64-unknown-linux-gnu-',
		  'cflags' : [ '-m64', '-mtune=generic', '-msse2' ],
		  'objdump-flags' : [ '-M', 'intel-mnemonic' ] },

		# ARM
		{ 'desc' : 'ARMv7 with NEON (linaro)',
		  'prefix' : 'arm-linux-gnueabi-',
		  'cflags' : [ '-marm', '-march=armv7-a', '-mfpu=neon' ]},

		{ 'desc' : 'ARMv7 with VFP (linaro)',
		  'prefix' : 'arm-linux-gnueabi-',
		  'cflags' : [ '-marm', '-march=armv7-a', '-mfpu=vfp' ]},

		# PowerPC
		{ 'desc' : 'PowerPC 64-bit with AltiVec',
		  'prefix' : 'powerpc64-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=powerpc64', '-maltivec' ] },

		{ 'desc' : 'PowerPC 64-bit without AltiVec',
		  'prefix' : 'powerpc64-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=powerpc64', '-mno-altivec' ] },

		# SH4
		{ 'desc' : 'SH4',
		  'prefix' : 'sh4-unknown-linux-gnu-',
		  'cflags' : [ ] },

		# Alpha
		{ 'desc' : 'Alpha',
		  'prefix' : 'alphaev67-unknown-linux-gnu-',
		  'cflags' : [] },

		# MIPS
		{ 'desc' : 'MIPS 64-bit',
		  'prefix' : 'mipsel64-unknown-linux-gnu-',
		  'cflags' : [ '-march=mips64r2', '-mno-abicalls', '-mabi=eabi', '-mfp64', '-mips3d', '-mdmx' ] },

		{ 'desc' : 'MIPS 32-bit',
		  'prefix' : 'mipsel-unknown-linux-gnu-',
		  'cflags' : [ '-march=mips32r2', '-mno-abicalls', '-mabi=eabi', '-mfp64', '-mips3d', '-mdmx' ] },

		# SPARC
		{ 'desc' : 'UltraSPARC T2 32-bit with VIS',
		  'prefix' : 'sparc-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=niagara2', '-mvis' ] },

		{ 'desc' : 'UltraSPARC T2 32-bit without VIS',
		  'prefix' : 'sparc-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=niagara2', '-mno-vis' ] },

		{ 'desc' : 'UltraSPARC T2 64-bit with VIS',
		  'prefix' : 'sparc64-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=niagara2', '-mvis' ] },

		{ 'desc' : 'UltraSPARC T2 64-bit without VIS',
		  'prefix' : 'sparc64-unknown-linux-gnu-',
		  'cflags' : [ '-mcpu=niagara2', '-mno-vis' ] },
	]

def exec_cmd_chain(l):
	for cmd in l:
		try:
			#print ' '.join(cmd)
			p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
			stdout, stderr = p.communicate()
			if (len(stdout.strip()) > 1):
				print stdout
		except:
			pass


def main():
	target = sys.argv[1]
	for a in archs:
		print "\n%s" % ("=" * 80)
		print a['desc']
		objs = []
		cmds = []

		objbase = target + '-' + a['prefix']

		hostflags = []
		if a['prefix']:
			hostflags += [ '-ccc-host-triple', a['prefix'][:-1] ]


		cmds.append([ a['prefix'] + 'gcc', '-v' ])
		cmds.append([ a['prefix'] + 'objdump', '-v' ])
		cmds.append([ 'clang', '-v' ])

		obj = objbase + 'clang'
		clang_cc = [ 'clang' ] + hostflags + common['cflags'] + a['cflags'] + [ '-S', '-o', obj + '.s', target ]
		clang_as = [ a['prefix'] + 'as', '-o', obj + '.o', obj + '.s' ]
		cmds.append(clang_cc)
		cmds.append(clang_as)
		cmds.append(['rm', obj + '.s'])
		objs.append(obj + '.o')

		# Another option for dump_mix is to just add '-Wa,-a,-an,-ad' to these cflags.
		obj = objbase + 'gcc'
		cc = [ a['prefix'] + 'gcc' ] + common['cflags'] + a['cflags'] + [ '-c', '-o', obj + '.o', target]
		cmds.append(cc)
		objs.append(obj + '.o')

		for obj in objs:
			dump_flags = []
			if 'objdump-flags' in a:
				dump_flags = a['objdump-flags']
			dump_asm = [ a['prefix'] + 'objdump' ] + dump_flags + [ '-d', obj ]
			dump_mix = [ a['prefix'] + 'objdump' ] + dump_flags + [ '-S', obj ]
			remove_obj = [ 'rm', obj ]
			cmds.append(dump_asm)
			#cmds.append(dump_mix)
			cmds.append(remove_obj)

		try:
			print "\nCommand list:"
			for cmd in cmds:
				print '  %s' % (' '.join(cmd))
			print "=" * 80
		except:
			pass
		exec_cmd_chain(cmds)

if __name__ == "__main__":
	main()
