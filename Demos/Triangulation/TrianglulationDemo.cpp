#include "TrianglulationDemo.h"
#include "global.h"
#include "mesh_buffer.h"
#include <cstdlib>
#include <GLFW/glfw3.h>
#include "triangle.h"

const float defalut_zValue = 10;

void report(
	struct triangulateio* io,
int markers,
int reporttriangles,
int reportneighbors,
int reportsegments,
int reportedges,
int reportnorms )
{
	int i, j;

	for (i = 0; i < io->numberofpoints; i++) {
		printf("Point %4d:", i);
		for (j = 0; j < 2; j++) {
			printf("  %.6g", io->pointlist[i * 2 + j]);
		}
		if (io->numberofpointattributes > 0) {
			printf("   attributes");
		}
		for (j = 0; j < io->numberofpointattributes; j++) {
			printf("  %.6g",
				io->pointattributelist[i * io->numberofpointattributes + j]);
		}
		if (markers) {
			printf("   marker %d\n", io->pointmarkerlist[i]);
		}
		else {
			printf("\n");
		}
	}
	printf("\n");

	if (reporttriangles || reportneighbors) {
		for (i = 0; i < io->numberoftriangles; i++) {
			if (reporttriangles) {
				printf("Triangle %4d points:", i);
				for (j = 0; j < io->numberofcorners; j++) {
					printf("  %4d", io->trianglelist[i * io->numberofcorners + j]);
				}
				if (io->numberoftriangleattributes > 0) {
					printf("   attributes");
				}
				for (j = 0; j < io->numberoftriangleattributes; j++) {
					printf("  %.6g", io->triangleattributelist[i *
						io->numberoftriangleattributes + j]);
				}
				printf("\n");
			}
			if (reportneighbors) {
				printf("Triangle %4d neighbors:", i);
				for (j = 0; j < 3; j++) {
					printf("  %4d", io->neighborlist[i * 3 + j]);
				}
				printf("\n");
			}
		}
		printf("\n");
	}

	if (reportsegments) {
		for (i = 0; i < io->numberofsegments; i++) {
			printf("Segment %4d points:", i);
			for (j = 0; j < 2; j++) {
				printf("  %4d", io->segmentlist[i * 2 + j]);
			}
			if (markers) {
				printf("   marker %d\n", io->segmentmarkerlist[i]);
			}
			else {
				printf("\n");
			}
		}
		printf("\n");
	}

	if (reportedges) {
		for (i = 0; i < io->numberofedges; i++) {
			printf("Edge %4d points:", i);
			for (j = 0; j < 2; j++) {
				printf("  %4d", io->edgelist[i * 2 + j]);
			}
			if (reportnorms && (io->edgelist[i * 2 + 1] == -1)) {
				for (j = 0; j < 2; j++) {
					printf("  %.6g", io->normlist[i * 2 + j]);
				}
			}
			if (markers) {
				printf("   marker %d\n", io->edgemarkerlist[i]);
			}
			else {
				printf("\n");
			}
		}
		printf("\n");
	}
}

void TrianglulationDemo::quicktest()
{

		struct triangulateio in, mid, out, vorout;

		/* Define input points. */

		in.numberofpoints = 4;
		in.numberofpointattributes = 1;
		in.pointlist = (REAL*)malloc(in.numberofpoints * 2 * sizeof(REAL));
		in.pointlist[0] = 0.0;
		in.pointlist[1] = 0.0;
		in.pointlist[2] = 1.0;
		in.pointlist[3] = 0.0;
		in.pointlist[4] = 1.0;
		in.pointlist[5] = 10.0;
		in.pointlist[6] = 0.0;
		in.pointlist[7] = 10.0;
		in.pointattributelist = (REAL*)malloc(in.numberofpoints *
			in.numberofpointattributes *
			sizeof(REAL));
		in.pointattributelist[0] = 0.0;
		in.pointattributelist[1] = 1.0;
		in.pointattributelist[2] = 11.0;
		in.pointattributelist[3] = 10.0;
		in.pointmarkerlist = (int*)malloc(in.numberofpoints * sizeof(int));
		in.pointmarkerlist[0] = 0;
		in.pointmarkerlist[1] = 2;
		in.pointmarkerlist[2] = 0;
		in.pointmarkerlist[3] = 0;

		in.numberofsegments = 0;
		in.numberofholes = 0;
		in.numberofregions = 1;
		in.regionlist = (REAL*)malloc(in.numberofregions * 4 * sizeof(REAL));
		in.regionlist[0] = 0.5;
		in.regionlist[1] = 5.0;
		in.regionlist[2] = 7.0;            /* Regional attribute (for whole mesh). */
		in.regionlist[3] = 0.1;          /* Area constraint that will not be used. */

		printf("Input point set:\n\n");
		report(&in, 1, 0, 0, 0, 0, 0);

		/* Make necessary initializations so that Triangle can return a */
		/*   triangulation in `mid' and a voronoi diagram in `vorout'.  */

		mid.pointlist = (REAL*)NULL;            /* Not needed if -N switch used. */
		/* Not needed if -N switch used or number of point attributes is zero: */
		mid.pointattributelist = (REAL*)NULL;
		mid.pointmarkerlist = (int*)NULL; /* Not needed if -N or -B switch used. */
		mid.trianglelist = (int*)NULL;          /* Not needed if -E switch used. */
		/* Not needed if -E switch used or number of triangle attributes is zero: */
		mid.triangleattributelist = (REAL*)NULL;
		mid.neighborlist = (int*)NULL;         /* Needed only if -n switch used. */
		/* Needed only if segments are output (-p or -c) and -P not used: */
		mid.segmentlist = (int*)NULL;
		/* Needed only if segments are output (-p or -c) and -P and -B not used: */
		mid.segmentmarkerlist = (int*)NULL;
		mid.edgelist = (int*)NULL;             /* Needed only if -e switch used. */
		mid.edgemarkerlist = (int*)NULL;   /* Needed if -e used and -B not used. */

		vorout.pointlist = (REAL*)NULL;        /* Needed only if -v switch used. */
		/* Needed only if -v switch used and number of attributes is not zero: */
		vorout.pointattributelist = (REAL*)NULL;
		vorout.edgelist = (int*)NULL;          /* Needed only if -v switch used. */
		vorout.normlist = (REAL*)NULL;         /* Needed only if -v switch used. */

		/* Triangulate the points.  Switches are chosen to read and write a  */
		/*   PSLG (p), preserve the convex hull (c), number everything from  */
		/*   zero (z), assign a regional attribute to each element (A), and  */
		/*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
		/*   neighbor list (n).                                              */

		triangulate("pczAevn", &in, &mid, &vorout);

		printf("Initial triangulation:\n\n");
		report(&mid, 1, 1, 1, 1, 1, 0);
		printf("Initial Voronoi diagram:\n\n");
		report(&vorout, 0, 0, 0, 0, 1, 1);

		/* Attach area constraints to the triangles in preparation for */
		/*   refining the triangulation.                               */

		/* Needed only if -r and -a switches used: */
		mid.trianglearealist = (REAL*)malloc(mid.numberoftriangles * sizeof(REAL));
		mid.trianglearealist[0] = 3.0;
		mid.trianglearealist[1] = 1.0;

		/* Make necessary initializations so that Triangle can return a */
		/*   triangulation in `out'.                                    */

		out.pointlist = (REAL*)NULL;            /* Not needed if -N switch used. */
		/* Not needed if -N switch used or number of attributes is zero: */
		out.pointattributelist = (REAL*)NULL;
		out.trianglelist = (int*)NULL;          /* Not needed if -E switch used. */
		/* Not needed if -E switch used or number of triangle attributes is zero: */
		out.triangleattributelist = (REAL*)NULL;

		/* Refine the triangulation according to the attached */
		/*   triangle area constraints.                       */

		triangulate("prazBP", &mid, &out, (struct triangulateio*) NULL);

		printf("Refined triangulation:\n\n");
		report(&out, 0, 1, 0, 0, 0, 0);

		/* Free all allocated arrays, including those allocated by Triangle. */

		free(in.pointlist);
		free(in.pointattributelist);
		free(in.pointmarkerlist);
		free(in.regionlist);
		free(mid.pointlist);
		free(mid.pointattributelist);
		free(mid.pointmarkerlist);
		free(mid.trianglelist);
		free(mid.triangleattributelist);
		free(mid.trianglearealist);
		free(mid.neighborlist);
		free(mid.segmentlist);
		free(mid.segmentmarkerlist);
		free(mid.edgelist);
		free(mid.edgemarkerlist);
		free(vorout.pointlist);
		free(vorout.pointattributelist);
		free(vorout.edgelist);
		free(vorout.normlist);
		free(out.pointlist);
		free(out.pointattributelist);
		free(out.trianglelist);
		free(out.triangleattributelist);

		return ;
}


std::string TrianglulationDemo::fillInputData1(triangulateio& input_data)
{
	input_data = triangulateio{ 0 };
	Vector<Vec2> vertices;

	vertices.push_back(Vec2(0, 0));
	vertices.push_back(Vec2(1, 0));
	vertices.push_back(Vec2(1, 1));
	vertices.push_back(Vec2(0, 1));
	vertices.push_back(Vec2(0.5, 0.5));

	input_data.numberofpoints = (int)vertices.size();
	input_data.pointlist = new float[vertices.size() * 2];
	memcpy_s(input_data.pointlist, vertices.size() * 2 * sizeof(float), vertices.data(), vertices.size() * sizeof(Vec2));

	//input_data.numberofpointattributes = 1;
	//input_data.pointattributelist = new float[vertices.size() * input_data.numberofpointattributes]{0};
	//input_data.pointmarkerlist = new int[vertices.size()]{ 0 };
	
	printf(":dd");
	return "pczAe";
}

void TrianglulationDemo::createMeshes(const triangulateio& data, std::set< IRenderable*>& meshGroup)
{
	Vector<Vec3> vertices;

	Vector<unsigned> pointIndices;
	vertices.reserve(data.numberofpoints);
	pointIndices.reserve(data.numberofpoints);

	float* pdata = data.pointlist;
	for (size_t i = 0; i < data.numberofpoints; ++i)
	{
		Vec3 vertex;
		vertex.x = *pdata;
		++pdata;
		vertex.y = *pdata;
		++pdata;
		vertex.z = defalut_zValue;

		vertices.push_back(vertex);
		pointIndices.push_back(i);
	}
	m_box.merge(vertices);
	

	Vector<unsigned> edgeIndices;
	if (data.numberofedges > 0)
	{
		edgeIndices.reserve(data.numberofedges);

		int* pData = data.edgelist;
		for (size_t i = 0; i < data.numberofedges; ++i)
		{
			edgeIndices.push_back((unsigned)*pData);
			++pData;
			edgeIndices.push_back((unsigned)*pData);
			++pData;
		}
	}
	
	Vector<unsigned> triangleIndices;
	if (data.numberoftriangles > 0)
	{
		triangleIndices.reserve(data.numberoftriangles);

		int* pData = data.edgelist;
		for (size_t i = 0; i < data.numberoftriangles; ++i)
		{
			triangleIndices.push_back((unsigned)*pData);
			++pData;
			triangleIndices.push_back((unsigned)*pData);
			++pData;
			triangleIndices.push_back((unsigned)*pData);
			++pData;
		}
	}

	MeshData pointData, edgeData, triData;
	MeshBuffer pointBuffer, edgeBuffer, triBuffer;

	if (!pointIndices.empty())
	{
		pointData.vertices = vertices;
		pointData.indices = pointIndices;
		pointData.primType = PT_POINT;
		pointBuffer.initBufferFromMeshData(pointData);
		PointRenderable* pPointRenderable = new PointRenderable(pointBuffer);
		Global::renderWindow().addRenderable(pPointRenderable);
		meshGroup.insert(pPointRenderable);
	}


	if (!edgeIndices.empty())
	{
		edgeData.vertices = vertices;
		edgeData.indices = edgeIndices;
		edgeData.primType = PT_LINES;
		edgeBuffer.initBufferFromMeshData(edgeData);

		LineRenderable* pEdgeRenderable = new LineRenderable(edgeBuffer);
		Global::renderWindow().addRenderable(pEdgeRenderable);
		meshGroup.insert(pEdgeRenderable);
	}

	if (!triangleIndices.empty())
	{
		triData.vertices = vertices;
		triData.indices = triangleIndices;
		triData.primType = PT_TRIANGLE;
		triBuffer.initBufferFromMeshData(triData);

		//TODO
	}

}

bool TrianglulationDemo::init()
{
	//quicktest();
	triangulateio input{0};
	std::string paramter_str = fillInputData1(input);
	char* str = const_cast<char*>(paramter_str.c_str());
	triangulateio output{ 0 };
	triangulate(str, &input, &output, nullptr);

	createMeshes(input, m_preMeshes);
	createMeshes(output, m_postMeshes);
	
	free(output.pointlist);
	free(output.pointattributelist);
	free(output.pointmarkerlist);
	free(output.trianglelist);
	free(output.triangleattributelist);
	free(output.trianglearealist);
	free(output.neighborlist);
	free(output.segmentlist);
	free(output.segmentmarkerlist);
	free(output.edgelist);
	free(output.edgemarkerlist);

	Global::cameraControl().fitBox(m_box);

	return true;
}




bool TrianglulationDemo::update()
{


	return true;
}

bool TrianglulationDemo::destroy()
{
	removeMeshGroup(m_preMeshes);
	removeMeshGroup(m_postMeshes);
	return true;
}

void TrianglulationDemo::removeMeshGroup(std::set<IRenderable*>& meshGroup)
{
	for (auto pRenderable : meshGroup)
	{
		Global::renderWindow().removeRenderable(pRenderable);
		delete pRenderable;
	}

	meshGroup.clear();

}