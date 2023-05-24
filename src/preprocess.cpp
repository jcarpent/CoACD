#include "./preprocess.h"
#include "logger.h"

namespace coacd
{
    void SDFManifold(Model &input, Model &output, double scale, double level_set)
    {
        std::vector<Vec3s> points;
        std::vector<Vec3I> tris;
        std::vector<Vec4I> quads;

        logger::info(" - Preprocess");
        logger::info("Preprocess resolution: {}", scale);

        clock_t start, end;
        start = clock();

        for (unsigned int i = 0; i < input.points.size(); ++i)
        {
            points.push_back({
                static_cast<float>(input.points[i][0] * scale), 
                static_cast<float>(input.points[i][1] * scale), 
                static_cast<float>(input.points[i][2] * scale)});
        }
        for (unsigned int i = 0; i < input.triangles.size(); ++i)
        {
            tris.push_back({(unsigned int)input.triangles[i][0], (unsigned int)input.triangles[i][1], (unsigned int)input.triangles[i][2]});
        }

        math::Transform::Ptr xform = math::Transform::createLinearTransform();
        tools::QuadAndTriangleDataAdapter<Vec3s, Vec3I> mesh(points, tris);

        DoubleGrid::Ptr sgrid = tools::meshToSignedDistanceField<DoubleGrid>(
            *xform, points, tris, quads, 3.0, 3.0);

        std::vector<Vec3s> newPoints;
        std::vector<Vec3I> newTriangles;
        std::vector<Vec4I> newQuads;
        tools::volumeToMesh(*sgrid, newPoints, newTriangles, newQuads, 0.55);

        output.Clear();
        for (unsigned int i = 0; i < newPoints.size(); ++i)
        {
            output.points.push_back({newPoints[i][0] / scale, newPoints[i][1] / scale, newPoints[i][2] / scale});
        }
        for (unsigned int i = 0; i < newTriangles.size(); ++i)
        {
            output.triangles.push_back({(int)newTriangles[i][0], (int)newTriangles[i][2], (int)newTriangles[i][1]});
        }
        for (unsigned int i = 0; i < newQuads.size(); ++i)
        {
            output.triangles.push_back({(int)newQuads[i][0], (int)newQuads[i][2], (int)newQuads[i][1]});
            output.triangles.push_back({(int)newQuads[i][0], (int)newQuads[i][3], (int)newQuads[i][2]});
        }
        end = clock();

        logger::info("Preprocess Time: {}s", double(end - start) / CLOCKS_PER_SEC);
    }

}