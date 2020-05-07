set app="appdemo"

set cp_src="Z:/hdd1/imx6/extk-1.0/out"
set cp_dst="/home/root/demo/"
set cp_cmd=uuu
@rem set cp_cmd=uuu -v

%cp_cmd% FBK: ucp %cp_src%/%app% t:/tmp
%cp_cmd% FBK: ucmd chmod +x /tmp/%app%
%cp_cmd% FBK: ucmd cp /tmp/%app% %cp_dst%

@rem pause
