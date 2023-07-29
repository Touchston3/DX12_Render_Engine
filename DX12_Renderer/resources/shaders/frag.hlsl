struct Vert_Out
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(Vert_Out input) : SV_TARGET
{
    return input.Color;
}
