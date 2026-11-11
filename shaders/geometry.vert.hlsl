
struct VSOutput
{
    float4 Position : SV_Position;
    [[vk::location(0)]] float3 Color : COLOR0;
};

struct VertexData
{
    float2 Pos;
};

[[vk::binding(0, 0)]] RWStructuredBuffer<VertexData> Vertices : register(u0, space0);

VSOutput main(uint VertexID : SV_VertexID)
{
    VSOutput output = (VSOutput)0;
    
    output.Position = float4(Vertices[VertexID].Pos.xy, 0.0, 1.0);
    
    float3 Colors[3] =
    {
        float3(1, 0, 0), float3(0, 1, 0), float3(0, 0, 1)
    };
    output.Color = Colors[VertexID];
    
    return output;
}