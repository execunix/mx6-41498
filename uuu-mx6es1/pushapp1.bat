set tgz="demo.tar.gz"

set cp_src="Z:/hdd1/imx6"
set cp_dst="/home/root/demo/"
set cp_cmd=uuu
@rem set cp_cmd=uuu -v

%cp_cmd% FBK: ucmd mkdir -p %cp_dst%
%cp_cmd% FBK: acmd EXTRACT_UNSAFE_SYMLINKS=1 tar -xvpz -C %cp_dst%
%cp_cmd% FBK: ucp %cp_src%/%tgz% t:-

@rem %cp_cmd% FBK: acmd EXTRACT_UNSAFE_SYMLINKS=1 tar -jxv -C %cp_dst%
@rem %cp_cmd% FBK: ucp %cp_src%/rootfs.tar.bz2 t:-

@rem %cp_cmd% FBK: ucp %cp_src%/init/init.cpp t:/tmp
@rem %cp_cmd% FBK: ucmd cp /tmp/init.cpp %cp_dst%/init.cpp

@rem pause
