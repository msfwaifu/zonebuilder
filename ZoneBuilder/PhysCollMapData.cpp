#include "StdInc.h"
#include "Tool.h"

void writecbrushside_t(ZStream* buf, cBrushSide* data, int num)
{
	WRITE_ASSET_NUM(data, cBrushSide, num);
	
	for(int i=0; i<num; i++)
	{	
		if(HAS_FIELD((&data[i]), side)) // OffsetToPoiner
		{
			buf->align(ALIGN_TO_4); 
			buf->write(data[i].side, sizeof(cPlane), 1);
			dest[i].side = (cPlane*)-1;
		}
	}
}

void writeBrush(ZStream* buf, BrushWrapper * data, BrushWrapper* dest)
{
	writecbrushside_t(buf, data->brush.brushSide, data->brush.count);
	data->brush.brushSide = (cBrushSide*)-1;

	WRITE_FIELD(data, brush.brushEdge, char, totalEdgeCount);
}

void writeBrushWrapper(ZStream* buf, BrushWrapper* data)
{
	WRITE_ASSET(data, BrushWrapper);

	writeBrush(buf, data, dest);

	WRITE_FIELD_ALIGNED(data, planes, cPlane, brush.count, ALIGN_TO_4); // OffsetToPoiner
}

void writePhysGeomInfo(ZStream* buf, PhysGeomInfo* data, int num)
{
	WRITE_ASSET_NUM(data, PhysGeomInfo, num);

	for(int i=0; i<num; i++)
	{
		if(dest[i].brush)
		{
			buf->align(ALIGN_TO_4);
			writeBrushWrapper(buf, dest[i].brush);
		}
	}
}

void writePhysCollmap(zoneInfo_t* info, ZStream* buf, PhysGeomList* data)
{
	WRITE_ASSET(data, PhysGeomList);
	buf->pushStream(ZSTREAM_VIRTUAL);

	WRITE_NAME(data);

	if (data->geoms)
	{
		buf->align(ALIGN_TO_4);
		writePhysGeomInfo(buf, data->geoms, data->count);
	}

	buf->popStream();
}

void * addPhysCollmap(zoneInfo_t* info, const char* name, char* data, int dataLen)
{
	if (dataLen < 0) return data;
	Com_Error(false, "Can't add new PhysCollmap assets!");
	return NULL;
}