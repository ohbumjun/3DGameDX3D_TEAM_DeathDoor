
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
	
	// �������� ������ ������ 0�� ��ȯ�ǰ� ����� ������ �ش� ���� �״�� ��ȯ�ȴ�.
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

	// �� ���̵�
    float3 Reflect = 2.f * ViewNormal * dot(ViewNormal, LightDir) - LightDir;
    Reflect = normalize(Reflect);

    // Gamma Space To Linear Space
	MtrlDif = pow(MtrlDif, 2.2f);

	float3 Env = g_Sky.Sample(g_LinearSmp, Reflect);
	
    // n dot l ��� Ramp Texture�� ���� ���ø��� ���� �������� ����
    // Diffuse : Linear Space���� ����
	float3 ToonDif = g_LightColor.xyz * MtrlDif * Wrapped * Attn;
	float3 MetalDif = ComputeMetalDiffuse(g_LightColor.xyz, Env, MtrlDif, Attn);

    // Metallic ����� ���� ���� ���� ����
	result.Dif = lerp(ToonDif, MetalDif, Metallic);

    // Linear Space To Gamma Space 
    // �ٽ� Gamma Space�� �ǵ���
	result.Dif = pow(result.Dif, 1 / 2.2f);

    // Metallic ��ü�� �ں��Ʈ ���� ����
	result.Amb = (g_LightColor.xyz * g_GLightAmbIntensity * MtrlAmb * Attn) * (1.f - Metallic);
	
    float SpcIntensity = max(0.f, dot(View, Reflect));
	
	// ��-�� ���̵�
    //float3 Reflect = normalize(View + LightDir);
    //float SpcIntensity = max(0.f, dot(ViewNormal, Reflect));
	
	float PowSpcAttn = pow(SpcIntensity, SpcPower) * Attn;

    // Metallic Specular�� ������ ���� ���󰣴�.
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
	
	// �������� ������ ������ 0�� ��ȯ�ǰ� ����� ������ �ش� ���� �״�� ��ȯ�ȴ�.
    float Intensity = max(0.f, dot(ViewNormal, LightDir));
    
    float3 MtrlDif = ConvertColor(MaterialColor.r).rgb;
    float3 MtrlAmb = ConvertColor(MaterialColor.g).rgb;
    float3 MtrlSpc = ConvertColor(MaterialColor.b).rgb;
    float3 MtrlEmv = ConvertColor(MaterialColor.a).rgb;

    float3 View = -Pos;
    View = normalize(View);
	
	// �� ���̵�
    float3 Reflect = 2.f * ViewNormal * dot(ViewNormal, LightDir) - LightDir;
    Reflect = normalize(Reflect);

	float3 Env = g_Sky.Sample(g_LinearSmp, Reflect);

    // Gamma Space To Linear Space
	MtrlDif = pow(MtrlDif, 2.2f);
	
    // Metallic ����� ����
	float3 StandardDif = g_LightColor.xyz * MtrlDif * Intensity * Attn; 
	float3 MetalDif = ComputeMetalDiffuse(g_LightColor.xyz, Env, MtrlDif, Attn);

	result.Dif = lerp(StandardDif, MetalDif, Metallic);

    // Linear Space To Gamma Space
	result.Dif = pow(result.Dif, 1 / 2.2f);

    // Metallic ��ü�� �ں��Ʈ ���� ����
	result.Amb = (g_LightColor.xyz * g_GLightAmbIntensity * MtrlAmb * Attn) * (1.f - Metallic);
	
    float SpcIntensity = max(0.f, dot(View, Reflect));
	
	// ��-�� ���̵�
    //float3 Reflect = normalize(View + LightDir);
    //float SpcIntensity = max(0.f, dot(ViewNormal, Reflect));
	
    // Specular ����
	float PowSpcAttn = pow(SpcIntensity, SpcPower) * Attn;

    float3 StandardSpc = g_LightColor.xyz * MtrlSpc * PowSpcAttn;
	float3 MetalSpc = ComputeMetalSpc(g_LightColor.xyz, MtrlDif, PowSpcAttn);

	result.Spc = lerp(StandardSpc, MetalSpc, Metallic);
	
    result.Emv = MtrlEmv;
	
    return result;
}
