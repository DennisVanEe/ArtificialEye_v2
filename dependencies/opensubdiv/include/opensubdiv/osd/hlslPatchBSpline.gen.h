"//\n"
"//   Copyright 2013 Pixar\n"
"//\n"
"//   Licensed under the Apache License, Version 2.0 (the \"Apache License\")\n"
"//   with the following modification; you may not use this file except in\n"
"//   compliance with the Apache License and the following modification to it:\n"
"//   Section 6. Trademarks. is deleted and replaced with:\n"
"//\n"
"//   6. Trademarks. This License does not grant permission to use the trade\n"
"//      names, trademarks, service marks, or product names of the Licensor\n"
"//      and its affiliates, except as required to comply with Section 4(c) of\n"
"//      the License and to reproduce the content of the NOTICE file.\n"
"//\n"
"//   You may obtain a copy of the Apache License at\n"
"//\n"
"//       http://www.apache.org/licenses/LICENSE-2.0\n"
"//\n"
"//   Unless required by applicable law or agreed to in writing, software\n"
"//   distributed under the Apache License with the above modification is\n"
"//   distributed on an \"AS IS\" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY\n"
"//   KIND, either express or implied. See the Apache License for the specific\n"
"//   language governing permissions and limitations under the Apache License.\n"
"//\n"
"\n"
"//----------------------------------------------------------\n"
"// Patches.VertexBSpline\n"
"//----------------------------------------------------------\n"
"\n"
"void vs_main_patches( in InputVertex input,\n"
"                      out HullVertex output )\n"
"{\n"
"    output.position = input.position;\n"
"    OSD_PATCH_CULL_COMPUTE_CLIPFLAGS(input.position);\n"
"}\n"
"\n"
"//----------------------------------------------------------\n"
"// Patches.HullBSpline\n"
"//----------------------------------------------------------\n"
"\n"
"[domain(\"quad\")]\n"
"[partitioning(OSD_PARTITIONING)]\n"
"[outputtopology(\"triangle_cw\")]\n"
"[outputcontrolpoints(16)]\n"
"[patchconstantfunc(\"HSConstFunc\")]\n"
"OsdPerPatchVertexBezier hs_main_patches(\n"
"    in InputPatch<HullVertex, 16> patch,\n"
"    uint primitiveID : SV_PrimitiveID,\n"
"    in uint ID : SV_OutputControlPointID )\n"
"{\n"
"    OsdPerPatchVertexBezier output;\n"
"\n"
"    float3 cv[16];\n"
"    for (int i=0; i<16; ++i) {\n"
"        cv[i] = patch[i].position.xyz;\n"
"    }\n"
"\n"
"    int3 patchParam = OsdGetPatchParam(OsdGetPatchIndex(primitiveID));\n"
"    OsdComputePerPatchVertexBSpline(patchParam, ID, cv, output);\n"
"\n"
"    return output;\n"
"}\n"
"\n"
"HS_CONSTANT_FUNC_OUT\n"
"HSConstFunc(\n"
"    InputPatch<HullVertex, 16> patch,\n"
"    OutputPatch<OsdPerPatchVertexBezier, 16> bezierPatch,\n"
"    uint primitiveID : SV_PrimitiveID)\n"
"{\n"
"    HS_CONSTANT_FUNC_OUT output;\n"
"\n"
"    int3 patchParam = OsdGetPatchParam(OsdGetPatchIndex(primitiveID));\n"
"\n"
"    float4 tessLevelOuter = float4(0,0,0,0);\n"
"    float2 tessLevelInner = float2(0,0);\n"
"    float4 tessOuterLo = float4(0,0,0,0);\n"
"    float4 tessOuterHi = float4(0,0,0,0);\n"
"\n"
"    OSD_PATCH_CULL(16);\n"
"\n"
"#if defined OSD_ENABLE_SCREENSPACE_TESSELLATION\n"
"#if 0\n"
"    // XXX: this doesn't work on nvidia driver 34x.\n"
"    OsdGetTessLevelsAdaptiveLimitPoints(bezierPatch, patchParam,\n"
"                     tessLevelOuter, tessLevelInner,\n"
"                     tessOuterLo, tessOuterHi);\n"
"#else\n"
"    // This is needed to coerce correct behavior on nvidia driver 34x\n"
"    OsdPerPatchVertexBezier cpBezier[16];\n"
"    for (int i=0; i<16; ++i) {\n"
"        cpBezier[i] = bezierPatch[i];\n"
"        cpBezier[i].P += 0.0f;\n"
"    }\n"
"    OsdGetTessLevelsAdaptiveLimitPoints(cpBezier, patchParam,\n"
"                     tessLevelOuter, tessLevelInner,\n"
"                     tessOuterLo, tessOuterHi);\n"
"#endif\n"
"#else\n"
"    OsdGetTessLevelsUniform(patchParam, tessLevelOuter, tessLevelInner,\n"
"                     tessOuterLo, tessOuterHi);\n"
"#endif\n"
"\n"
"    output.tessLevelOuter[0] = tessLevelOuter[0];\n"
"    output.tessLevelOuter[1] = tessLevelOuter[1];\n"
"    output.tessLevelOuter[2] = tessLevelOuter[2];\n"
"    output.tessLevelOuter[3] = tessLevelOuter[3];\n"
"\n"
"    output.tessLevelInner[0] = tessLevelInner[0];\n"
"    output.tessLevelInner[1] = tessLevelInner[1];\n"
"\n"
"    output.tessOuterLo = tessOuterLo;\n"
"    output.tessOuterHi = tessOuterHi;\n"
"\n"
"    return output;\n"
"}\n"
"\n"
"//----------------------------------------------------------\n"
"// Patches.DomainBSpline\n"
"//----------------------------------------------------------\n"
"\n"
"[domain(\"quad\")]\n"
"void ds_main_patches(\n"
"    in HS_CONSTANT_FUNC_OUT input,\n"
"    in OutputPatch<OsdPerPatchVertexBezier, 16> patch,\n"
"    in float2 domainCoord : SV_DomainLocation,\n"
"    out OutputVertex output )\n"
"{\n"
"    float3 P = float3(0,0,0), dPu = float3(0,0,0), dPv = float3(0,0,0);\n"
"    float3 N = float3(0,0,0), dNu = float3(0,0,0), dNv = float3(0,0,0);\n"
"\n"
"    OsdPerPatchVertexBezier cv[16];\n"
"    for (int i=0; i<16; ++i) {\n"
"        cv[i] = patch[i];\n"
"    }\n"
"\n"
"    float2 UV = OsdGetTessParameterization(domainCoord,\n"
"                                           input.tessOuterLo,\n"
"                                           input.tessOuterHi);\n"
"\n"
"    int3 patchParam = patch[0].patchParam;\n"
"    OsdEvalPatchBezier(patchParam, UV, cv, P, dPu, dPv, N, dNu, dNv);\n"
"\n"
"    // all code below here is client code\n"
"    output.position = mul(OsdModelViewMatrix(), float4(P, 1.0f));\n"
"    output.normal = mul(OsdModelViewMatrix(), float4(N, 0.0f)).xyz;\n"
"    output.tangent = mul(OsdModelViewMatrix(), float4(dPu, 0.0f)).xyz;\n"
"    output.bitangent = mul(OsdModelViewMatrix(), float4(dPv, 0.0f)).xyz;\n"
"#ifdef OSD_COMPUTE_NORMAL_DERIVATIVES\n"
"    output.Nu = dNu;\n"
"    output.Nv = dNv;\n"
"#endif\n"
"#ifdef OSD_PATCH_ENABLE_SINGLE_CREASE\n"
"    output.vSegments = cv[0].vSegments;\n"
"#endif\n"
"\n"
"    output.patchCoord = OsdInterpolatePatchCoord(UV, patchParam);\n"
"\n"
"    OSD_DISPLACEMENT_CALLBACK;\n"
"\n"
"    output.positionOut = mul(OsdProjectionMatrix(), output.position);\n"
"    output.edgeDistance = 0;\n"
"}\n"
"\n"
