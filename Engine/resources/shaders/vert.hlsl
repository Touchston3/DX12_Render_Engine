
struct Vert_Out
{
    float4 Position : SV_POSITION;
    float2 Uv: UV;
};

cbuffer WorldView : register(b0)
{
    float4x4 rotation;
};
Vert_Out main(float3 pos : POSITION, float2 uv: UV)  
{
    Vert_Out vert_out;
    vert_out.Position = mul(rotation, float4(pos, 1.0f));
    vert_out.Uv = uv;
    return vert_out;
}