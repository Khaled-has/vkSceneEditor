
struct VSOutput
{
    float4 Position : SV_Position;
    [[vk::location(0)]] float3 Color : COLOR0;
};

struct PSOutput
{
    float4 Color : SV_Target0;
};

PSOutput main(VSOutput input)
{
    PSOutput output = (PSOutput)0;
    output.Color = float4(input.Color.rgb, 1.0);
    
    return output;
}