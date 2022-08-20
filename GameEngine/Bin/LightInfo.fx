
void ComputeLightDirAndAttn(out float3 outLightDir, out float outAttn, float3 Pos)
{
    if (g_LightType == LightTypeDir)
    {
		outLightDir = -g_LightDir;
		outLightDir = normalize(outLightDir);
		outAttn = 1.f;
	}
	
    else if (g_LightType == LightTypePoint)
    {
		outLightDir = g_LightPos - Pos;
		outLightDir = normalize(outLightDir);
		
        float Dist = distance(g_LightPos, Pos);
		
        if (Dist > g_LightDistance)
			outAttn = 0.f;
        else
            outAttn = 1.f / (g_LightAtt1 + g_LightAtt2 * Dist + g_LightAtt3 * (Dist * Dist));
    }
}

float3 ComputeMetalDiffuse(float3 LightColor, float3 Env, float3 MtrlDiffuse, float Attn)
{
	return LightColor * MtrlDiffuse * Env * Attn;
}

float3 ComputeMetalSpc(float3 LightColor, float3 MtrlDiffuse, float PowSpcMulAttn)
{
	return LightColor * MtrlDiffuse * PowSpcMulAttn;
}

LightResult ComputeLightToon(float3 Pos, float3 Normal, float4 MaterialColor,float SpcPower, float Metallic, float ToonType)
{
    LightResult result = (LightResult) 0;
	
    float3 LightDir = (float3) 0.f;
    float Attn = 1.f;

	ComputeLightDirAndAttn(LightDir, Attn, Pos);
	
    float3 ViewNormal = Normal;
	
	// 내적값이 음수가 나오면 0이 반환되고 양수가 나오면 해당 값이 그대로 반환된다.
    float Intensity = max(0.f, dot(ViewNormal, LightDir));

    // Half Rambert
	Intensity = (Intensity * 0.5f) + 0.5f;

    // Wraped Diffuse
	int TexIndex = (int)ceil(ToonType);
	float3 Wrapped = g_RampTexArray.Sample(g_BaseSmp, float3(Intensity, 0.f, ToonType)).rgb;
    
    float3 MtrlDif = ConvertColor(MaterialColor.r).rgb;
    float3 MtrlAmb = ConvertColor(MaterialColor.g).rgb;
    float3 MtrlSpc = ConvertColor(MaterialColor.b).rgb;
    float3 MtrlEmv = ConvertColor(MaterialColor.a).rgb;
	
    float3 View = -Pos;
    View = normalize(View);

	// 퐁 쉐이딩
    float3 Reflect = 2.f * ViewNormal * dot(ViewNormal, LightDir) - LightDir;
    Reflect = normalize(Reflect);

    // Gamma Space To Linear Space
	MtrlDif = pow(MtrlDif, 2.2f);

	float3 Env = g_Sky.Sample(g_LinearSmp, Reflect);
	
    // n dot l 대신 Ramp Texture의 값을 샘플링한 값을 색상으로 취함
    // Diffuse : Linear Space에서 연산
	float3 ToonDif = g_LightColor.xyz * MtrlDif * Wrapped * Attn;
	float3 MetalDif = ComputeMetalDiffuse(g_LightColor.xyz, Env, MtrlDif, Attn);

    // Metallic 색상과 원래 색상 사이 보간
	result.Dif = lerp(ToonDif, MetalDif, Metallic);

    // Linear Space To Gamma Space 
    // 다시 Gamma Space로 되돌림
	result.Dif = pow(result.Dif, 1 / 2.2f);

    // Metallic 물체는 앰비언트 주지 않음
	result.Amb = (g_LightColor.xyz * g_GLightAmbIntensity * MtrlAmb * Attn) * (1.f - Metallic);
	
    float SpcIntensity = max(0.f, dot(View, Reflect));
	
	// 블린-퐁 쉐이딩
    //float3 Reflect = normalize(View + LightDir);
    //float SpcIntensity = max(0.f, dot(ViewNormal, Reflect));
	
	float PowSpcAttn = pow(SpcIntensity, SpcPower) * Attn;

    // Metallic Specular는 재질의 색을 따라간다.
    float3 ToonSpc = g_LightColor.xyz * MtrlSpc * PowSpcAttn; 
	float3 MetalSpc = ComputeMetalSpc(g_LightColor.xyz, MtrlDif, PowSpcAttn);

	result.Spc = lerp(ToonSpc, MetalSpc, Metallic);
	
    result.Emv = MtrlEmv;
	
    return result;
}

LightResult ComputeLight(float3 Pos, float3 Normal, float4 MaterialColor, float SpcPower, float Metallic)
{
    LightResult result = (LightResult) 0;
	
    float3 LightDir = (float3) 0.f;
    float Attn = 1.f;

	ComputeLightDirAndAttn(LightDir, Attn, Pos);
	
    float3 ViewNormal = Normal;
	
	// 내적값이 음수가 나오면 0이 반환되고 양수가 나오면 해당 값이 그대로 반환된다.
    float Intensity = max(0.f, dot(ViewNormal, LightDir));
    
    float3 MtrlDif = ConvertColor(MaterialColor.r).rgb;
    float3 MtrlAmb = ConvertColor(MaterialColor.g).rgb;
    float3 MtrlSpc = ConvertColor(MaterialColor.b).rgb;
    float3 MtrlEmv = ConvertColor(MaterialColor.a).rgb;

    float3 View = -Pos;
    View = normalize(View);
	
	// 퐁 쉐이딩
    float3 Reflect = 2.f * ViewNormal * dot(ViewNormal, LightDir) - LightDir;
    Reflect = normalize(Reflect);

	float3 Env = g_Sky.Sample(g_LinearSmp, Reflect);

    // Gamma Space To Linear Space
	MtrlDif = pow(MtrlDif, 2.2f);
	
    // Metallic 색상과 보간
	float3 StandardDif = g_LightColor.xyz * MtrlDif * Intensity * Attn; 
	float3 MetalDif = ComputeMetalDiffuse(g_LightColor.xyz, Env, MtrlDif, Attn);

	result.Dif = lerp(StandardDif, MetalDif, Metallic);

    // Linear Space To Gamma Space
	result.Dif = pow(result.Dif, 1 / 2.2f);

    // Metallic 물체는 앰비언트 주지 않음
	result.Amb = (g_LightColor.xyz * g_GLightAmbIntensity * MtrlAmb * Attn) * (1.f - Metallic);
	
    float SpcIntensity = max(0.f, dot(View, Reflect));
	
	// 블린-퐁 쉐이딩
    //float3 Reflect = normalize(View + LightDir);
    //float SpcIntensity = max(0.f, dot(ViewNormal, Reflect));
	
    // Specular 보간
	float PowSpcAttn = pow(SpcIntensity, SpcPower) * Attn;

    float3 StandardSpc = g_LightColor.xyz * MtrlSpc * PowSpcAttn;
	float3 MetalSpc = ComputeMetalSpc(g_LightColor.xyz, MtrlDif, PowSpcAttn);

	result.Spc = lerp(StandardSpc, MetalSpc, Metallic);
	
    result.Emv = MtrlEmv;
	
    return result;
}
