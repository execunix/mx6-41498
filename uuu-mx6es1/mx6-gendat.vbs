Set fso = CreateObject("Scripting.FileSystemObject")

Dim dtbsize
Dim zimagesize
Dim bootlogosize

Set binfile = fso.GetFile("files\imx6q-es1.dtb")
dtbsize = binfile.Size
Set binfile = Nothing

Set binfile = fso.GetFile("files\zImage")
zimagesize = binfile.Size
Set binfile = Nothing

Set binfile = fso.GetFile("files\bootlogo.png")
bootlogosize = binfile.Size
Set binfile = Nothing

bootcfg = "<BOOTCFG avbmeta=a:7:0:0/>"

Set txtfile = fso.CreateTextFile("files\bootcfg.txt",true)
txtfile.Write bootcfg
txtfile.Write Chr(0)
txtfile.Close
Set txtfile = Nothing

bootenv = "<BOOTENV dtbsize={0} zimagesize={1} bootlogosize={2}/>"
bootenv = Replace(bootenv, "{0}", dtbsize)
bootenv = Replace(bootenv, "{1}", zimagesize)
bootenv = Replace(bootenv, "{2}", bootlogosize)

Set txtfile = fso.CreateTextFile("files\bootenv.txt",true)
txtfile.Write bootenv
txtfile.Write Chr(0)
txtfile.Close
Set txtfile = Nothing

Set fso = Nothing
