#include "../dataStructs.h"
#include "../../objects/shapes.h"
#include "../linked_list.h"
#include "../Quad_trees.h"

void InitEnginePools(void)
{
    initShapesPool();
    initLinkedListPool();
    //initQuadtreePool();
}
