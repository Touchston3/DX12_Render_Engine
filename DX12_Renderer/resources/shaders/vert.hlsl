
struct Vert_Out
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

cbuffer WorldView : register(b0)
{
    float4x4 rotation;
};
Vert_Out main(float3 pos : POSITION, float3 color : COLOR)  
{
    Vert_Out vert_out;
    vert_out.Color = float4(color, 1.0f);
    vert_out.Position = mul(rotation, float4(pos, 1.0f));
    return vert_out;
}