fxc "shaders.hlsl" /Od /Zi /T vs_5_0 /E "VS_Main" /Fo "vs.cso"
fxc "shaders.hlsl" /D REFLECTION /Od /Zi /T vs_5_0 /E "VS_Main" /Fo "vs_ref.cso"
fxc "shaders.hlsl" /D SHADOW /Od /Zi /T vs_5_0 /E "VS_Main" /Fo "vs_sha.cso"

fxc "shaders.hlsl" /Od /Zi /T ps_5_0 /E "PS_Main" /Fo "ps.cso"
fxc "shaders.hlsl" /D SHADOW /Od /Zi /T ps_5_0 /E "PS_Main" /Fo "ps_sha.cso"

fxc "billboard.hlsl" /Od /Zi /T vs_5_0 /E "VS_Main" /Fo "vs_bill.cso"
fxc "billboard.hlsl" /Od /Zi /T gs_5_0 /E "GS_Main" /Fo "gs_bill.cso"
fxc "billboard.hlsl" /Od /Zi /T ps_5_0 /E "PS_Main" /Fo "ps_bill.cso"

fxc "blur.hlsl" /Od /Zi /T cs_5_0 /E "CS_Blur_H" /Fo "cs_blur_h.cso"
fxc "blur.hlsl" /Od /Zi /T cs_5_0 /E "CS_Blur_V" /Fo "cs_blur_v.cso"