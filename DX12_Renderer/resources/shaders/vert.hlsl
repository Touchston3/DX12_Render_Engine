
struct Vert_Out
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

Vert_Out main(float3 pos : POSITION, float3 color : COLOR)  
{
    Vert_Out vert_out;
    vert_out.Color = float4(color, 1.0f);
    vert_out.Position = float4(pos, 1.0f);
    return vert_out;
}