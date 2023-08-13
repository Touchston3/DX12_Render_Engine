Texture2D tex: register(t0);
SamplerState samp: register(s0);

struct Vert_Out
{
    float4 Position : SV_POSITION;
    float2 Uv: UV;
};

float4 main(Vert_Out input) : SV_TARGET
{
    return tex.Sample(samp, input.Uv);
    //return float4(1.0f, 1.0f, 0.0f, 1.0f);
}
