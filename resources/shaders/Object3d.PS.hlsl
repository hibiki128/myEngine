#include"object3d.hlsli"

struct Material
{
    float4 color;
    int enableLighting;
    float4x4 uvTransform;
    float shininess;
};

struct DirectionalLight
{
    float4 color; //<! ライトの色
    float3 direction; //!< ライトの向き
    float intensity; //!< 輝度
    int active;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Camera
{
    float3 worldPosition;
};

// ポイントライトの構造体
struct PointLight
{
    float4 color; //<! ライトの色
    float3 position; //<! ライトの位置
    float intensity; //<! 輝度
    int active;
    float radius;
    float decay;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);
ConstantBuffer<Camera> gCamera : register(b2);
ConstantBuffer<PointLight> gPointLight : register(b3); //<! ポイントライト定数バッファ


PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    PixelShaderOutput output;
    if (gMaterial.enableLighting != 0)
    {
    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
        if (gDirectionalLight.active != 0)
        {
          // 指向性ライトの計算
            float NdotLDirectional = dot(normalize(input.normal), normalize(-gDirectionalLight.direction));
            float cosDirectional = pow(NdotLDirectional * 0.5f + 0.5f, 2.0f);
            float3 toEyeDirectional = normalize(gCamera.worldPosition - input.worldPosition);
            float3 halfVectorDirectional = normalize(-gDirectionalLight.direction + toEyeDirectional);
            float NDotHDirectional = dot(normalize(input.normal), halfVectorDirectional);
            float specularPowDirectional = pow(saturate(NDotHDirectional), gMaterial.shininess);

            // 拡散反射
            float3 diffuseDirectional = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cosDirectional * gDirectionalLight.intensity;
            // 鏡面反射
            float3 specularDirectional = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPowDirectional * float3(1.0f, 1.0f, 1.0f);
            // 拡散反射 + 鏡面反射
            output.color.rgb += diffuseDirectional + specularDirectional;
        }
        if (gPointLight.active != 0)
        {
              // ポイントライトの計算
            float3 lightDir = gPointLight.position - input.worldPosition; // ライトからの方向
            float distance = length(lightDir); // ライトまでの距離
            lightDir = normalize(lightDir); // 正規化

            // 拡散反射
            float NdotLPoint = dot(normalize(input.normal), lightDir);
            float cosPoint = max(NdotLPoint, 0.0f); // コサイン値
            float3 diffusePoint = gMaterial.color.rgb * textureColor.rgb * gPointLight.color.rgb * cosPoint * gPointLight.intensity;

            // 鏡面反射
            float3 toEyePoint = normalize(gCamera.worldPosition - input.worldPosition);
            float3 halfVectorPoint = normalize(lightDir + toEyePoint);
            float NDotHPoint = dot(normalize(input.normal), halfVectorPoint);
            float specularPowPoint = pow(saturate(NDotHPoint), gMaterial.shininess);
            float3 specularPoint = gPointLight.color.rgb * gPointLight.intensity * specularPowPoint * float3(1.0f, 1.0f, 1.0f);

            float factor = pow(saturate(-distance / gPointLight.radius + 1.0f), gPointLight.decay);
            
            // 拡散反射 + 鏡面反射
            output.color.rgb += (diffusePoint + specularPoint) * (gPointLight.color.rgb * gPointLight.intensity * factor);
        }
        output.color.a = gMaterial.color.a * textureColor.a;
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    if (textureColor.a == 0.0f)
    {
        discard;
    }
    if (output.color.a == 0.0f)
    {
        discard;
    }
    return output;
}
