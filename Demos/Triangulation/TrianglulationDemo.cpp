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


//void TrianglulationDemo::quicktest()
//{
//
//		struct triangulateio in, mid, out, vorout;
//
//		/* Define input points. */
//
//		in.numberofpoints = 4;
//		in.numberofpointattributes = 1;
//		in.pointlist = (REAL*)malloc(in.numberofpoints * 2 * sizeof(REAL));
//		in.pointlist[0] = 0.0f;
//		in.pointlist[1] = 0.0f;
//		in.pointlist[2] = 1.0f;
//		in.pointlist[3] = 0.0f;
//		in.pointlist[4] = 1.0f;
//		in.pointlist[5] = 10.0f;
//		in.pointlist[6] = 0.0f;
//		in.pointlist[7] = 10.0f;
//		in.pointattributelist = (REAL*)malloc(in.numberofpoints *
//			in.numberofpointattributes *
//			sizeof(REAL));
//		in.pointattributelist[0] = 0.0f;
//		in.pointattributelist[1] = 1.0f;
//		in.pointattributelist[2] = 11.0f;
//		in.pointattributelist[3] = 10.0f;
//		in.pointmarkerlist = (int*)malloc(in.numberofpoints * sizeof(int));
//		in.pointmarkerlist[0] = 0.f;
//		in.pointmarkerlist[1] = 2.f;
//		in.pointmarkerlist[2] = 0.f;
//		in.pointmarkerlist[3] = 0.f;
//
//		in.numberofsegments = 0.f;
//		in.numberofholes = 0.f;
//		in.numberofregions = 1.f;
//		in.regionlist = (REAL*)malloc(in.numberofregions * 4 * sizeof(REAL));
//		in.regionlist[0] = 0.5f;
//		in.regionlist[1] = 5.0f;
//		in.regionlist[2] = 7.0f;            /* Regional attribute (for whole mesh). */
//		in.regionlist[3] = 0.1f;          /* Area constraint that will not be used. */
//
//		printf("Input point set:\n\n");
//		report(&in, 1, 0, 0, 0, 0, 0);
//
//		/* Make necessary initializations so that Triangle can return a */
//		/*   triangulation in `mid' and a voronoi diagram in `vorout'.  */
//
//		mid.pointlist = (REAL*)NULL;            /* Not needed if -N switch used. */
//		/* Not needed if -N switch used or number of point attributes is zero: */
//		mid.pointattributelist = (REAL*)NULL;
//		mid.pointmarkerlist = (int*)NULL; /* Not needed if -N or -B switch used. */
//		mid.trianglelist = (int*)NULL;          /* Not needed if -E switch used. */
//		/* Not needed if -E switch used or number of triangle attributes is zero: */
//		mid.triangleattributelist = (REAL*)NULL;
//		mid.neighborlist = (int*)NULL;         /* Needed only if -n switch used. */
//		/* Needed only if segments are output (-p or -c) and -P not used: */
//		mid.segmentlist = (int*)NULL;
//		/* Needed only if segments are output (-p or -c) and -P and -B not used: */
//		mid.segmentmarkerlist = (int*)NULL;
//		mid.edgelist = (int*)NULL;             /* Needed only if -e switch used. */
//		mid.edgemarkerlist = (int*)NULL;   /* Needed if -e used and -B not used. */
//
//		vorout.pointlist = (REAL*)NULL;        /* Needed only if -v switch used. */
//		/* Needed only if -v switch used and number of attributes is not zero: */
//		vorout.pointattributelist = (REAL*)NULL;
//		vorout.edgelist = (int*)NULL;          /* Needed only if -v switch used. */
//		vorout.normlist = (REAL*)NULL;         /* Needed only if -v switch used. */
//
//		/* Triangulate the points.  Switches are chosen to read and write a  */
//		/*   PSLG (p), preserve the convex hull (c), number everything from  */
//		/*   zero (z), assign a regional attribute to each element (A), and  */
//		/*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
//		/*   neighbor list (n).                                              */
//
//		triangulate("pczAevn", &in, &mid, &vorout);
//
//		printf("Initial triangulation:\n\n");
//		report(&mid, 1, 1, 1, 1, 1, 0);
//		printf("Initial Voronoi diagram:\n\n");
//		report(&vorout, 0, 0, 0, 0, 1, 1);
//
//		/* Attach area constraints to the triangles in preparation for */
//		/*   refining the triangulation.                               */
//
//		/* Needed only if -r and -a switches used: */
//		mid.trianglearealist = (REAL*)malloc(mid.numberoftriangles * sizeof(REAL));
//		mid.trianglearealist[0] = 3.0;
//		mid.trianglearealist[1] = 1.0;
//
//		/* Make necessary initializations so that Triangle can return a */
//		/*   triangulation in `out'.                                    */
//
//		out.pointlist = (REAL*)NULL;            /* Not needed if -N switch used. */
//		/* Not needed if -N switch used or number of attributes is zero: */
//		out.pointattributelist = (REAL*)NULL;
//		out.trianglelist = (int*)NULL;          /* Not needed if -E switch used. */
//		/* Not needed if -E switch used or number of triangle attributes is zero: */
//		out.triangleattributelist = (REAL*)NULL;
//
//		/* Refine the triangulation according to the attached */
//		/*   triangle area constraints.                       */
//
//		triangulate("prazBP", &mid, &out, (struct triangulateio*) NULL);
//
//		printf("Refined triangulation:\n\n");
//		report(&out, 0, 1, 0, 0, 0, 0);
//
//		/* Free all allocated arrays, including those allocated by Triangle. */
//
//		free(in.pointlist);
//		free(in.pointattributelist);
//		free(in.pointmarkerlist);
//		free(in.regionlist);
//		free(mid.pointlist);
//		free(mid.pointattributelist);
//		free(mid.pointmarkerlist);
//		free(mid.trianglelist);
//		free(mid.triangleattributelist);
//		free(mid.trianglearealist);
//		free(mid.neighborlist);
//		free(mid.segmentlist);
//		free(mid.segmentmarkerlist);
//		free(mid.edgelist);
//		free(mid.edgemarkerlist);
//		free(vorout.pointlist);
//		free(vorout.pointattributelist);
//		free(vorout.edgelist);
//		free(vorout.normlist);
//		free(out.pointlist);
//		free(out.pointattributelist);
//		free(out.trianglelist);
//		free(out.triangleattributelist);
//
//		return ;
//}


#define PUSH_V(x,y) vertices.push_back(Vec2(x,y));

void TrianglulationDemo::fillInputData1(triangulateio& input_data)
{
	input_data = triangulateio{ 0 };
	Vector<Vec2> vertices;

	vertices.push_back(Vec2((3.59f, 3.05f)));
	vertices.push_back(Vec2(7.63f,1.25f));
	vertices.push_back(Vec2(13.97f,1.57f));
	vertices.push_back(Vec2(15.03f, 5.65f));
	vertices.push_back(Vec2(13.41f,9.17f));
	vertices.push_back(Vec2(9.55f, 9.01f));
	vertices.push_back(Vec2(4.17f,7.15f));

	Vector<int> segments;
	segments.reserve(2 * vertices.size());
	for (int i = 0, n = (int)vertices.size(); i < n; ++i)
	{
		segments.push_back(i);
		segments.push_back(i+1);
	}
	segments.back() = 0;

	int vertexOffset = (int)vertices.size();

	PUSH_V(7.47, 8.21);
	vertices.push_back(Vec2(8.13,7.11));
	vertices.push_back(Vec2(8.87,5.75));
	vertices.push_back(Vec2(10,4));
	PUSH_V(11.25, 1.65);


	for (int i = vertexOffset; i < vertices.size() - 1; ++i)
	{
		segments.push_back(i);
		segments.push_back(i + 1);
	}

	input_data.numberofpoints = (int)vertices.size();
	input_data.pointlist = new float[vertices.size() * 2];
	memcpy_s(input_data.pointlist, vertices.size() * 2 * sizeof(float), vertices.data(), vertices.size() * sizeof(Vec2));


	input_data.numberofsegments = (int)segments.size() / 2;
	input_data.segmentlist = new int[segments.size()];
	memcpy_s(input_data.segmentlist, sizeof(int) * segments.size(), segments.data(), segments.size() * sizeof(int));

	return;
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
		pointIndices.push_back((unsigned)i);
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
	else if (data.numberofsegments > 0)
	{
		edgeIndices.reserve(data.numberofsegments);

		int* pData = data.segmentlist;
		for (size_t i = 0; i < data.numberofsegments; ++i)
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
	m_pUI = new TriangulationUIView(this);
	Global::uiManager().addUIView(m_pUI);

	//quicktest();
	triangulateio input{0};
	fillInputData1(input);
	createMeshes(input, m_preMeshes);

	triangulateio output{ 0 };
	triangulate("pzea0.5", &input, &output, nullptr);
	createMeshes(output, m_postMeshes);
	
	//triangulateio tmp{ 0 };
	//triangulateio tmp1{ 0 };
	//triangulateio tmp2{ 0 };
	//triangulateio tmp3{ 0 };
	//triangulateio refine{ 0 };
	//triangulate("pze", &input, &tmp, nullptr);
	//triangulate("pzea2.0f", &tmp, &tmp1, nullptr);
	//triangulate("pzea2.0f", &tmp1, &tmp2, nullptr);
	//triangulate("pzea1.0f", &tmp2, &tmp3, nullptr);
	//triangulate("pzea0.5f", &tmp3, &refine, nullptr);

	//createMeshes(tmp1, m_refineMesh);

	displayPostModel();

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
	removeMeshGroup(m_refineMesh);

	
	Global::uiManager().removeUIView(m_pUI);
	delete m_pUI;
	m_pUI = nullptr;

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

void TrianglulationDemo::showMeshGroup(const std::set<IRenderable*>& meshGroup, bool enable)
{
	for (auto pRenderable : meshGroup)
		pRenderable->setEnabel(enable);

}

void TrianglulationDemo::displayPreModel()
{
	showMeshGroup(m_preMeshes,true);
	showMeshGroup(m_postMeshes, false);
	showMeshGroup(m_refineMesh, false);
}

void TrianglulationDemo::displayPostModel()
{
	showMeshGroup(m_preMeshes, false);
	showMeshGroup(m_postMeshes, true);
	showMeshGroup(m_refineMesh, false);
}

void TrianglulationDemo::displayRefineModel()
{
	showMeshGroup(m_preMeshes, false);
	showMeshGroup(m_postMeshes, false);
	showMeshGroup(m_refineMesh, true);
}