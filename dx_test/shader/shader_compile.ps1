fxc "shaders.hlsl" /Od /Zi /T vs_5_0 /E "VS_Main" /Fo "vs.cso"
fxc "shaders.hlsl" /D REFLECTION /Od /Zi /T vs_5_0 /E "VS_Main" /Fo "vs_ref.cso"
fxc "shaders.hlsl" /D SHADOW /Od /Zi /T vs_5_0 /E "VS_Main" /Fo "vs_sha.cso"

fxc "shaders.hlsl" /Od /Zi /T ps_5_0 /E "PS_Main" /Fo "ps.cso"
fxc "shaders.hlsl" /D SHADOW /Od /Zi /T ps_5_0 /E "PS_Main" /Fo "ps_sha.cso"
