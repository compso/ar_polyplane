#include <ai.h>
#include <vector>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator

// Procedural parameters

const char* enum_subdiv[] =
{
   "none",
   "catclark",
   "linear",
   NULL
};

const char* enum_adaptive[] =
{
   "auto",
   "edge_length",
   "flatness",
   NULL
};

const char* enum_adaptive_space[] =
{
   "raster",
   "object",
   NULL
};

struct PolyPlane
{
    float   width;
    int divisions;

    int subdiv_type;
    int subdiv_iterations;
    float subdiv_adaptive_error;
    int subdiv_adaptive_metric;
    int subdiv_adaptive_space;

    AtNode * disp_map;
    float  disp_height;
    float  disp_zero_value;
    float  disp_padding;
    bool  disp_autobump;

    int num_vertices;
    AtNode * node;
};

AI_PROCEDURAL_NODE_EXPORT_METHODS(PolyPlaneMtd);

node_parameters
{
    AiParameterFlt("width", 10);
    AiParameterInt("divisions", 2);

    AiParameterEnum("subdiv_type", 0, enum_subdiv);
    AiParameterByte("subdiv_iterations", 1);
    AiParameterFlt("subdiv_adaptive_error", 0);
    AiParameterEnum("subdiv_adaptive_metric", 0, enum_adaptive);
    AiParameterEnum("subdiv_adaptive_space", 0, enum_adaptive_space);

    AiParameterNode("disp_map", NULL);
    AiParameterFlt("disp_zero_value", 0);
    AiParameterFlt("disp_height", 1);
    AiParameterFlt("disp_padding", 0);
    AiParameterBool("disp_autobump", false);
}

procedural_init
{
    PolyPlane *plane = new PolyPlane();
    *user_ptr = plane;

    plane->width                    = AiNodeGetFlt(node, "width");
    plane->divisions                = AiNodeGetInt(node, "divisions");

    plane->subdiv_type              = AiNodeGetInt(node, "subdiv_type");
    plane->subdiv_iterations        = AiNodeGetByte(node, "subdiv_iterations");
    plane->subdiv_adaptive_error    = AiNodeGetFlt(node, "subdiv_adaptive_error");
    plane->subdiv_adaptive_metric   = AiNodeGetFlt(node, "subdiv_adaptive_metric");
    plane->subdiv_adaptive_space    = AiNodeGetFlt(node, "subdiv_adaptive_space");

    plane->disp_map                 = static_cast<AtNode*>(AiNodeGetPtr(node, "disp_map"));
    plane->disp_height              = AiNodeGetFlt(node, "disp_height");
    plane->disp_zero_value          = AiNodeGetFlt(node, "disp_zero_value");
    plane->disp_padding             = AiNodeGetFlt(node, "disp_padding");
    plane->disp_autobump             = AiNodeGetBool(node, "disp_autobump");

    plane->num_vertices             = pow(plane->divisions+1, 2);


    std::vector<unsigned char> nsides;
    std::vector<float> vlist;
    std::vector<uint32_t> vidxs;

    AtNode* mesh_node = AiNode("polymesh", "plane_shape", node);

    if (!mesh_node)
    {
        AiMsgError("[bla] failed to make polymesh node for %s",AiNodeGetName(node));
        return false;
    }

    plane->node = mesh_node;

    // nsides
    nsides.reserve(pow(plane->divisions, 2));
    for (size_t i = 0; i < pow(plane->divisions, 2); ++i)
    {
        nsides.push_back(4);
    }

    AiNodeSetArray(mesh_node,"nsides",AiArrayConvert(nsides.size(), 1, AI_TYPE_BYTE, nsides.data()));

    // vidxs
    int a = 0;
    int v = 0;
    for (int d = 0; d < plane->divisions; ++d)
    {
        for (int c = 0; c < plane->divisions; ++c)
        {
            v = a+c;
            vidxs.push_back(v);
            v++;
            vidxs.push_back(v);
            vidxs.push_back(v+plane->divisions+1);
            vidxs.push_back(v+plane->divisions);
        }
        a = v+1;
    }

    AiNodeSetArray(mesh_node,"vidxs",AiArrayConvert(vidxs.size(), 1, AI_TYPE_UINT, vidxs.data()));

    // vlist
    for (int d = 0; d <= plane->divisions; ++d)
    {
        float posz = (plane->width-(d*(plane->width/plane->divisions)))-(plane->width/2);
        for (int c = 0; c <= plane->divisions; ++c)
        {
            float posx = (plane->width/(float)plane->divisions)*c-(plane->width/2.0);
            vlist.push_back(posx);
            vlist.push_back(0.0);
            vlist.push_back(posz);
        }
    }

    AiNodeSetArray(mesh_node,"vlist",AiArrayConvert(vlist.size(), 1, AI_TYPE_FLOAT, vlist.data()));

    // subdiv
    AiNodeSetBool(mesh_node, "smoothing", true);
    AiNodeSetInt(mesh_node, "subdiv_type", plane->subdiv_type);
    AiNodeSetByte(mesh_node, "subdiv_iterations", plane->subdiv_iterations);
    AiNodeSetFlt(mesh_node, "subdiv_adaptive_error", plane->subdiv_adaptive_error);
    AiNodeSetInt(mesh_node, "subdiv_adaptive_metric", plane->subdiv_adaptive_metric);
    AiNodeSetInt(mesh_node, "subdiv_adaptive_space", plane->subdiv_adaptive_space);

    // displacement
    if (plane->disp_map != nullptr)
    {
        AiNodeSetPtr(mesh_node,"disp_map",plane->disp_map);
        AiNodeSetFlt(mesh_node,"disp_height",plane->disp_height);
        AiNodeSetFlt(mesh_node,"disp_zero_value",plane->disp_zero_value);
        AiNodeSetFlt(mesh_node,"disp_padding",plane->disp_padding);
        AiNodeSetBool(mesh_node,"disp_autobump",plane->disp_autobump);
    }

    return true;
}

procedural_cleanup
{
    delete reinterpret_cast<PolyPlane*>(user_ptr);
    return 1;
}

procedural_num_nodes
{
    return 1;
}

procedural_get_node
{
    PolyPlane * plane = reinterpret_cast<PolyPlane*>(user_ptr);
    return plane->node ? plane->node : 0;;
}

node_loader
{
    if (i>0)
        return false;

    node->methods      = PolyPlaneMtd;
    node->output_type  = AI_TYPE_NONE;
    node->name         = "ar_polyplane";
    node->node_type    = AI_NODE_SHAPE_PROCEDURAL;
    strcpy(node->version, AI_VERSION);

    return true;
}
